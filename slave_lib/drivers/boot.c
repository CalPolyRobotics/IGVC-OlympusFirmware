#include "commsLib.h"
#include "spi.h"

static wrError_t we;

uint8_t *bootload(){
    we = WR_OK;
    
    writeResponse(&we, 1);

    NVIC_SystemReset();

    /** The code will not execute this return because of the Reset **/
    return &we;
}
