#include <stdio.h>
#include "adc.h"
#include "timerCallback.h"
#include "motorControl.h"
#include "comms.h"

#define GPIO_SPEED_PWM  PORTA, 6
#define GPIO_STEERING_DIRECTION  GPIOC, 6
#define GPIO_HEPHAESTUS_GPIO  PORTB, 15
#define SPEED_PWM_AF    GPIO_AF9_TIM14
#define STEERING_CONTROL_PERIOD 100

#define STEERING_DEADZONE 100

static uint32_t steeringPotTarget = 3000;
static uint32_t enableSteeringVar = 0;

static Timer_Return steeringControlCallback(void* dummy);

void initSteering()
{
    addCallbackTimer(STEERING_CONTROL_PERIOD, steeringControlCallback, NULL);
}

Timer_Return steeringControlCallback(void* dummy)
{
    uint16_t newSteeringValue = getSteeringValue();
    if (enableSteeringVar)
    {
        if (newSteeringValue < (steeringPotTarget - STEERING_DEADZONE))
        {
            setSteeringMotorDir(STEERING_RIGHT);
        } else if (newSteeringValue > (steeringPotTarget + STEERING_DEADZONE))
        {
            setSteeringMotorDir(STEERING_LEFT);
        } else {
            setSteeringMotorDir(STEERING_STOP);
            enableSteeringVar = 0;
        }
    } else {
        setSteeringMotorDir(STEERING_STOP);
    }

    return CONTINUE_TIMER;
}

static uint32_t mapTargetToPot(uint16_t target)
{
    uint32_t left = 2510;
    uint32_t right = 3520;

    return ((right - left) * (uint32_t)target) / 65535 + left;
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

void enableSteering(uint8_t enable)
{
    enableSteeringVar = enable;
}
