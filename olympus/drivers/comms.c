#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>

#include "comms.h"
#include "error.h"
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
#include "hermes.h"
#include "hephaestus.h"

#include "tcp_server.h"

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

static uint8_t inputBuf[256];

static packetResponse_t response[] = {
    {0u,    NULL,     5u,   status,                  NULL},                // (0x00)

    // Hermes 
    {0u,    NULL,     2u,   hermesData.speed,        NULL},                // (0x02)
    {0u,    NULL,     2u,   NULL,                    NULL},                // (0x04)
    {0u,    NULL,     0u,   NULL,                    NULL},                // (0x06)

    // Apollo
    {1u,    inputBuf, 0u,   NULL,                    commsSetTurnSignal},  // (0x08)
    {2u,    inputBuf, 0u,   NULL,                    commsSetHeadlights},  // (0x0A)
    {2u,    inputBuf, 0u,   NULL,                    commsSetMiscLights},  // (0x0C)

    // Janus
    {0u,    NULL,     1u,   NULL,                    NULL},                // (0x0E)
    {0u,    NULL,     1u,   NULL,                    NULL},                // (0x10)
    {1u,    inputBuf, 0u,   NULL,                    commsSetFNR},         // (0x12)

    // Hephaestus
    {1u,    inputBuf, 0u,   NULL,                    commsSetSteering},    // (0x14)
    {2u,    inputBuf, 0u,   NULL,                    commsSetBrake},       // (0x16)
    {0u,    NULL,     0u,   NULL,                    NULL},                // (0x18)

    // Olympus
    {2u,    inputBuf, 0u,   NULL,                    commsSetSpeed},       // (0x1A)
    {2u,    inputBuf, 0u,   NULL,                    commsSetLeds},        // (0x1C)
    {0u,    NULL,     16u,  olympusData.power.u8,    commsPwradcCallback}, // (0x1E)
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

static void sendResponse(CommsHandler_t *hdl, Packet_t* packet)
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

    if (usingUSB)
    {
        usbWrite(packetBuffer, outPacket->header.packetLen);
    }
    else
    {
        struct tcp_pcb *tpcb = (struct tcp_pcb *)(hdl->arg);
        tcp_server_send(tpcb, (char *)outPacket, outPacket->header.packetLen);
    }
}

void runCommsFSM(CommsHandler_t *hdl, char *buff, size_t len)
{
    Packet_t *packet = (Packet_t *)(hdl->packetBuffer);

    for(int i = 0; i < len; i++){
        char data = buff[i];

        switch(hdl->state)
        {
            case WAITING_FOR_START_1:
            {
                hdl->packetIdx = 0;
                if (data == START_BYTE_1)
                {
                    hdl->state = WAITING_FOR_START_2;
                    hdl->packetBuffer[0] = data;
                    hdl->packetIdx = 1;
                }
            }
            break;

            case WAITING_FOR_START_2:
            {
                if (data == START_BYTE_2)
                {
                    hdl->state = WAITING_FOR_HEADER;
                    hdl->packetBuffer[1] = data;
                    hdl->packetIdx = 2;
                } else {
                    hdl->state = WAITING_FOR_START_1;
                }
            }
            break;

            case WAITING_FOR_HEADER:
            {
                hdl->packetBuffer[hdl->packetIdx++] = data;

                if (hdl->packetIdx == sizeof(PacketHeader_t) - 1)
                {
                    // There must be 1 byte of space available for CRC
                    if (hdl->packetIdx == packet->header.packetLen - 1)
                    {
                        hdl->state = WAITING_FOR_CRC;
                    } else {
                        hdl->state = WAITING_FOR_DATA;
                    }
                }
            }
            break;

            case WAITING_FOR_DATA:
            {
                hdl->packetBuffer[hdl->packetIdx++] = data;

                // There must be 1 byte of space available for CRC
                if (hdl->packetIdx == packet->header.packetLen - 1)
                {
                    hdl->state = WAITING_FOR_CRC;
                } else if (hdl->packetIdx >= MAX_PACKET_SIZE - 1) {
                    hdl->state = WAITING_FOR_START_1;
                }
            }
            break;

            case WAITING_FOR_CRC:
            {
                hdl->packetBuffer[hdl->packetIdx] = data;

                if(!checkPacket(packet))
                {
                    hdl->state = WAITING_FOR_START_1;
                    break;
                }

                runPacket(packet);
                sendResponse(hdl, packet);
                hdl->state = WAITING_FOR_START_1;
            }
            break;

            default:
            {
                hdl->state = WAITING_FOR_START_1;
            }
            break;
        }
    }
}

void initCommsHandler(CommsHandler_t *hdl, void *arg){
    hdl->state = WAITING_FOR_START_1;
    hdl->packetIdx = 0;
    hdl->arg = arg;
}
