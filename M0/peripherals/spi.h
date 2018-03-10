#ifndef SPI_H_
#define SPI_H_

#include "stm32f0xx_hal.h"

extern SPI_HandleTypeDef hspi1;

void MX_SPI1_Init();

void init_spi_master();
void init_spi_slave();

#endif
