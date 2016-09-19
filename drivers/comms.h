#ifndef __COMMS_H__
#define __COMMS_H__

#include <stdint.h>
#include <stddef.h>

typedef struct {
    uint8_t startByte[2];
    uint8_t CRC8;
    uint8_t msgType;
    uint8_t seqNumber;
    uint8_t packetLen;
} __attribute__((packed)) PacketHeader_t;

typedef struct {
    PacketHeader_t header;
    uint8_t data[250];
} Packet_t;

typedef void (*commsCallback)(Packet_t*);

void runCommsFSM(char data);


#endif