#include "main.h"
#include "timer.h"

#define CALLBACK_TIMER TIM9
#define CALLBACK_FREQUENCY 1000
#define TIMER_FREQUENCY 1000000
#define TIMER_MAX_VALUE 0xFFFF

#define MAX_NUM_CALLBACK_TIMERS 10

typedef struct CallbackTimer_t {
    uint32_t currentValue;
    uint32_t resetValue;
    timerCallback callback;
    void* parameters;
} CallbackTimer;

CallbackTimer timerList[MAX_NUM_CALLBACK_TIMERS];

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

    uint32_t ndx;
    for (ndx = 0; ndx < MAX_NUM_CALLBACK_TIMERS; ndx++)
    {
        timerList[ndx].resetValue = 0;
    }

}

int32_t addCallbackTimer(uint32_t count, timerCallback callback, void* parameters)
{
    uint32_t ndx;

    if (count == 0 || callback == NULL)
    {
        return -1;
    }

    for (ndx = 0; ndx < MAX_NUM_CALLBACK_TIMERS; ndx++)
    {
        if (timerList[ndx].resetValue == 0)
        {
            break;
        }
    }

    if (ndx == MAX_NUM_CALLBACK_TIMERS)
    {
        return -1;
    }

    timerList[ndx].resetValue = count;
    timerList[ndx].callback = callback;
    timerList[ndx].parameters = parameters;

    __disable_irq();

    timerList[ndx].currentValue = count;

    __enable_irq();

    return 0;
}

void TIM1_BRK_TIM9_IRQHandler()
{
    uint32_t ndx;
    for (ndx = 0; ndx < MAX_NUM_CALLBACK_TIMERS; ndx++)
    {
        if (timerList[ndx].resetValue != 0)
        {
            timerList[ndx].currentValue--;
            if (timerList[ndx].currentValue == 0)
            {
                if (timerList[ndx].callback(timerList[ndx].parameters) == DISABLE_TIMER)
                {
                    timerList[ndx].resetValue = 0;
                }

                timerList[ndx].currentValue = timerList[ndx].resetValue;
            }
        }
    }


    TIM_ClearITPendingBit(CALLBACK_TIMER, TIM_EventSource_Update);
}