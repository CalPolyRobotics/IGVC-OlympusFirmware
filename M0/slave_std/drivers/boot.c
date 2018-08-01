#include "commsLib.h"
#include "spi.h"

static wrError_t we;

uint8_t *bootload(){
    we = WR_OK;

    HAL_SPI_Transmit(&hspi1, &we, 1, SPI_TIMEOUT);

    /** TODO - Run a software reset to put in bootloader mode **/

    return &we;
}
