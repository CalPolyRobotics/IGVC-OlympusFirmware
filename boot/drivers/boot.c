#include <stdint.h>

#include "stm32f205xx.h"
#include "flash.h"
#define PROG_COMPLETE 0x4444

typedef enum bootState{
    HEADER, DATA3, DATA2, DATA1, DATA0, CHECKSUM
}bootState_t;

typedef union uint32{
    uint32_t u32;
    uint16_t u16[2];
    uint8_t  u8[4];
}uint32_u;

typedef struct header{
    uint32_u device;
    uint32_u progSize;
} header_t;


void runBootFSM(uint8_t data){
    static bootState_t state = HEADER;
    static uint32_t* dataAddr = PROG_START;
    static uint32_t count = 0;

    static header_t header;
    //static uint8_t checksum = {0};
    static uint32_u word;

    switch(state)
    {
        case HEADER:
            if(count < sizeof(header.device))
            {
                header.device.u8[count] = data;
            }
            else if(count > sizeof(header.device))
            {
                header.progSize.u8[count] = data;
            }

            if(++count == sizeof(header)){
                state = DATA3;
                count = 0;
            }
            break;

        case DATA3:
            word.u8[3] = data;
            state = DATA2;
            break;

        case DATA2:
            word.u8[2] = data;
            state = DATA1;
            break;

        case DATA1:
            state = DATA0;
            word.u8[1] = data;
            break;

        case DATA0:
            word.u8[0] = data;
            writeFlash(dataAddr, word.u32);
            dataAddr++;

            if(++count == header.progSize.u32){
                state = CHECKSUM;
                count = 0;
            }else{
                state = DATA3;
            }
            break;

        case CHECKSUM:
            // Read Checksum
            completeWrite();
            jumpToApp();
            break;
    }
}
