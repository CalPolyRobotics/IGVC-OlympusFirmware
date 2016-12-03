#include <stdint.h>
#include <stdio.h>

typedef struct {
    uint8_t startByte;
    uint8_t msgType;
    uint8_t seqNumber;
    uint8_t packetLen;
}__attribute__((packed)) PacketHeader_t;

typedef struct {
    PacketHeader_t header;
    uint8_t data[27];
    uint8_t crc;
} Packet_t;

typedef enum{
    START_1 = 0, HEADER, DATA, CRC
}commsState_t
int main(){
    printf("Header: %lu\n", sizeof(PacketHeader_t));
    printf("Packet: %lu\n", sizeof(Packet_t));
    return 0;
}

runCommsFSM(uint8_t * data){
    static commsState_t ps = START_1;
    static uint8_t packetBuf[MAX_PACKET_LEN];
    static Packet_t* packet = (Packet_t*)packetBuffer;
    static uint8_t packetIdx;

    switch(ps){
        case START_1:
        break;
        case HEADER:
        break;
        case DATA:
        break;
        case CRC:
        break;
        case defuault:
        break;
    }

}

/**
Packet_t* readPacket(){

}

bool checkPacket(){

}
*/

