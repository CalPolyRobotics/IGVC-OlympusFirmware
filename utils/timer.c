#include "main.h"

#define CALLBACK_TIMER TIM9
#define CALLBACK_FREQUENCY 20000
#define TIMER_FREQUENCY 1000000
#define TIMER_MAX_VALUE 0xFFFF

void initIGVCCallbackTimer()
{
    RCC_ClocksTypeDef clocks;

    RCC_GetClocksFreq(&clocks);

    CALLBACK_TIMER->DIER = TIM_DIER_UIE;

    CALLBACK_TIMER->PSC = clocks.PCLK2_Frequency / TIMER_FREQUENCY;
    CALLBACK_TIMER->ARR = (TIMER_FREQUENCY / CALLBACK_FREQUENCY); 

    NVIC_InitTypeDef timerInt;
    timerInt.NVIC_IRQChannel = TIM1_BRK_TIM9_IRQn;
    timerInt.NVIC_IRQChannelPreemptionPriority = 0xF;
    timerInt.NVIC_IRQChannelSubPriority = 0xF;

    NVIC_SetPriority(TIM1_BRK_TIM9_IRQn, 2);
    NVIC_EnableIRQ(TIM1_BRK_TIM9_IRQn);

    CALLBACK_TIMER->CR1 = TIM_CR1_CEN | TIM_CR1_ARPE;

}


void TIM1_BRK_TIM9_IRQHandler()
{
    GPIO_ToggleBits(GPIOA, GPIO_Pin_4);    
    TIM_ClearITPendingBit(CALLBACK_TIMER, TIM_EventSource_Update);
    //printf("int");
}