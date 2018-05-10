#include <stdlib.h>
#include "comms.h"
#include "spi.h"

typedef enum commState{
    START_BYTE=0, MSG_TYPE, DATA
}commState_t;

static uint8_t errorByte;
static uint8_t buf[256];

static void writeResponse(uint8_t *data, uint8_t length);

/** TODO make sure error returns don't get confused as data by Olympus **/
void runCommsFSM(uint8_t data){
    static commState_t state = START_BYTE;
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
            if(data > NUM_MSGS){
                errorByte = WR_ERR_INV_MSG_TYPE;
                writeResponse(&errorByte, 1);
                state = START_BYTE;
                break;
            }

            msgType = data;
            dataSize = msgResp[msgType].txDataLength;

            if(dataSize == 0){
                writeResponse(msgResp[msgType].callback(NULL), msgResp[msgType].rxDataLength);
                state = START_BYTE;
            }else{
                dataIdx = 0;
                state = DATA;
            }
            break;

        case DATA:
            buf[dataIdx] = data;
            if(dataIdx == dataSize - 1){
                writeResponse(msgResp[msgType].callback(buf), msgResp[msgType].rxDataLength);
                state = START_BYTE;
            }else{
                dataIdx++;
            }
            break;
    }
}


void writeResponse(uint8_t *data, uint8_t length){
    HAL_SPI_Transmit(&hspi1, data, length, SPI_TIMEOUT);
}
