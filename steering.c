#include <stdio.h>
#include "adc.h"
#include "timerCallback.h"
#include "motorControl.h"

#define GPIO_SPEED_PWM  PORTA, 6
#define GPIO_STEERING_DIRECTION  GPIOC, 6
#define GPIO_HEPHAESTUS_GPIO  PORTB, 15
#define SPEED_PWM_AF    GPIO_AF9_TIM14
#define STEERING_CONTROL_PERIOD 100

#define STEERING_DEADZONE 10

static uint32_t steeringPotTarget = 2000;

static Timer_Return steeringControlCallback(void* dummy);

void initSteering()
{
    addCallbackTimer(STEERING_CONTROL_PERIOD, steeringControlCallback, NULL);


    /*SPEED_PWM_TIM->CCMR1 = (TIM13_CCMR1_OC1M_6) | (TIM13_CCMR1_CC1S_0);
    SPEED_PWM_TIM->CCER = (TIM13_CCER_CC1E);

    SPEED_PWM_TIM->CR1 = (TIM13_CR1_CKD_2) | (TIM13_CR1_CEN);*/

    //FREQ in ARR
    //Duty in CCRx
}

Timer_Return steeringControlCallback(void* dummy)
{
    uint16_t newSteeringValue = getSteeringValue();

    if (newSteeringValue < (steeringPotTarget - STEERING_DEADZONE))
    {
        setSteeringMotorDir(STEERING_RIGHT);
    } else if (newSteeringValue > (steeringPotTarget + STEERING_DEADZONE))
    {
        setSteeringMotorDir(STEERING_LEFT);
    } else {
        setSteeringMotorDir(STEERING_STOP);
    }

    return CONTINUE_TIMER;
}

static uint32_t mapTargetToPot(uint16_t target)
{
    return target;
}

void setSteeringTarget(uint16_t newTarget)
{
    steeringPotTarget = mapTargetToPot(newTarget);
}

void setRawSteeringTarget(uint32_t newTarget)
{
    steeringPotTarget = newTarget;
}
