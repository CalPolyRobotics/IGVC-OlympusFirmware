#include <stdint.h>
#include <stdio.h>

#define DATA_MSG     0x00
#define SIZE_MSG     0x01
#define SECTOR_MSG   0x02
#define CHECKSUM_MSG 0x03
#define READY_MSG    0x04
#define START_MSG    0x05
#define END_MSG      0x06
#define ERROR_MSG    0x07

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

void runCommsFSM(uint8_t data){
    static commsState_t state = START_1;
    static uint8_t packetBuf[MAX_PACKET_LEN];
    static Packet_t* packet = (Packet_t*)packetBuffer;
    static uint8_t packetIdx;

    switch(state){
        case START_1:
            packetIdx = 0;
            if (data == START_BYTE){
                state = HEADER;
                packetBuffer[0] = data;
                packetIdx = 1;
            }
        break;

        case HEADER:
            packetBuffer[packetIdx++] = data;

            if(packetIdx == sizeof(PacketHeader_t)){
                if(packetIdx == packet->header.packetLen){
                    state = CRC;
                }else{
                    state = DATA;
                }
            }
        break;

        case DATA:
            packetBuffer[packetIdx++] = data;

            if(packetIdx == packet->header.packetLen){
                state = CRC;
            }

        break;

        case CRC:
            packetBuffer[sizeof(Packet_t) - 1] = data;

            if(checkPacket(packet)){
                runPacket(packet);
                sendResponse(packet);
            }

            state = START_1
        break;

        default:
            state = START_1;
        break;
    }
}

void runPacket(packet * Packet_t){
    switch(packet -> header.msg_type){
        case DATA_MSG:
            writeData(packet);
        break;

        case CHECKSUM_MSG:
            runChecksum(packet);
        break;

        case READY_MSG:
            // Do something
        break;

        case START_MSG:
            //checkSectorSpace(packet -> data[1]);
            writeInit(packet -> data[0]);
        break;

        case END_MSG:
            writeComplete();
            jumpToApp();
        break;

        case ERROR_MSG:
            // Do something
        break;

        default:
            // Do nothing
        break;

    }

}

bool checkPacket(){
    return true;
}
