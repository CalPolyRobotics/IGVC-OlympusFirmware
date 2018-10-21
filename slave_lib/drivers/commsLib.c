#include <stdlib.h>
#include <stdbool.h>
#include "stm32f0xx.h"
#include "comms.h"
#include "spi.h"

typedef enum commState{
    START_BYTE=0, MSG_TYPE, DATA
}commState_t;

static uint8_t buff[256];

void pauseInterruptingPeripherals() __attribute__((weak));
void restoreInterruptingPeripherals() __attribute__((weak));

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

            if(msgResp[msgType].callback == NULL){
                state = START_BYTE;
                break;
            }

            if(dataSize == 0){
                if(msgType == BOOT_MSG)
                {
                    // Bootload message must be handled seperately since it never returns
                    writeResponse(WR_OK, NULL, 0u);
                }

                pauseInterruptingPeripherals();
                writeResponse(msgResp[msgType].callback(buff), buff, msgResp[msgType].txDataLength);
                restoreInterruptingPeripherals();

                state = START_BYTE;
            }else{
                dataIdx = 0u;
                state = DATA;
            }

            break;

        case DATA:
            buff[dataIdx++] = data;
            if(dataIdx == dataSize)
            {
                pauseInterruptingPeripherals();
                writeResponse(msgResp[msgType].callback(buff), buff, msgResp[msgType].txDataLength);
                restoreInterruptingPeripherals();

                state = START_BYTE;
            }

            break;
    }

    return 0;
}
