#ifndef __FNR_H__
#define __FNR_H__

#include "gpio.h"

#include "comms.h"

#define FNR_DIRECTION_PIN   GPIO_PIN_13
#define FNR_DIRECTION_PORT  GPIOC

#define FNR_ENABLE_PIN      GPIO_PIN_15
#define FNR_ENABLE_PORT     GPIOC

typedef enum {FNR_NEUTRAL = 0, FNR_FORWARD, FNR_REVERSE} FNR_t;

void setFNR(FNR_t newState);

void FNRCommsHandler(Packet_t* packet);

#endif