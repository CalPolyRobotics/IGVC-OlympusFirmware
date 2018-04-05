#include <stdlib.h>
#include "comms.h"
#include "spi.h"

void write_response(uint8_t *data, uint8_t length);

typedef enum COMMS_STATE{
    START_BYTE=0, MSG_TYPE, DATA
}COMMS_STATE;

uint8_t buf[256];

void runCommsFSM(uint8_t data){
    static COMMS_STATE state = START_BYTE;
    static uint8_t dataSize;
    static uint8_t dataIdx;
    static uint8_t msgType;

    switch(state){
        case START_BYTE:
            if(data == COMMS_START_BYTE){
                state = MSG_TYPE;
            }
            break;

        case MSG_TYPE:
            if(data >= MSG_INFO_SIZE){
                state = START_BYTE;
                break;
            }

            msgType = data;
            dataSize = msgResp[msgType].txDataLength;

            if(dataSize == 0){
                write_response(msgResp[msgType].callback(NULL), msgResp[msgType].rxDataLength);
                state = START_BYTE;
            }else{
                dataIdx = 0;
                state = DATA;
            }
            break;

        case DATA:
            buf[dataIdx] = data;
            if(dataIdx == dataSize - 1){
                write_response(msgResp[msgType].callback(buf), msgResp[msgType].rxDataLength);
                state = START_BYTE;
            }else{
                dataIdx++;
            }
            break;
    }
}


void write_response(uint8_t *data, uint8_t length){
    HAL_SPI_Transmit(&hspi1, data, length, SPI_TIMEOUT);
}
