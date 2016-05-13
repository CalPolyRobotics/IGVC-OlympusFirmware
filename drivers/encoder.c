#include "encoder.h"
#define ROTATION_LENGTH 0xFF //number of inputs to signify full rotation (currently ARBITRARY)

void initEncoderInputCapture() {
    //initialize GPIO
    GPIO_InitTypeDef gpio;
    gpio.Pin = GPIO_PIN_8;
    gpio.Mode = GPIO_MODE_AF_OD;
    gpio.Pull = GPIO_PULLDOWN;
    gpio.Speed = GPIO_SPEED_LOW;
    gpio.Alternate = GPIO_AF3_TIM10;

    HAL_GPIO_Init(GPIOB, &gpio);  
    
    //Setup TIM10_CH1
    TIM10->CCMR1 = TIM_CCMR1_CC1S_0 | //set capture input to TI1
                    (TIM_CCMR1_IC1F_0 | TIM_CCMR1_IC1F_1 |
                     TIM_CCMR1_IC1F_2); //set filter
    

    TIM10->CCER = (TIM_CCER_CC1NP) | //determine edge sensitivity
                    TIM_CCER_CC1E;  //enable capture on TI1
    
    TIM10->DIER = TIM_DIER_CC1IE; //enable interrupts on timer 

    TIM10->CR1 = TIM_CR1_CEN; //enable the timer
    //ENABLE INTERRUPTS
    NVIC_SetPriority(TIM1_UP_TIM10_IRQn, 32);
    NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn);
}

void TIM1_UP_TIM10_IRQHandler() {
    printf("AAAAHHCOUNT: %lu VALUE: %lu\n", TIM10->CNT, TIM10->CCR1);
}





