#include "main.h"

#include "encoder.h"

#include "stdio.h"

#define DIAMETER 1.4 // Meters
#define MAX_COUNT 65535
#define TIM10_PRESCALER 511 // Timer is max 120 MHz
#define TIM12_PRESCALER 511 // Timer is max 60 MHz
//#define METER_TICKS_PER_SECOND 54405000 // Measured Value Times 1000 to maintain precision
#define METER_TICKS_PER_SECOND 27202500

// Meter Ticks / Second can be calculated as
// DIAMETER / (ENCODERS_PER_ROTATION * (PRESCALE/(2 *CLK_FREQ)))

static uint16_t leftSpeedValues[8] = {0};
static uint16_t rightSpeedValues[8] = {0};
static uint8_t leftSpeedIdx = 0;
static uint8_t rightSpeedIdx = 0;
volatile uint16_t speedCommsValue[2] = {0}; //0 - Left, 1 - Right;

void initEncoderInputCapture() {
    //INITIALIZE HALL EFFECT 1 
    GPIO_InitTypeDef gpio;
    gpio.Pin = GPIO_PIN_8;
    gpio.Mode = GPIO_MODE_AF_OD;
    gpio.Pull = GPIO_PULLUP;
    gpio.Speed = GPIO_SPEED_LOW;
    gpio.Alternate = GPIO_AF3_TIM10;

    HAL_GPIO_Init(GPIOB, &gpio);  
    
    //Setup TIM10_CH1
    TIM10->CCMR1 = TIM_CCMR1_CC1S_0 | //set capture input to TI1
                    (TIM_CCMR1_IC1F_0 | TIM_CCMR1_IC1F_1 |
                     TIM_CCMR1_IC1F_2); //set filter
    

    TIM10->CCER = (TIM_CCER_CC1NP | TIM_CCER_CC1P) | //determine edge sensitivity
                    TIM_CCER_CC1E;  //enable capture on TI1
    
    TIM10->DIER = TIM_DIER_CC1IE | //enable interrupts on timer 
                  TIM_DIER_UIE;
    

    TIM10->PSC = TIM10_PRESCALER;
    TIM10->CR1 = TIM_CR1_URS | TIM_CR1_CEN; //enable the timer


    //ENABLE INTERRUPTS
    NVIC_SetPriority(TIM1_UP_TIM10_IRQn, 32);
    NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn);

    //INITIALIZE HALL EFFECT 2 
    gpio.Pin = GPIO_PIN_14;
    gpio.Mode = GPIO_MODE_AF_OD;
    gpio.Pull = GPIO_PULLUP;
    gpio.Speed = GPIO_SPEED_LOW;
    gpio.Alternate = GPIO_AF9_TIM12;

    HAL_GPIO_Init(GPIOB, &gpio);  
    
    //Setup TIM12_CH1
    TIM12->CCMR1 = TIM_CCMR1_CC1S_0 | //set capture input to TI1
                    (TIM_CCMR1_IC1F_0 | TIM_CCMR1_IC1F_1 |
                     TIM_CCMR1_IC1F_2); //set filter
    

    TIM12->CCER = (TIM_CCER_CC1NP) | //determine edge sensitivity
                    TIM_CCER_CC1E;  //enable capture on TI1
    
    TIM12->DIER = TIM_DIER_CC1IE; //enable interrupts on timer 

    TIM12->PSC = TIM12_PRESCALER; 

    TIM12->CR1 = TIM_CR1_CEN; //enable the timer

    //ENABLE INTERRUPTS
    NVIC_SetPriority(TIM8_BRK_TIM12_IRQn, 31);
    NVIC_EnableIRQ(TIM8_BRK_TIM12_IRQn);
}

static uint16_t calcFilteredSpeedValue(uint16_t* speedArray)
{
    uint8_t i;
    uint32_t accum = 0;
    for (i = 0; i < (sizeof(leftSpeedValues) / 2); i++)
    {
        accum += speedArray[i];
    }

    return METER_TICKS_PER_SECOND/(accum >> 3);
}


void TIM1_UP_TIM10_IRQHandler() {
    // uint16_t speed = (uint8_t)(getMaximumMMPerSecond() / TIM10->CCR1);
    if (TIM10->SR & TIM_SR_CC1IF)
    {
        leftSpeedValues[leftSpeedIdx++] = TIM10->CCR1;
        if (leftSpeedIdx >= (sizeof(leftSpeedValues) / 2))
        {
            leftSpeedIdx = 0;
        }
        uint16_t filteredSpeed = calcFilteredSpeedValue(leftSpeedValues);
        speedCommsValue[0] = (filteredSpeed >> 8) | ((filteredSpeed & 0xFF) << 8);
        TIM10->EGR |= TIM_EGR_UG;

        // printf("LEFT: %u %lu\r\n", speed, TIM10->CNT); 
    }

    if (TIM10->SR & TIM_SR_UIF) {
        // printf("OVF\r\n");
        speedCommsValue[0] = 0;
    }

    TIM10->SR = 0;
}

//TODO: Make sure the speed measurement AVOIDS floating point, adjust calculations to also avoid floating point
//     ADDITIONALLY: 

void TIM8_BRK_TIM12_IRQHandler() {
    // uint16_t speed = (uint8_t)(getMaximumMMPerSecond() / TIM12->CCR1);
    if (TIM12->SR & TIM_SR_CC1IF)
    {
        rightSpeedValues[rightSpeedIdx++] = TIM12->CCR1;
        if (rightSpeedIdx >= (sizeof(rightSpeedValues) / sizeof(rightSpeedValues[0])))
        {
            rightSpeedIdx = 0;
        }

        uint16_t filteredSpeed = calcFilteredSpeedValue(rightSpeedValues);
        speedCommsValue[1] = (filteredSpeed >> 8) | ((filteredSpeed & 0xFF) << 8);
        TIM12->EGR |= TIM_EGR_UG;
    }

    if (TIM12->SR & TIM_SR_UIF)
    {
        speedCommsValue[1] = 0;
    }

    TIM12->SR = 0;
}

