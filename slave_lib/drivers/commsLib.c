#include <stdlib.h>
#include "comms.h"
#include "spi.h"

typedef enum commState{
    START_BYTE=0, MSG_TYPE, DATA
}commState_t;

static uint8_t buf[256];

wrError_t runCommsFSM(uint8_t data){
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
            if(data >= NUM_MSGS){
                state = START_BYTE;
                break;
            }

            msgType = data;
            dataSize = msgResp[msgType].rxDataLength;

            if(dataSize == 0){
                writeResponse(msgResp[msgType].callback(NULL), msgResp[msgType].txDataLength, STD_SPI_DELAY);

                state = START_BYTE;
            }else{
                dataIdx = 0;
                state = DATA;
            }
            break;

        case DATA:
            buf[dataIdx++] = data;
            if(dataIdx == dataSize){
                writeResponse(msgResp[msgType].callback(buf), msgResp[msgType].txDataLength, STD_SPI_DELAY);
                state = START_BYTE;
            }

            break;
    }

    return 0;
}
