#ifndef __SPEED_DAC_H__
#define __SPEED_DAC_H__

#include "comms.h"

#define SPEED_DAC_ENABLE_PIN    GPIO_PIN_12
#define SPEED_DAC_ENABLE_PORT   GPIOB

void initSpeedDAC();
void writeSpeedDAC(uint8_t value);

void setSpeedDACOutputEnable(uint8_t enable);
void speedDACHandler(Packet_t* packet);
void toggleSpeedDAC(Packet_t* packet);
void resetSpeedDAC();

#endif