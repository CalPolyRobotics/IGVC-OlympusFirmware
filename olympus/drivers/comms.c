#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>

#include "comms.h"
#include "submoduleComms.h"
#include "led.h"
#include "usart.h"
#include "usb_otg.h"
#include "pwradc.h"
#include "speedDAC.h"
#include "kill.h"
#include "crc8.h"
#include "adc.h"
#include "apollo.h"
#include "janus.h"
#include "hephaestus.h"

#define HEADER_START_SIZE 2
#define COMMS_START_BYTE 0xF0
#define MAX_PACKET_SIZE 64

#define START_BYTE_1 0xF0
#define START_BYTE_2 0x5A

typedef enum {
    WAITING_FOR_START_1 = 0,
    WAITING_FOR_START_2,
    WAITING_FOR_HEADER,
    WAITING_FOR_DATA,
    WAITING_FOR_CRC
} CommsState_t;

typedef struct {
    uint32_t inputDataLen;
    uint8_t* inputData;
    uint32_t responseDataLen;
    uint8_t* responseData;
    commsCallback callback;
} packetResponse_t;

void toggleLED(Packet_t* packet)
{
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_4);
}

void toggleLED2(Packet_t* packet)
{
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
}

void toggleLED3(Packet_t* packet)
{
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_7);
}

/** XXX - Remove these when they are reimplemented **/
static uint8_t status[5] = {0};

olympusData_t olympusData;

static void commsSetTurnSignal(uint8_t* data){return;}
static void commsSetHeadlights(uint8_t* data){return;}
static void commsSetMiscLights(uint8_t* data){return;}
static void commsSetBrake(uint8_t* data){return;}
static void commsSetSpeed(uint8_t* data){
    writeSpeedDAC((( (uint16_t) data[1]) << 8) | data[0]);
    return;
}
static uint8_t inputBuf[256];

static packetResponse_t response[] = {
    {0u,    NULL,     5u,   status,                  NULL},                // (0x00)

    // Hera
    {0u,    NULL,     2u,   NULL,                    NULL},                // (0x02)
    {0u,    NULL,     2u,   NULL,                    NULL},                // (0x04)
    {0u,    NULL,     8u,   NULL,                    NULL},                // (0x06)

    // Apollo
    {1u,    inputBuf, 0u,   NULL,                    commsSetTurnSignal},  // (0x08)
    {2u,    inputBuf, 0u,   NULL,                    commsSetHeadlights},  // (0x0A)
    {2u,    inputBuf, 0u,   NULL,                    commsSetMiscLights},  // (0x0C)

    // Janus
    {0u,    NULL,     1u,   janusData.fnr,           NULL},                // (0x0E)
    {0u,    NULL,     1u,   janusData.ctrl,          NULL},                // (0x10)
    {1u,    inputBuf, 0u,   NULL,                    commsSetFNR},         // (0x12)

    // Hephaestus
    {1u,    inputBuf, 0u,   NULL,                    commsSetSteering},    // (0x14)
    {2u,    inputBuf, 0u,   NULL,                    commsSetBrake},       // (0x16)
    {0u,    NULL,     0u,   NULL,                    NULL},                // (0x18)

    // Olympus
    {2u,    inputBuf, 0u,   NULL,                    commsSetSpeed},       // (0x1A)
    {2u,    inputBuf, 0u,   NULL,                    commsSetLeds},        // (0x1C)
    {0u,    NULL,     16u,  olympusData.power.u8,    commsPwradcCallback},                // (0x1E)
    {0u,    NULL,     0u,   NULL,                    killBoard},           // (0x20)
    {4u,    inputBuf, 0u,   NULL,                    resetBoard}           // (0x22)
};

static bool checkPacket(Packet_t* packet)
{
    return crc8(packet, packet->header.packetLen) == 0;
}


static void runPacket(Packet_t* packet)
{
    uint32_t bytesToCopy, idx;
    uint8_t packetType = packet->header.msgType >> 1;

    // CRC is of length 1 and is not included in PacketHeader_t
    uint8_t dataLen = packet->header.packetLen - sizeof(PacketHeader_t) - 1;

    if (response[packetType].inputDataLen > 0)
    {
        if (response[packetType].inputDataLen > dataLen)
        {
            bytesToCopy = dataLen;
        } else {
            bytesToCopy = response[packetType].inputDataLen;
        }

        for (idx = 0; idx < bytesToCopy; idx++)
        {
            response[packetType].inputData[idx] = packet->data[idx];
        }
    }

    if (response[packetType].callback != NULL)
    {
        response[packetType].callback(response[packetType].inputData);
    }
}

static void sendResponse(Packet_t* packet)
{
    static uint8_t packetBuffer[MAX_PACKET_SIZE];
    Packet_t* outPacket = (Packet_t*)packetBuffer;
    uint8_t packetType = packet->header.msgType >> 1;
    uint32_t idx;

    outPacket->header.startByte[0] = START_BYTE_1;
    outPacket->header.startByte[1] = START_BYTE_2;
    outPacket->header.msgType = packet->header.msgType | 1;
    outPacket->header.seqNumber = packet->header.seqNumber;

    // CRC is of length 1
    outPacket->header.packetLen = response[packetType].responseDataLen + sizeof(PacketHeader_t) + 1;

    for (idx = 0; idx < response[packetType].responseDataLen; idx++)
    {
        outPacket->data[idx] = response[packetType].responseData[idx];
    }

    outPacket->data[idx] = crc8(outPacket, outPacket->header.packetLen - 1);

    usbWrite(packetBuffer, outPacket->header.packetLen);
}

void runCommsFSM(char data)
{
    static CommsState_t state = WAITING_FOR_START_1;
    static uint8_t packetBuffer[MAX_PACKET_SIZE];
    static Packet_t* packet = (Packet_t*)packetBuffer;
    static uint32_t packetIdx;

    switch(state)
    {
        case WAITING_FOR_START_1:
        {
            packetIdx = 0;
            if (data == START_BYTE_1)
            {
                state = WAITING_FOR_START_2;
                packetBuffer[0] = data;
                packetIdx = 1;
            }
        }
        break;

        case WAITING_FOR_START_2:
        {
            if (data == START_BYTE_2)
            {
                state = WAITING_FOR_HEADER;
                packetBuffer[1] = data;
                packetIdx = 2;
            } else {
                state = WAITING_FOR_START_1;
            }
        }
        break;

        case WAITING_FOR_HEADER:
        {
            packetBuffer[packetIdx++] = data;

            if (packetIdx == sizeof(PacketHeader_t) - 1)
            {
                // There must be 1 byte of space available for CRC
                if (packetIdx == packet->header.packetLen - 1)
                {
                    state = WAITING_FOR_CRC;
                } else {
                    state = WAITING_FOR_DATA;
                }
            }
        }
        break;

        case WAITING_FOR_DATA:
        {
            packetBuffer[packetIdx++] = data;

            // There must be 1 byte of space available for CRC
            if (packetIdx == packet->header.packetLen - 1)
            {
                state = WAITING_FOR_CRC;
            } else if (packetIdx >= MAX_PACKET_SIZE - 1) {
                state = WAITING_FOR_START_1;
            }
        }
        break;

        case WAITING_FOR_CRC:
        {
            packetBuffer[packetIdx] = data;

            if(!checkPacket(packet))
            {
                state = WAITING_FOR_START_1;
                break;
            }

            runPacket(packet);
            sendResponse(packet);
            state = WAITING_FOR_START_1;
        }
        break;

        default:
        {
            state = WAITING_FOR_START_1;
        }
        break;
    }

}
