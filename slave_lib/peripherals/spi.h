#ifndef SPI_H_
#define SPI_H_

#include "stm32f0xx_hal.h"
#include "commsLib.h"

extern SPI_HandleTypeDef hspi1;

void MX_SPI1_Init();

wrError_t writeResponse(uint8_t *data, uint16_t length);
#endif
