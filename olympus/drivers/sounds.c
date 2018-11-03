#include "sounds.h"
#include "tim.h"
#include "stm32f2xx_hal.h"
#include "timerCallback.h"
#include <stdint.h>

#define C4 6849
#define c4 6464
#define D4 6101
#define d4 6101
#define E4 5435
#define F4 5130
#define f4 4842
#define G4 4570
#define g4 4314
#define A4 4072
#define a4 3844
#define B4 3628
#define C5 3424
#define c5 3232
#define D5 3050
#define d5 2879
#define E5 2717
#define F5 2565
#define f5 2421
#define G5 2285
#define g5 2157
#define A5 2035
#define a5 1922
#define B5 1814
#define C6 1712
#define c6 1616
#define D6 1525
#define d6 1440
#define E6 1359 

note sounds[]= {
{d6, 3},
{d5, 1},
{a5, 4},
{g5, 5},
{d6, 3},
{a5, 8}
};
  
uint8_t idx = 0;

uint16_t noteslen[13] = {
    0, 107, 214, 321, 428, 535, 642, 750, 857, 964, 1071, 1178, 1285};
    //to calculate length use #16th notes * .25 * (1/tempo)*1000*60

Timer_Return timing(void*dummy)
{
    
    if (idx < sizeof(sounds)/sizeof(note )){
    playNote(sounds[idx].freq);
    addCallbackTimer(noteslen[sounds[idx].numnotes], timing, NULL);
    idx+=1 ;
    }
    else if (idx == sizeof(sounds)/sizeof(note)){
    playNote(0);
    }
    return DISABLE_TIMER;
    }

void playNote(uint16_t fre){
    TIM4 -> ARR = fre*2;
    TIM4 -> CCR3 = fre;
}

void Tim_Init(void){
    __GPIOB_CLK_ENABLE();
    __TIM4_CLK_ENABLE();
    TIM4 -> PSC = 19;
    TIM4 -> CCMR2 = (TIM_CCMR2_OC3M_0 | TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3M_2);
    TIM4 -> CCER = TIM_CCER_CC3E;   
    TIM4 -> CR1 = TIM_CR1_CEN;
    TIM4 -> ARR = 24000;
    TIM4 -> CCR3 = 24000;

    GPIO_InitTypeDef GPIO_InitTypeDef;
    GPIO_InitTypeDef.Pin = (BUZZER);
    GPIO_InitTypeDef.Pull = GPIO_NOPULL;
    GPIO_InitTypeDef.Mode = GPIO_MODE_AF_PP;
    GPIO_InitTypeDef.Speed = GPIO_SPEED_LOW;
    GPIO_InitTypeDef.Alternate = GPIO_AF2_TIM4;
    HAL_GPIO_Init(GPIOB, &GPIO_InitTypeDef ); 
}

void Song(void){
    Tim_Init();    
    addCallbackTimer(1, timing, NULL);  
}
