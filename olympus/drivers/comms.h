#ifndef __COMMS_H__
#define __COMMS_H__

#include <stdint.h>
#include <stddef.h>


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

void runCommsFSM(char data);
#endif
