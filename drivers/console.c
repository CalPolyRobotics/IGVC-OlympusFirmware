#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>

#include "main.h"
#include "usart.h"
#include "led.h"
#include "i2c.h"
#include "pwradc.h"
#include "sevenSeg.h"
#include "speedDAC.h"
#include "kill.h"
#include "comms.h"
#include "usb_otg.h"
#include "adc.h"
#include "characterMapping.h"

#define CONSOLE_MAX_CMD_LEN     255
#define CONSOLE_MAX_NUM_ARGS    32

typedef void (*commandCallback)(uint32_t, char**);

typedef struct {
    char* cmdStr;
    uint32_t minArguments;
    commandCallback callback;
} ConsoleCommand;

typedef enum {CONSOLE_START = 0, CONSOLE_READING} ConsoleState;

typedef enum {CONSOLE_ECHO_ON = 0, CONSOLE_ECHO_OFF, CONSOLE_ECHO_MAP, CONSOLE_ECHO_DEBUG} EchoState;

static void console_i2cWrite(uint32_t, char**);
//static void console_i2cRead(uint32_t, char**);
static void console_i2cScan(uint32_t, char**);
//static void console_setGPIO(uint32_t, char**);
static void console_setLED(uint32_t, char**);
static void console_setSegment(uint32_t, char**);
static void console_measPower(uint32_t, char**);
static void console_kill(uint32_t, char**);
static void console_writeFNR(uint32_t, char**);
static void console_readFNR(uint32_t, char**);
static void console_writeSpeed(uint32_t, char**);
static void console_readSpeed(uint32_t, char**);
static void console_setEnableSpeed(uint32_t, char**);
static void console_emulateUSB(uint32_t, char**);
static void console_setSteerAngle(uint32_t, char**);
static void console_USBWrite(uint32_t, char**);
static void console_readSteeringDir(uint32_t, char**);
static void console_setRawSteerAngle(uint32_t, char**);
static void console_getSteerTarget(uint32_t, char**);
static void console_getSteerValue(uint32_t, char**);
static void console_enableSteering(uint32_t, char**);
static void console_hardmode(uint32_t, char**);
static void console_help(uint32_t, char**);
static void console_clear(uint32_t, char**);

static ConsoleCommand commands[] = {
    {"i2cWrite", 2, console_i2cWrite},
    //{"i2cRead", 2, console_i2cRead},
    {"i2cScan", 0, console_i2cScan},
    //{"setGPIO", 3, console_setGPIO},
    {"setLED", 2, console_setLED},
    {"setSegment", 2, console_setSegment},
    {"measPower", 1, console_measPower},
    {"kill", 0, console_kill},
    {"writeFNR", 1, console_writeFNR},
    {"readFNR", 0, console_readFNR},
    {"writeSpeed", 1, console_writeSpeed},
    {"readSpeed", 1, console_readSpeed},
    {"setEnableSpeed", 0, console_setEnableSpeed},
    {"readSteeringDir", 0, console_readSteeringDir},
    {"emulateUSB", 1, console_emulateUSB},
    {"setSteerAngle", 2, console_setSteerAngle},
    {"setRawSteerAngle", 1, console_setRawSteerAngle},
    {"USBWrite", 1, console_USBWrite},
    {"getSteerTarget", 0, console_getSteerTarget},
    {"getSteerValue", 0, console_getSteerValue},
    {"enableSteering", 1, console_enableSteering},
    {"hardmode", 1, console_hardmode},
    {"help", 0, console_help},
    {"clear", 0, console_clear},
    {NULL, 0, NULL}
};

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

static void console_i2cWrite(uint32_t argc, char** argv)
{
    //uint8_t addr;
    //uint8_t dataBytes[CONSOLE_MAX_NUM_ARGS];
    //uint32_t i;

    //addr = atoi(argv[0]);

    //for (i = 1; i < argc; i++)
    //{
        //dataBytes[i-1] = atoi(argv[i]);
    //}

    //i2cTransmit(addr, dataBytes, argc-1);
}

/*
static void console_i2cRead(uint32_t argc, char** argv) 
{

}
*/

static void console_i2cScan(uint32_t argc, char** argv)
{
    //i2cScan();
}

/*
static void console_setGPIO(uint32_t argc, char** argv)
{

}
*/

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

static char segmentCharFromChar(char c){
    if (c >= '0' && c <= '9')
    {
        return c - '0';
    } else if(c >= 'a' && c <= 'f'){
        return c - 'a' + 0xA;
    } else {
        return c;
    }
}

static void console_setSegment(uint32_t argc, char** argv)
{
    setSevenSeg(segmentCharFromChar(*argv[0]), segmentCharFromChar(*argv[1]));
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

static void console_measPower(uint32_t argc, char** argv)
{
    const struct adc_cmd *val;

    for (val = adc_dict; val->cmd; val++) {
        if (!strncmp(val->cmd, argv[0], strlen(val->cmd))) {
            printf("%s: %d", val->cmd, adc_conv(val->per));
            break;
        }
    }
}

static void console_kill(uint32_t argc, char** argv)
{
    killBoard();
}

static void console_writeFNR(uint32_t argc, char** argv)
{
    /** TODO **/
}

static void console_readFNR(uint32_t argc, char** argv)
{
    /** TODO **/
}

static void console_writeSpeed(uint32_t argc, char** argv)
{
    uint16_t num = parseUint16(argv[0]);
    if(!errno){
        writeSpeedDAC(num);
    }
}


static void console_readSpeed(uint32_t argc, char** argv)
{
    printf("%u\r\n", getSpeedDAC());
}

/**
 * A value of zero will disable the speed output,
 * a non-zero value [0,UINT16_MAX] will enable the speed output,
 */
static void console_setEnableSpeed(uint32_t argc, char** argv){
    uint16_t num = parseUint16(argv[0]);
    if(!errno){
        if(num){
            enableSpeedDAC();
        }else{
            disableSpeedDAC();
        }
    }
}

static void console_emulateUSB(uint32_t argc, char** argv)
{
    uint32_t i;
    uint32_t numPacketArgs = argc - 1;

    char buf[sizeof(PacketHeader_t)];
    PacketHeader_t* header = (PacketHeader_t*)buf;
    header->startByte[0] = 0xF0;
    header->startByte[1] = 0x5A;
    header->msgType = atoi(argv[0]);
    header->seqNumber = 0;
    header->packetLen = sizeof(PacketHeader_t) + 1 + numPacketArgs;

    for (i = 0; i < sizeof(PacketHeader_t); i++)
    {
        runCommsFSM(buf[i]);
    }

    for (i = 0; i < numPacketArgs; i++)
    {
        runCommsFSM(strtol(argv[i + 1], NULL, 16) & 0xFF);
    }
}

static void console_setSteerAngle(uint32_t argc, char** argv)
{
    /** TODO **/
}

static void console_setRawSteerAngle(uint32_t argc, char** argv)
{
    /** TODO **/
}

static void console_getSteerTarget(uint32_t argc, char** argv)
{
    /** TODO **/
}

static void console_getSteerValue(uint32_t argc, char** argv)
{
    /** TODO **/
}

static void console_readSteeringDir(uint32_t argc, char** argv)
{
    /** TODO **/
}

static void console_USBWrite(uint32_t argc, char** argv)
{
    printf("%s", argv[0]);
    usbWrite((uint8_t*)argv[0], strlen(argv[0]));
}

static void console_enableSteering(uint32_t argc, char** argv)
{
    /** TODO **/
}

static void console_hardmode(uint32_t argc, char** argv)
{
    echoCharacters = strtol(argv[0], NULL, 10);
}

static void console_help(uint32_t argc, char** argv) 
{
    uint32_t i = 0;
    printf("***Command Summary***");
    while (commands[i].cmdStr != NULL) {
        printf("%s", commands[i].cmdStr);
        printf("\t requires %lu arguments\n", commands[i].minArguments);
        i++;
    }
}

static void console_clear(uint32_t argc, char** argv){
    // Move to col 1 row 1
    printf("\033[H");
    printf("\033[J");
}
