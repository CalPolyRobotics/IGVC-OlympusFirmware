#ifndef __STEERING_H__
#define __STEERING_H__

#include "comms.h"

void initSteering();

void setSteeringTarget(uint16_t newTarget);
void setRawSteeringTarget(uint32_t newTarget);

void setSteeringTargetFromComms(Packet_t* packet);
uint16_t getRawSteeringTarget();
//uint16_t getRawSteeringPotValue();

uint16_t getSteeringDir();
uint16_t getRawSteeringPotValue();
void enableSteering();
uint32_t getCurrentSteeringValue();

void commsSteeringCallback();

#endif
