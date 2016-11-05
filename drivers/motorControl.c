
#include "main.h"
#include "config.h"
#include "motorControl.h"

#define PWM_TIM TIM13
#define STEERING_MOTOR_FREQ     30000
#define STEERING_MOTOR_COUNT    1000

void initSteeringMotor()
{
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_TIM13;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    uint32_t clkFreq = HAL_RCC_GetPCLK1Freq();

    PWM_TIM->CR1 =  TIM_CR1_ARPE;

    PWM_TIM->CCMR1 =    TIM_CCMR1_OC1M_2 |
                        TIM_CCMR1_OC1M_1 |
                        TIM_CCMR1_OC1PE;

    PWM_TIM->CCER = TIM_CCER_CC1E;

    PWM_TIM->ARR = STEERING_MOTOR_COUNT;
    PWM_TIM->PSC = (clkFreq) / (STEERING_MOTOR_FREQ * STEERING_MOTOR_COUNT);
    PWM_TIM->CCR1 = 750;

    PWM_TIM->CR1 |= TIM_CR1_CEN;
    PWM_TIM->EGR |= TIM_EGR_UG;
}

void setSteeringMotorDir(MotorDirection dir)
{
    if (dir == STEERING_LEFT)
    {
        HAL_GPIO_WritePin(GPIO_STEER_LEFT, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIO_STEER_RIGHT, GPIO_PIN_RESET);
    } else if (dir == STEERING_RIGHT) {
        HAL_GPIO_WritePin(GPIO_STEER_LEFT, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIO_STEER_RIGHT, GPIO_PIN_SET);
    } else {
        HAL_GPIO_WritePin(GPIO_STEER_LEFT, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIO_STEER_RIGHT, GPIO_PIN_RESET);
    }
}
