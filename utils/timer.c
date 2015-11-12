#include "main.h"

#define CALLBACK_TIMER TIM2
#define CALLBACK_FREQUENCY 1000

void initIGVCCallbackTimer()
{
    RCC_ClocksTypeDef clocks;

    RCC_GetClocksFreq(&clocks);


    TIM2->CR1 |= TIM_CR1_DIR; //| TIM_CR1_ARPE;


    TIM2->CNT = clocks.PCLK1_frequency / CALLBACK_FREQUENCY;


    TIM2->CR1 |= TIM_CR1_CEN;

}