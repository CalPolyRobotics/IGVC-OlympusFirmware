#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>

#include "main.h"

#include "comms.h"
#include "submoduleComms.h"
#include "bootComms.h"
#include "led.h"
#include "usart.h"
#include "usb_otg.h"
#include "pwradc.h"
#include "speedDAC.h"
#include "kill.h"
#include "crc8.h"
#include "adc.h"

#define HEADER_START_SIZE 2
#define COMMS_START_BYTE 0xF0
#define MAX_PACKET_SIZE 64

#define START_BYTE_1 0xF0
#define START_BYTE_2 0x5A

extern volatile uint8_t  commsPwradcValues[16];

/* XXX Currently used to send return data for usb requests
 * Remove when no longer needed
 */
uint8_t dummy[16] = {0};

typedef enum {
    WAITING_FOR_START_1 = 0,
    WAITING_FOR_START_2,
    WAITING_FOR_HEADER,
    WAITING_FOR_DATA,
    WAITING_FOR_CRC
} CommsState_t;


typedef struct {
    uint32_t inputDataMaxLen;
    uint8_t* inputData;
    uint32_t responseDataLen;
    uint8_t* responseData;
    uint8_t  module;
    uint8_t  moduleMsgType;
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

static uint8_t inputBuf[256];

void echoPacketCallback(Packet_t* packet);

static packetResponse_t response[] = {
    {250,  inputBuf, 0,  inputBuf,           OLYMPUS,    0,                       echoPacketCallback}, // (0x00) Echo
    {1,    NULL,     1,  submoduleCommsBuff, APOLLO,     APOLLO_ECHO,             NULL},               // (0x02) Apollo Echo
    {1,    NULL,     1,  submoduleCommsBuff, APOLLO,     APOLLO_SET_TURN_SIGNAL,  NULL},               // (0x04) Apollo Set Turn Signal
    {1,    NULL,     1,  submoduleCommsBuff, APOLLO,     APOLLO_SET_HEADLIGHTS,   NULL},               // (0x06) Apollo Set Headlights
    {1,    NULL,     1,  submoduleCommsBuff, APOLLO,     APOLLO_GET_LIGHT_SENSOR, NULL},               // (0x08) Apollo Get Light Sensor
    {1,    NULL,     1,  submoduleCommsBuff, HERA,       HERA_ECHO,               NULL},               // (0x00) Hera Echo
    {1,    NULL,     1,  submoduleCommsBuff, HEPHAESTUS, HEPHAESTUS_ECHO,         NULL},               // (0x00) Hephaestus Echo
    {1,    NULL,     1,  submoduleCommsBuff, JANUS,      JANUS_ECHO,              NULL},               // (0x00) Janus Echo
    {1,    NULL,     1,  submoduleCommsBuff, JANUS,      JANUS_SET_FNR,           NULL},               // (0x00) Janus Set FNR
    {0,    NULL,     1,  submoduleCommsBuff, JANUS,      JANUS_GET_FNR,           NULL},               // (0x00) Janus Get FNR
    {3,    NULL,     1,  NULL,               OLYMPUS,    0,                       bootloadBoard},      // (0x00) Enter Bootloader Mode
    {0,    NULL,     0,  NULL,               OLYMPUS,    0,                       killBoard}           // Send Stop
};

void echoPacketCallback(Packet_t* packet)
{
    // CRC is of length 1
    response[0].responseDataLen = packet->header.packetLen - sizeof(PacketHeader_t) - 1;
}

static bool checkPacket(Packet_t* packet)
{
    return crc8(packet, packet->header.packetLen) == 0;
}


static void runPacket(Packet_t* packet)
{
    uint32_t bytesToCopy, idx;
    uint8_t packetType = packet->header.msgType >> 1;
    uint8_t dataLen = packet->header.packetLen - sizeof(PacketHeader_t);

    if (response[packetType].inputDataMaxLen > 0)
    {
        if (response[packetType].inputDataMaxLen > dataLen)
        {
            bytesToCopy = dataLen;
        } else {
            bytesToCopy = response[packetType].inputDataMaxLen;
        }

        for (idx = 0; idx < bytesToCopy; idx++)
        {
            response[packetType].inputData[idx] = packet->data[idx];
        }
    }

    if (response[packetType].module == OLYMPUS)
    {
        response[packetType].callback(packet);
    } else {
        messageSubmodule(response[packetType].module,
                      response[packetType].moduleMsgType,
                      &packet->data[0],
                      response[packetType].inputDataMaxLen,
                      response[packetType].responseDataLen);
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
