#ifndef SPI_H_
#define SPI_H_

#include "stm32f0xx_hal.h"

#define SPI_TIMEOUT 100

extern SPI_HandleTypeDef hspi1;

void MX_SPI1_Init();

void writeResponse(uint8_t *data, uint16_t length);
#endif
