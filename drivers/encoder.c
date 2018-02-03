#include "main.h"

#include "encoder.h"

#include "stdio.h"

#define CIRCUMFRENCE 1.4 // Meters
#define MAX_COUNT (0x1FFFFFFF)

#define TIM2_PRESCALER 0 // 511 // Timer is max 120 MHz
#define TIM5_PRESCALER 0 // 511  // Timer is max 60 MHz
//#define METER_TICKS_PER_SECOND 54405000 // Measured Value Times 1000 to maintain precision
#define METER_TICKS_PER_SECOND 13927680000 //27202500
//#define METER_TICKS_PER_SECOND 27203 // MTPS / 1000 to calculate micro meters / s

#define SPEED_ARRAY_SIZE 8
 
// Meter Ticks / Second can be calculated as
// CIRCUMFRENCE / (ENCODERS_PER_ROTATION * (PRESCALE/(2 *CLK_FREQ)))

static uint32_t leftSpeedValues[SPEED_ARRAY_SIZE] = {0};
static uint32_t rightSpeedValues[SPEED_ARRAY_SIZE] = {0};

static uint8_t leftSpeedIdx = 0;
static uint8_t rightSpeedIdx = 0;

volatile uint16_t speedCommsValue[2] = {0}; //0 - Left, 1 - Right;

void initEncoderInputCapture() {
    //INITIALIZE HALL EFFECT 1 
    GPIO_InitTypeDef gpio;
    gpio.Pin = GPIO_PIN_5;
    gpio.Mode = GPIO_MODE_AF_OD;
    gpio.Pull = GPIO_PULLUP;
    gpio.Speed = GPIO_SPEED_LOW;
    gpio.Alternate = GPIO_AF1_TIM2;

    HAL_GPIO_Init(GPIOA, &gpio);  

    //Setup TIM2_CH1
    TIM2->CCMR1 = TIM_CCMR1_CC1S_0 | //set capture input to TI1
                  TIM_CCMR1_IC1F_0 | TIM_CCMR1_IC1F_1 | TIM_CCMR1_IC1F_2; // Filtering - Fsamp=Fdts/4 N=8

    TIM2->CCER =  TIM_CCER_CC1NP | //falling edge sensitivity
                  TIM_CCER_CC1E; //enable capture on TI1
    
    TIM2->DIER = TIM_DIER_CC1IE | //enable interrupts on timer 
                 TIM_DIER_UIE;
    

    TIM2->PSC = TIM2_PRESCALER;

    TIM2->ARR = MAX_COUNT;

    TIM2->CR1 = TIM_CR1_CEN; //enable the timer


    //ENABLE INTERRUPTS
    NVIC_SetPriority(TIM2_IRQn, 32);
    NVIC_EnableIRQ(TIM2_IRQn);

    //INITIALIZE HALL EFFECT 2 
    gpio.Pin = GPIO_PIN_0;
    gpio.Mode = GPIO_MODE_AF_OD;
    gpio.Pull = GPIO_PULLUP;
    gpio.Speed = GPIO_SPEED_LOW;
    gpio.Alternate = GPIO_AF2_TIM5;

    HAL_GPIO_Init(GPIOA, &gpio);
    
    //Setup TIM5_CH1
    TIM5->CCMR1 = TIM_CCMR1_CC1S_0 | //set capture input to TI1
                  TIM_CCMR1_IC1F_0 | TIM_CCMR1_IC1F_1 | TIM_CCMR1_IC1F_2; //set filter
    
    TIM5->CCER = TIM_CCER_CC1NP | //falling edge sensitivity
                 TIM_CCER_CC1E;  //enable capture on TI1
    
    TIM5->DIER = TIM_DIER_CC1IE | //enable interrupts on timer 
                 TIM_DIER_UIE; 

    TIM5->PSC = TIM5_PRESCALER; 

    TIM5->ARR = MAX_COUNT;

    TIM5->CR1 = TIM_CR1_CEN; //enable the timer

    //ENABLE INTERRUPTS
    NVIC_SetPriority(TIM5_IRQn, 31);
    NVIC_EnableIRQ(TIM5_IRQn);
}

static uint16_t calcFilteredSpeedValue(uint32_t* speedArray)
{
    uint8_t i;
    uint32_t accum = 0;
    for (i = 0; i < SPEED_ARRAY_SIZE; i++)
    {
        accum += speedArray[i];
    }

    return METER_TICKS_PER_SECOND/(accum / SPEED_ARRAY_SIZE);
}


void TIM2_IRQHandler(){
    static  uint8_t overflow = 0;

    if (TIM2-> SR & TIM_SR_UIF) {
        overflow = 1;

        leftSpeedValues[leftSpeedIdx] =  MAX_COUNT;
        leftSpeedIdx = (leftSpeedIdx + 1) % SPEED_ARRAY_SIZE;

        TIM2->EGR |= TIM_EGR_UG;

        uint16_t filteredSpeed = calcFilteredSpeedValue(leftSpeedValues);
        speedCommsValue[0] = (filteredSpeed >> 8) | ((filteredSpeed & 0xFF) << 8);
    }

    if (TIM2->SR & TIM_SR_CC1IF)
    {
        if(overflow){
            overflow = 0;
        }else{
            leftSpeedValues[leftSpeedIdx] = TIM2->CCR1;
            leftSpeedIdx = (leftSpeedIdx + 1) % SPEED_ARRAY_SIZE;
        }

        TIM2->EGR |= TIM_EGR_UG;

        uint16_t filteredSpeed = calcFilteredSpeedValue(leftSpeedValues);
        speedCommsValue[0] = (filteredSpeed >> 8) | ((filteredSpeed & 0xFF) << 8);
    }

    TIM2->SR = 0;
}

void TIM5_IRQHandler(){
    static  uint8_t overflow = 0;

    if (TIM5-> SR & TIM_SR_UIF) {
        overflow = 1;

        rightSpeedValues[rightSpeedIdx] =  MAX_COUNT;
        rightSpeedIdx = (rightSpeedIdx + 1) % SPEED_ARRAY_SIZE;

        TIM5->EGR |= TIM_EGR_UG;

        uint16_t filteredSpeed = calcFilteredSpeedValue(rightSpeedValues);
        speedCommsValue[1] = (filteredSpeed >> 8) | ((filteredSpeed & 0xFF) << 8);
    }

    if (TIM5->SR & TIM_SR_CC1IF)
    {
        if(overflow){
            overflow = 0;
        }else{
            rightSpeedValues[rightSpeedIdx] = TIM5->CCR1;
            rightSpeedIdx = (rightSpeedIdx + 1) % SPEED_ARRAY_SIZE;
        }

        TIM5->EGR |= TIM_EGR_UG;

        uint16_t filteredSpeed = calcFilteredSpeedValue(rightSpeedValues);
        speedCommsValue[1] = (filteredSpeed >> 8) | ((filteredSpeed & 0xFF) << 8);
    }

    TIM5->SR = 0;
}
