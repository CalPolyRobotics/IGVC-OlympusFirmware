#include "tim.h"
#include "config.h"
#include "timerCallback.h"

typedef struct CallbackTimer_t {
    uint32_t currentValue;
    uint32_t resetValue;
    timerCallback callback;
    void* parameters;
} CallbackTimer;

static CallbackTimer timerList[MAX_NUM_CALLBACK_TIMERS];

static volatile uint8_t shouldServiceCallbackTimer = 0;

// Initialize the timer used for callbacks.
// The callback frequency is set by the CALLBACK_FREQUENCE
// assuming that TIMER_FREQUENCY is correct.
void initIGVCCallbackTimer()
{
    CALLBACK_TIMER->DIER = TIM_DIER_UIE;

    CALLBACK_TIMER->PSC = (APB2_TMR_CLK_FREQ / TIMER_FREQUENCY) - 1;
    CALLBACK_TIMER->ARR = (TIMER_FREQUENCY / CALLBACK_FREQUENCY) - 1; 

    NVIC_SetPriority(CALLBACK_TIMER_IRQ, 0xF);  //Lowest Pre-emption Priority
    NVIC_EnableIRQ(CALLBACK_TIMER_IRQ);

    CALLBACK_TIMER->CR1 = TIM_CR1_CEN | TIM_CR1_ARPE;

    uint32_t ndx;
    for (ndx = 0; ndx < MAX_NUM_CALLBACK_TIMERS; ndx++)
    {
        timerList[ndx].resetValue = 0;
    }
}

// Adds a callback timer to the timerList.
// The function (callback) will be called with (parameters) as an argument.
// The frequency is CALLBACK_FREQUENCY / (count)
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

    timerList[ndx].currentValue = count;

    return 0;
}

// Should be called once per main loop to handle recent callback events
void serviceCallbackTimer()
{
    uint32_t ndx;

    if (shouldServiceCallbackTimer)
    {
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

        shouldServiceCallbackTimer = 0;
    }
}

void led6Toggle(void*);

// The ISR called by the timer
void CALLBACK_TIMER_ISR()
{
    shouldServiceCallbackTimer = 1;

    CALLBACK_TIMER->SR &= ~(TIM_SR_UIF);
}

