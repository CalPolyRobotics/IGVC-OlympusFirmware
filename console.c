
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "main.h"
#include "usart.h"
#include "led.h"
#include "i2c.h"
#include "sevenSeg.h"
#include "kill.h"
#include "fnr.h"
#include "comms.h"
#include "speedDAC.h"
#include "usb_otg.h"

#define CONSOLE_MAX_CMD_LEN     255
#define CONSOLE_MAX_NUM_ARGS    32

typedef void (*commandCallback)(uint32_t, char**);

typedef struct {
    char* cmdStr;
    uint32_t minArguments;
    commandCallback callback;
} ConsoleCommand;

typedef enum {CONSOLE_START = 0, CONSOLE_READING} ConsoleState;

static void console_i2cWrite(uint32_t, char**);
static void console_i2cRead(uint32_t, char**);
static void console_i2cScan(uint32_t, char**);
static void console_setGPIO(uint32_t, char**);
static void console_setLED(uint32_t, char**);
static void console_setSegment(uint32_t, char**);
static void console_measPower(uint32_t, char**);
static void console_kill(uint32_t, char**);
static void console_writeFNR(uint32_t, char**);
static void console_readFNR(uint32_t, char**);
static void console_writeSpeed(uint32_t, char**);
static void console_readSpeed(uint32_t, char**);
static void console_readBatt(uint32_t, char**);
static void console_emulateUSB(uint32_t, char**);
static void console_setSteerAngle(uint32_t, char**);
static void console_USBWrite(uint32_t, char**);

static ConsoleCommand commands[] = {
    {"i2cWrite", 2, console_i2cWrite},
    {"i2cRead", 2, console_i2cRead},
    {"i2cScan", 0, console_i2cScan},
    {"setGPIO", 3, console_setGPIO},
    {"setLED", 2, console_setLED},
    {"setSegment", 1, console_setSegment},
    {"measPower", 2, console_measPower},
    {"kill", 0, console_kill},
    {"writeFNR", 1, console_writeFNR},
    {"readFNR", 0, console_readFNR},
    {"writeSpeed", 1, console_writeSpeed},
    {"readSpeed", 0, console_readSpeed},
    {"readBatt", 1, console_readBatt},
    {"emulateUSB", 1, console_emulateUSB},
    {"setSteerAngle", 1, console_setSteerAngle},
    {"USBWrite", 1, console_USBWrite},
    {NULL, 0, NULL}
};

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
    int i = 0;
    char* argv[CONSOLE_MAX_NUM_ARGS];
    uint32_t argc;

    while (commands[i].cmdStr != NULL)
    {
        if (strncmp(cmd, commands[i].cmdStr, strlen(commands[i].cmdStr)) == 0)
        {
            argc = createArgv(cmd, argv);

            printf("\r\n<");
            if (argc < commands[i].minArguments)
            {
                printf("ERROR: Need %lu Args", commands[i].minArguments);
            } else {
                commands[i].callback(argc, argv);
            }
            printf(">\r\n");

            break;
        }
        i++;
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

    uint8_t data;

    while (usartRead(&data, 1))
    {
        usartPut(data);
        switch(state) {
            case CONSOLE_START:
                if (data == '[')
                {
                    state = CONSOLE_READING;
                    memset(cmdBuffer, 0, CONSOLE_MAX_CMD_LEN);
                    cmdBufferIdx = 0;
                }
                break;
            case CONSOLE_READING:
                if (data == ']')
                {
                    processCommand(cmdBuffer);
                    state = CONSOLE_START;
                } else {
                    if (cmdBufferIdx < CONSOLE_MAX_CMD_LEN)
                    {
                        cmdBuffer[cmdBufferIdx] = data;
                        cmdBufferIdx++;
                    } else {
                        state = CONSOLE_START;
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
    uint8_t addr;
    uint8_t dataBytes[CONSOLE_MAX_NUM_ARGS];
    uint32_t i;

    addr = atoi(argv[0]);

    for (i = 1; i < argc; i++)
    {
        dataBytes[i-1] = atoi(argv[i]);
    }

    i2cTransmit(addr, dataBytes, argc-1);
}

static void console_i2cRead(uint32_t argc, char** argv) 
{

}

static void console_i2cScan(uint32_t argc, char** argv)
{
    i2cScan();
}

static void console_setGPIO(uint32_t argc, char** argv)
{

}

static void console_setLED(uint32_t argc, char** argv)
{
    uint8_t ledNum = atoi(argv[0]);
    uint8_t state = atoi(argv[1]);

    setLED(ledNum, state);

    if (state)
    {
        printf("%d, SET", ledNum);
    } else {
        printf("%d, RESET", ledNum);
    }
}

static void console_setSegment(uint32_t argc, char** argv)
{
    if (*argv[0] >= '0' && *argv[0] <= '9')
    {
        setSevenSeg(*argv[0] - '0');
    } else if(*argv[0] >= 'A' && *argv[0] <= 'F') {
        setSevenSeg(*argv[0] - 'A' + 0xA);
    } else {
        setSevenSeg(*argv[0]);
    }
}

static void console_measPower(uint32_t argc, char** argv)
{

}

static void console_kill(uint32_t argc, char** argv)
{
    killBoard();
}

static void console_writeFNR(uint32_t argc, char** argv)
{
    FNR_t fnr = atoi(argv[0]);
    if (fnr >= 0 && fnr <= 2)
    {
        setFNR(fnr);
        switch(fnr)
        {
            case FNR_NEUTRAL:
                printf("Neutral");
                break;
            case FNR_FORWARD:
                printf("Forward");
                break;
            case FNR_REVERSE:
                printf("Reverse");
                break;
        }
    } else {
        printf("Invalid State");
    }
}

static void console_readFNR(uint32_t argc, char** argv)
{

}

static void console_writeSpeed(uint32_t argc, char** argv)
{
    setSpeedDAC(atoi(argv[0]));
}

static void console_readSpeed(uint32_t argc, char** argv)
{

}

static void console_readBatt(uint32_t argc, char** argv)
{

}

static void console_emulateUSB(uint32_t argc, char** argv)
{
    uint32_t i;
    uint32_t numPacketArgs = argc - 1;

    char buf[sizeof(PacketHeader_t)];
    PacketHeader_t* header = (PacketHeader_t*)buf;;
    header->startByte[0] = 0xF0;
    header->startByte[1] = 0x5A;
    header->CRC8 = 0;
    header->msgType = atoi(argv[0]);
    header->seqNumber = 0;
    header->packetLen = sizeof(PacketHeader_t) + numPacketArgs;

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

}

static void console_USBWrite(uint32_t argc, char** argv)
{
    printf("%s", argv[0]);
    usbWrite((uint8_t*)argv[0], strlen(argv[0]));
}
