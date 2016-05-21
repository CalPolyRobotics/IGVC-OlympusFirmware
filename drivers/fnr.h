#ifndef __FNR_H__
#define __FNR_H__

#include "gpio.h"

#include "comms.h"

typedef enum {FNR_NEUTRAL = 0, FNR_FORWARD, FNR_REVERSE} FNR_t;

uint8_t FNRState;

void initAutomanInt();
void setFNR(FNR_t newState);

void FNRCommsHandler(Packet_t* packet);
void commsGetFNRCallback();

uint8_t getFNR();

#endif
