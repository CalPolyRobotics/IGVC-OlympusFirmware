#ifndef __SPEED_DAC_H__
#define __SPEED_DAC_H__

#include "comms.h"

void initSpeedDAC();
void writeSpeedDAC(uint8_t value);

void setSpeedDACOutputEnable(uint8_t enable);
void speedDACHandler(Packet_t* packet);
void toggleSpeedDAC(Packet_t* packet);
void enableSpeedDAC();
void disableSpeedDAC();


void resetSpeedDAC();
uint16_t getSpeedDAC();

#endif
