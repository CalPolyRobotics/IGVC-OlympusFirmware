#include <stdio.h>
#include "config.h"
#include "adc.h"
#include "timerCallback.h"
#include "motorControl.h"
#include "comms.h"

#define STEERING_COUNT_THRESHOLD 5

static uint32_t steeringPotTarget = 1000;
static uint32_t enableSteeringVar = 0;
static uint8_t steeringValueCount = 0;

uint16_t currentSteeringValue = 0;
uint8_t commsCurrentSteeringValue[] = {0,0};

static Timer_Return steeringControlCallback(void* dummy);

void initSteering()
{
    addCallbackTimer(STEERING_CONTROL_PERIOD, steeringControlCallback, NULL);
}

static uint32_t mapTargetToPot(uint16_t target)
{
    uint32_t left = 2510;
    uint32_t right = 3520;

    return ((right - left) * (uint32_t)target) / 65535 + left;
}

static uint32_t mapPotToTarget(uint16_t pot)
{
    uint32_t left = 2510;
    uint32_t right = 3520;

    if (pot < left)
    {
        pot = left;
    } else if (pot > right)
    {
        pot = right;
    }

    return ((pot - left) * 65535) / (right - left);
}

uint32_t getCurrentSteeringValue() 
{
    return mapPotToTarget(getSteeringValue());
}


Timer_Return steeringControlCallback(void* dummy)
{
    uint16_t newSteeringValue = getSteeringValue();

    currentSteeringValue = (uint16_t)mapPotToTarget(newSteeringValue);

    if (enableSteeringVar)
    {
        if (newSteeringValue < (steeringPotTarget - STEERING_CONTROL_DEADZONE))
        {
            steeringValueCount = 0;
            setSteeringMotorDir(STEERING_RIGHT);
        } else if (newSteeringValue > (steeringPotTarget + STEERING_CONTROL_DEADZONE))
        {
            steeringValueCount = 0;
            setSteeringMotorDir(STEERING_LEFT);
        } else {
            if (++steeringValueCount >= STEERING_COUNT_THRESHOLD) {
                setSteeringMotorDir(STEERING_STOP);
                enableSteeringVar = 0;
                steeringValueCount = 0;
            }
        }
    } else {
        steeringValueCount = 0;
        setSteeringMotorDir(STEERING_STOP);
    }

    return CONTINUE_TIMER;
}

void setSteeringTarget(uint16_t newTarget)
{
    steeringPotTarget = mapTargetToPot(newTarget);
}

void setSteeringTargetFromComms(Packet_t* packet)
{
    uint16_t val = (packet->data[0] << 8) | (packet->data[1]);
    enableSteeringVar = 1;
    setSteeringTarget(val);
}

void setRawSteeringTarget(uint32_t newTarget)
{
    steeringPotTarget = newTarget;
}

uint16_t getRawSteeringTarget()
{
    return steeringPotTarget;
}

uint16_t getRawSteeringPotValue()
{
    return getSteeringValue();
}

uint16_t getSteeringDir()
{
    return mapPotToTarget(getSteeringValue());
}

void enableSteering(uint8_t enable)
{
    enableSteeringVar = enable;
}

void commsSteeringCallback()
{
    commsCurrentSteeringValue[0] = (currentSteeringValue >> 8) & 0xFF;
    commsCurrentSteeringValue[1] = currentSteeringValue & 0xFF;
    printf("%u", commsCurrentSteeringValue[0]);
    printf("%u", commsCurrentSteeringValue[1]);
}
