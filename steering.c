#include "config.h"
#include "adc.h"
#include "timerCallback.h"

static uint16_t steeringTarget = 0x8000;

static void steeringControlCallback(void* dummy);

void initSteering()
{
    addCallbackTimer(STEERING_CONTROL_PERIOD, steeringControlCallback, NULL);


    /*SPEED_PWM_TIM->CCMR1 = (TIM13_CCMR1_OC1M_6) | (TIM13_CCMR1_CC1S_0);
    SPEED_PWM_TIM->CCER = (TIM13_CCER_CC1E);

    SPEED_PWM_TIM->CR1 = (TIM13_CR1_CKD_2) | (TIM13_CR1_CEN);*/

    //FREQ in ARR
    //Duty in CCRx
}

void steeringControlCallback(void* dummy)
{
    //printf("ADC: %d\r\n", getSteeringValue());
}