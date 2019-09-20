#ifndef __COMMS_H__
#define __COMMS_H__

#include <stdint.h>
#include <stddef.h>

#include "lwip/tcp.h"

extern int usingUSB;

#define HEADER_START_SIZE 2
#define COMMS_START_BYTE 0xF0
#define MAX_PACKET_SIZE 64

#define START_BYTE_1 0xF0
#define START_BYTE_2 0x5A


typedef struct {
    uint8_t startByte[2];
    uint8_t msgType;
    uint8_t seqNumber;
    uint8_t packetLen;
} __attribute__((packed)) PacketHeader_t;

typedef struct {
    PacketHeader_t header;
    uint8_t data[251];
} Packet_t;

typedef enum {
    WAITING_FOR_START_1 = 0,
    WAITING_FOR_START_2,
    WAITING_FOR_HEADER,
    WAITING_FOR_DATA,
    WAITING_FOR_CRC
} CommsState_t;

typedef struct{
    CommsState_t state;
    uint8_t packetBuffer[MAX_PACKET_SIZE];
    Packet_t *packet;
    uint32_t packetIdx;
    void *arg;
}CommsHandler_t;

typedef struct power{
   uint16_t battVolt;
   uint16_t battCur;
   uint16_t twlvVolt;
   uint16_t twlvCur;
   uint16_t fiveVolt;
   uint16_t fiveCur;
   uint16_t thrVolt;
   uint16_t thrCur;
} __attribute__((packed)) power_t;

typedef union powerUnion{
   uint8_t u8[16];
   power_t obj;
} powerUnion_t;

typedef struct olympusData{
   powerUnion_t power;
} __attribute__((packed)) olympusData_t;

typedef void (*commsCallback)(uint8_t*);

void runCommsFSM(CommsHandler_t *hdl, char *buff, size_t len);
void initCommsHandler(CommsHandler_t *hdl, void *arg);

#endif
