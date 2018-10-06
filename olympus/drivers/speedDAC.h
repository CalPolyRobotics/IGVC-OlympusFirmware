#ifndef __SPEED_DAC_H__
#define __SPEED_DAC_H__

#include "comms.h"

void initSpeedDAC();
void writeSpeedDAC(uint16_t value);

void commsSetThrottleCallback(Packet_t* packet);
void commsSetThrottleEnableCallback(Packet_t* packet);

void enableSpeedDAC();
void disableSpeedDAC();

void resetSpeedDAC();
uint16_t getSpeedDAC();

void commsPedalAdcCallback();

#endif
