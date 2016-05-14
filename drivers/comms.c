#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>

#include "main.h"

#include "comms.h"
#include "led.h"
#include "usart.h"
#include "usb_otg.h"
#include "fnr.h"
#include "pwradc.h"
#include "speedDAC.h"
#include "kill.h"
#include "steering.h"

#define HEADER_START_SIZE 2
#define COMMS_START_BYTE 0xF0
#define MAX_PACKET_SIZE 64

#define START_BYTE_1 0xF0
#define START_BYTE_2 0x5A

// Used for CRC
#define GP  0x107   /* x^8 + x^2 + x + 1 */
#define DI  0x07

typedef enum {
    WAITING_FOR_START_1 = 0,
    WAITING_FOR_START_2,
    WAITING_FOR_HEADER,
    WAITING_FOR_DATA
} CommsState_t;


typedef struct {
    uint32_t inputDataMaxLen;
    uint8_t* inputData;
    uint32_t responseDataLen;
    uint8_t* responseData;
    commsCallback callback;
} packetResponse_t;

// Generates a CRC8 lookup table
static void init_crc8()
     /*
      * Should be called before any other crc function.  
      */
{
  int i,j;
  unsigned char crc;

  if (!made_table) {
    for (i=0; i<256; i++) {
      crc = i;
      for (j=0; j<8; j++)
        crc = (crc << 1) ^ ((crc & 0x80) ? DI : 0);
      crc8_table[i] = crc & 0xFF;
      /* printf("table[%d] = %d (0x%X)\n", i, crc, crc); */
    }
    made_table=1;
  }
}

// Calculates an intermediate CRC8
void crc8byte(unsigned char *crc, unsigned char m)
{
  if (!made_table)
    init_crc8();

  *crc = crc8_table[(*crc) ^ m];
  *crc &= 0xFF;
}

// Calculates a complete CRC8
void crc8(Packet_t* packet){
    uint8_t* byte_pointer = (uint8_t*)packet;
    *crc = 0;

    for (int i = 0; i < packet->header.packetLen; i++){
        crc8byte(byte_pointer[i],*crc);
    }
}

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

packetResponse_t response[] = {
    {0,  NULL, 0,  NULL, toggleLED},                  // Status
    {0,  NULL, 0,  NULL, toggleLED2},                 // Get 1 Sonar
    {0,  NULL, 0,  NULL, toggleLED},                  // Get all Sonars
    {0,  NULL, 0,  NULL, FNRCommsHandler},            // Set FNR
    {0,  NULL, 0,  NULL, toggleLED},                  // Get FNR
    {0,  NULL, 0,  NULL, speedDACHandler},            // Set Throttle  
    {0,  NULL, 0,  NULL, toggleSpeedDAC},             // Set Speed
    {0,  NULL, 0,  NULL, toggleLED},                  // Get Speed
    {0,  NULL, 0,  NULL, setSteeringTargetFromComms}, // Set Steering
    //{0,  NULL, 2,  commsCurrentSteeringValue, NULL},// Get Steering Angle
    {0,  NULL, 2,  &commsCurrentSteeringValue[0], commsSteeringCallback},  // Get Steering Angle
    {0,  NULL, 0,  NULL, commsSetLightsCallback},     // Set Lights
    {0,  NULL, 0,  NULL, toggleLED},                  // Get Battery
    {0,  NULL, 16,  &commsPwradcValues[0], commsPwradcCallback}, //Get Power
    {0,  NULL, 0,  NULL, killBoard}                  //Send Stop
};

static bool checkPacket(Packet_t* packet)
{
    if (crc8(packet) == 0){
        return true;
    }
    else{
        return true; // CHANGE THIS LATER
    }
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

    if (response[packetType].callback)
    {
        response[packetType].callback(packet);
    }
}


static void sendResponse(Packet_t* packet)
{
    uint8_t packetBuffer[MAX_PACKET_SIZE];
    Packet_t* outPacket = (Packet_t*)packetBuffer;
    uint8_t packetType = packet->header.msgType >> 1;
    uint32_t idx;

    outPacket->header.startByte[0] = START_BYTE_1;
    outPacket->header.startByte[1] = START_BYTE_2;
    outPacket->header.CRC8 = 0;
    outPacket->header.msgType = packet->header.msgType | 1;
    outPacket->header.seqNumber = packet->header.seqNumber;
    outPacket->header.packetLen = response[packetType].responseDataLen + sizeof(PacketHeader_t);

    for (idx = 0; idx < response[packetType].responseDataLen; idx++)
    {
        outPacket->data[idx] = response[packetType].responseData[idx];
    }

    //outPacket->header.CRC8 = crc8(outPacket, outPacket->header.packetLen);

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
            //printf("Waiting START1\r\n");
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
            //printf("Waiting START2\r\n");
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
            packetBuffer[packetIdx] = data;
            packetIdx++;
            //printf("Waiting HEADER %lu == %u\r\n", packetIdx, sizeof(PacketHeader_t));
            if (packetIdx == sizeof(PacketHeader_t))
            {
                if (packetIdx == packet->header.packetLen)
                {
                    checkPacket(packet);
                    runPacket(packet);
                    sendResponse(packet);
                    state = WAITING_FOR_START_1;
                } else {
                    state = WAITING_FOR_DATA;
                }
            }
        }
        break;

        case WAITING_FOR_DATA:
        {
            packetBuffer[packetIdx] = data;
            packetIdx++;
            //printf("Waiting DATA %lu == %u\r\n", packetIdx, packet->header.packetLen);
            if (packetIdx == packet->header.packetLen)
            {
                checkPacket(packet);
                runPacket(packet);
                sendResponse(packet);
                state = WAITING_FOR_START_1;
            } else if (packetIdx >= MAX_PACKET_SIZE) {
                state = WAITING_FOR_START_1;
            }
        }
        break;

        default:
        {
            state = WAITING_FOR_START_1;
        }
        break;
    }

}
