#include "main.h"

#include "encoder.h"

#define PULSE_PER_ROTATION 6
#define WHEEL_CIRCUMFERENCE_MUL 91 //measured in meters 
#define WHEEL_CIRCUMFERENCE_DIV 64 //measured in meters 
#define MINIMUM_MINIMUM_SPM 10
#define MAX_COUNT 65535

static const uint32_t CLOCK_FREQUENCY = 60; //SET IN MHZ
//static uint32_t TIMER_FREQUENCY = ; //SET IN MHZ
static uint32_t MINIMUM_SECOND_PER_METER = MINIMUM_MINIMUM_SPM; 
static uint32_t PRESCALER = 0;

static uint16_t leftSpeedValues[8] = {0};
static uint16_t rightSpeedValues[8] = {0};
static uint8_t leftSpeedIdx = 0;
static uint8_t rightSpeedIdx = 0;
volatile uint16_t speedCommsValue[2] = {0}; //0 - Left, 1 - Right;


void setPrescaleValue() {
    if (MINIMUM_SECOND_PER_METER) {
       PRESCALER = CLOCK_FREQUENCY * (1000000) * 
                   WHEEL_CIRCUMFERENCE_MUL * MINIMUM_SECOND_PER_METER
                    /(MAX_COUNT * 6 * WHEEL_CIRCUMFERENCE_DIV);    
    }
}

void setMinimumMetersPerSecond(uint32_t target) {
    //MINIMUM_METERS_PER_SECOND = target;
    //setPrescaleValue();
}

uint32_t getMaximumMMPerSecond() {
    return (MAX_COUNT * 1000/ MINIMUM_MINIMUM_SPM);
}

void initEncoderInputCapture() {
    //INITIALIZE HALL EFFECT 1 
    setPrescaleValue();

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
    

    // TIM10->PSC = PRESCALER; 
    TIM10->PSC = 500;
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

    TIM12->PSC = 500; 

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

    return accum >> 3;
}


void TIM1_UP_TIM10_IRQHandler() {
    // uint16_t speed = (uint8_t)(getMaximumMMPerSecond() / TIM10->CCR1);
    if (TIM10->SR & TIM_SR_CC1IF)
    {
           uint16_t speed = TIM10->CCR1;
        leftSpeedValues[leftSpeedIdx] = speed;
        leftSpeedIdx++;
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
    }

    TIM10->SR = 0;
}

//TODO: Make sure the speed measurement AVOIDS floating point, adjust calculations to also avoid floating point
//     ADDITIONALLY: 

void TIM8_BRK_TIM12_IRQHandler() {
    // uint16_t speed = (uint8_t)(getMaximumMMPerSecond() / TIM12->CCR1);
    if (TIM12->SR & TIM_SR_CC1IF)
    {
        uint16_t speed = TIM12->CCR1;
        rightSpeedValues[rightSpeedIdx] = speed;
        rightSpeedIdx++;
        if (rightSpeedIdx >= (sizeof(rightSpeedValues) / 2))
        {
            rightSpeedIdx = 0;
        }
        // uint16_t filteredSpeed = calcFilteredSpeedValue(rightSpeedValues);
        // speedCommsValue[1] = (filteredSpeed >> 8) | ((filteredSpeed & 0xFF) << 8);
        speedCommsValue[1] = 0xAABB;
        TIM12->EGR |= TIM_EGR_UG;
    }

    if (TIM12->SR & TIM_SR_UIF)
    {

    }

}

