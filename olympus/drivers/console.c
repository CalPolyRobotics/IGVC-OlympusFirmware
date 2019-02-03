#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>

#include "usart.h"
#include "submoduleComms.h"
#include "led.h"
#include "pwradc.h"
#include "sevenSeg.h"
#include "speedDAC.h"
#include "kill.h"
#include "comms.h"
#include "usb_otg.h"
#include "adc.h"
#include "characterMapping.h"

#include "hera.h"
#include "hephaestus.h"
#include "janus.h"

#include <stdlib.h>

#define CONSOLE_MAX_CMD_LEN     255
#define CONSOLE_MAX_NUM_ARGS    32

typedef void (*commandCallback)(uint32_t, char**);

typedef struct {
    char* cmdStr;
    char* argStr;
    uint32_t minArguments;
    commandCallback callback;
} ConsoleCommand;

typedef enum {
    CONSOLE_START = 0,
    CONSOLE_READING
}ConsoleState;

typedef enum {
    CONSOLE_ECHO_ON = 0,
    CONSOLE_ECHO_OFF,
    CONSOLE_ECHO_MAP,
    CONSOLE_ECHO_DEBUG
}EchoState;


// Board Commands
static void console_setLED(uint32_t, char**);
static void console_setSegment(uint32_t, char**);
static void console_getPower(uint32_t, char**);
static void console_kill(uint32_t, char**);

// FNR Commands
static void console_setFNR(uint32_t, char**);
static void console_getFNR(uint32_t, char**);

// Speed Commands
static void console_setSpeedTarget(uint32_t, char**);
static void console_getSpeedTarget(uint32_t, char**);

// Steering Commands
static void console_setSteerTarget(uint32_t, char**);
static void console_getSteerPot(uint32_t, char**);

// Console Commands
static void console_help(uint32_t, char**);
static void console_clear(uint32_t, char**);
static void console_setEchoMode(uint32_t, char**);


static ConsoleCommand commands[] = {
    /*cmdStr,          argStr                             minArgs callback*/
    {"setSegment",     "char1<a-f|0-9>, char2<a-f|0-9>" , 2, console_setSegment},
    {"setLED",         "ledNum<0-5>, state<0-1>",         2, console_setLED},
    {"setFNR",         "state<0-2>",                      1, console_setFNR},
    {"setSpeedTarget", "target<0-65535>",                 1, console_setSpeedTarget},
    {"setSteerTarget", "angle<45-125>",                   1, console_setSteerTarget},
    {"setEchoMode",    "mode<0-3>",                       1, console_setEchoMode},
    {"getFNR",         NULL,                              0, console_getFNR},
    {"getSpeedTarget", NULL,                              0, console_getSpeedTarget},
    {"getSteerPot",    NULL,                              0, console_getSteerPot},
    {"getPower",       NULL,                              0, console_getPower},
    {"kill",           NULL,                              0, console_kill},
    {"help",           NULL,                              0, console_help},
    {"clear",          NULL,                              0, console_clear},
    {NULL,             NULL,                              0, NULL}
};

static int parseUint8(char* str){
    long int num = strtol(str, NULL, 10);
    if(errno || num < 0 || num > UINT8_MAX){
        printf("Could not parse input\r\n");
    }

    return (uint8_t)num;
}

static int parseUint16(char* str){
    long int num = strtol(str, NULL, 10);
    if(errno || num < 0 || num > UINT16_MAX){
        printf("Could not parse input\r\n");
    }

    return (uint16_t)num;
}

static EchoState echoCharacters = CONSOLE_ECHO_ON;

static void rewriteBuffer(char* characterBuffer, uint8_t color)
{
    uint32_t i;
    uint8_t tempMapping[10] = {0};

    if (echoCharacters != CONSOLE_ECHO_OFF)
    {
        // Move cursor left
        if (echoCharacters == CONSOLE_ECHO_MAP)
        {
            printf("\033[2G\033[0K");
        } else {
            //printf("\033[%dD", strlen(characterBuffer));
            printf("\033[2G\033[0K");
        }

        if (color < 10)
        {
            // Set color
            printf("\033[%dm", 30 + color);
        }

        for (i = 0; i < strlen(characterBuffer); i++)
        {
            switch (echoCharacters)
            {
                case CONSOLE_ECHO_ON:
                    printf("%c", characterBuffer[i]);
                    break;

                case CONSOLE_ECHO_MAP:
                    memset(tempMapping, 0, 10);
                    mapCharacter(characterBuffer[i], tempMapping);
                    printf("%s", tempMapping);
                    break;

                case CONSOLE_ECHO_OFF:
                case CONSOLE_ECHO_DEBUG:
                default:
                    break;
            }
        }
    }
}

static int32_t matchCommand(char* buffer)
{
    uint32_t i = 0;
    uint32_t commandLen;
    char* commaPtr;

    commaPtr = strchr(buffer, ',');
    if (commaPtr == NULL)
    {
        commandLen = strlen(buffer);
    } else {
        commandLen = commaPtr - buffer;
    }

    while (commands[i].cmdStr != NULL)
    {
        if (strncmp(buffer, commands[i].cmdStr, commandLen) == 0 &&
            commandLen == strlen(commands[i].cmdStr))
        {
            return i;
        }

        i++;
    }

    return -1;
}

static uint32_t createArgv(char* cmd, char** argv)
{
    uint32_t numArgs = 0;

    while (numArgs < CONSOLE_MAX_NUM_ARGS && *cmd != '\0')
    {
        if (*cmd == ',')
        {
            *cmd = '\0';
            argv[numArgs] = cmd + 1;
            numArgs++;
        }

        cmd++;
    }

    argv[numArgs] = NULL;

    return numArgs;
}

static void processCommand(char* cmd)
{
    int32_t i = 0;
    char* argv[CONSOLE_MAX_NUM_ARGS];
    uint32_t argc;


    if ((i = matchCommand(cmd)) >= 0)
    {
        argc = createArgv(cmd, argv);

        printf("\r\n<");
        if (argc < commands[i].minArguments)
        {
            printf("ERROR: Need %lu Args", commands[i].minArguments);
        } else { 
            commands[i].callback(argc, argv);
        }

        if(strcmp(commands[i].cmdStr,"clear") != 0){
            printf(">\r\n");
        }

    }

    if (commands[i].cmdStr == NULL)
    {
        printf("\r\n<ERROR: Command Not Found>\r\n");
    }
}

void consoleProcessBytes()
{
    static ConsoleState state = CONSOLE_START;
    static char cmdBuffer[CONSOLE_MAX_CMD_LEN];
    static uint32_t cmdBufferIdx;
    uint8_t tempMapping[10] = {0};

    uint8_t data;

    while (usartRead(&data, 1))
    {

        
        if (data == 0x7F)
        { 
            // Data is a Backspace
            if (cmdBufferIdx > 0)
            {
                cmdBufferIdx--;
                cmdBuffer[cmdBufferIdx] = 0;

                // Left One and delete to end of line
                printf("\033[1D\033[0K");

                if (cmdBufferIdx > 0)
                {
                    if (matchCommand(cmdBuffer) != -1)
                    {
                        rewriteBuffer(cmdBuffer, 2);
                    } else {
                        rewriteBuffer(cmdBuffer, 1);
                    } 
                }
            }

            continue;

        } else {
            switch(echoCharacters)
            {
                case CONSOLE_ECHO_ON:
                    printf("%c", data);
                    break;
                
                case CONSOLE_ECHO_OFF:
                    break;

                case CONSOLE_ECHO_MAP:
                    mapCharacter(data, tempMapping);
                    printf("%s", tempMapping);
                    break;

                case CONSOLE_ECHO_DEBUG:
                    printf("%d", data);
                    break;
            }   
        }

        switch(state) {
            case CONSOLE_START:
                if (data == '[')
                {
                    state = CONSOLE_READING;
                    cmdBufferIdx = 0;
                }
                break;
            case CONSOLE_READING:
                if (data == ']')
                {
                    // Remove Ending ]
                    printf("\033[1D\033[0K");

                    // Clear Color
                    printf("\033[0m");
                    
                    // Redraw ]
                    printf("]");

                    processCommand(cmdBuffer);

                    // Reset cmdBuffer
                    memset(cmdBuffer, 0, CONSOLE_MAX_CMD_LEN);

                    state = CONSOLE_START;
                } else {
                    if (cmdBufferIdx < CONSOLE_MAX_CMD_LEN)
                    {
                        cmdBuffer[cmdBufferIdx] = data;
                        cmdBufferIdx++;
                    } else {
                        state = CONSOLE_START;
                    }

                    if (matchCommand(cmdBuffer) != -1)
                    {
                        rewriteBuffer(cmdBuffer, 2);
                    } else {
                        rewriteBuffer(cmdBuffer, 1);
                    }
                }
                break;
            default:
                state = CONSOLE_START;
                break;
        }
    }
}

static void console_setLED(uint32_t argc, char** argv)
{
    uint8_t ledNum = atoi(argv[0]);
    uint8_t state = atoi(argv[1]);

    writeLED(ledNum, state);

    if (state)
    {
        printf("%d, SET", ledNum);
    } else {
        printf("%d, RESET", ledNum);
    }
}

static void console_setSegment(uint32_t argc, char** argv)
{
    setSevenSeg(*argv[0], *argv[1]);
}

struct adc_cmd {
    char *cmd;
    enum adc_periph per;
};

static const struct adc_cmd adc_dict[] = {
    {"batt_i", batt_i},
    {"fv_v",   fv_v},
    {"thr_v",  thr_v},
    {"batt_v", batt_v},
    {"twlv_i", twlv_i},
    {"twlv_v", twlv_v},
    {"fv_i",   fv_i},
    {"thr_i",  thr_i},
    {0}
};

static void console_getPower(uint32_t argc, char** argv)
{
    const struct adc_cmd *val;

    for (val = adc_dict; val->cmd; val++) {
        printf("%s: %d\r\n", val->cmd, adc_conv(val->per));
    }
}

static void console_kill(uint32_t argc, char** argv)
{
    killBoard();
}

static void console_setFNR(uint32_t argc, char** argv)
{
    uint8_t state = parseUint8(argv[0]);
    if (state <= 2)
    {
        setFNR(state);
    }
}

static void console_getFNR(uint32_t argc, char** argv)
{
    printf("%u", *janusData.fnr);
}

static void console_setSpeedTarget(uint32_t argc, char** argv)
{
    uint16_t num = parseUint16(argv[0]);
    if(!errno){
        writeSpeedDAC(num);
    }
}


static void console_getSpeedTarget(uint32_t argc, char** argv)
{
    printf("%u\r\n", getSpeedDAC());
}

static void console_setSteerTarget(uint32_t argc, char** argv)
{
    uint8_t angle = parseUint8(argv[0]);
    if(!errno && angle < 180)
    {
        if(setHephaestusSteering(angle) != COMMS_OK){
            printf("setHephaestusSteering Failed\r\n");
        }
    }
    else
    {
        printf("Invalid Argument");
    }
}

static void console_getSteerPot(uint32_t argc, char** argv)
{
    printf("%u", *((uint16_t*)heraData.steer));
}

static void console_setEchoMode(uint32_t argc, char** argv)
{
    echoCharacters = strtol(argv[0], NULL, 10);
}

static void console_help(uint32_t argc, char** argv) 
{
    uint32_t i = 0;
    printf("\r\n  \033[2m\033[4mCommand\t\tArgs\033[0m\r\n");

    while (commands[i].cmdStr != NULL)
    {
        if(commands[i].argStr == NULL)
        {
            printf("  %s\r\n", commands[i].cmdStr);
        }
        else
        {
            printf("  %s%s[%s]\r\n", commands[i].cmdStr,
                   strlen(commands[i].cmdStr) >= 12 ? "\t" : "\t\t",
                   commands[i].argStr);
        }

        i++;
    }
}

static void console_clear(uint32_t argc, char** argv){
    // Move to col 1 row 1
    printf("\033[H");
    printf("\033[J");
}
