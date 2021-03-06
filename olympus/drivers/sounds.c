#include "sounds.h"
#include "tim.h"
#include "stm32f2xx_hal.h"
#include "timerCallback.h"
#include <stdint.h>

//these are the correct counts for these particular notes
#define C4 5707  
#define c4 5386
#define D4 5084
#define d4 4798
#define E4 4529
#define F4 4275
#define f4 4035
#define G4 3808
#define g4 3595
#define A4 3393
#define a4 3202
#define B4 3023
#define C5 2853
#define c5 2693
#define D5 2542
#define d5 2399
#define E5 2265
#define F5 2138
#define f5 2017
#define G5 1904
#define g5 1797
#define A5 1696
#define a5 1601
#define B5 1511
#define C6 1426
#define c6 1346
#define D6 1271
#define d6 1199
#define E6 1132
#define g7 899

note windows[]= {  //notes and lengths needed to play windows start up sound
    {d6, 3},
    {d5, 1},
    {a5, 4},
    {g5, 5},
    {d6, 3},
    {a5, 8}
};

note shutdown[]={   //plays the windows shutdown sound 
    {g7, 3},
    {d6, 3},
    {g5, 3},
    {a5, 4}
};
  
uint8_t idx = 0; //sets the index to 0

uint16_t noteslen[13] = {
    0, 107, 214, 321, 428, 535, 642, 750, 857, 964, 1071, 1178, 1285};
    //to calculate length use #16th notes * .25 * (1/tempo)*1000*60

Timer_Return startup(void*dummy)//creates a call-back timer that will check back in
{
    
    if (idx < sizeof(windows)/sizeof(note ))
    {
        playNote(windows[idx].freq); //play the note give by the struct
        addCallbackTimer(noteslen[windows[idx].numnotes], startup, NULL);//create a new callback timer
        idx+=1 ;//add one to the index 
    }
    else if (idx == sizeof(windows)/sizeof(note))
    {
        playNote(0);// set the output to 0 so that no note plays
    }
    return DISABLE_TIMER;//kill the current timer
    }

void playNote(uint16_t fre){//plays a note 
    TIM4 -> ARR = fre*2;//set he auto-reload register to 2 the number given
    TIM4 -> CCR3 = fre;//sets the capture complare register to the number passed in
}

void Tim_Init(void){//intializes the timer so that a frequency can be assigned to it
    __GPIOB_CLK_ENABLE();
    __TIM4_CLK_ENABLE();
    TIM4 -> PSC = 19;
    TIM4 -> CCMR2 = (TIM_CCMR2_OC3M_0 | TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3M_2);
    TIM4 -> CCER = TIM_CCER_CC3E;   
    TIM4 -> CR1 = TIM_CR1_CEN;
    TIM4 -> ARR = 24000;
    TIM4 -> CCR3 = 24000;

    GPIO_InitTypeDef GPIO_InitTypeDef;
    GPIO_InitTypeDef.Pin = (BUZZER_PIN);
    GPIO_InitTypeDef.Pull = GPIO_NOPULL;
    GPIO_InitTypeDef.Mode = GPIO_MODE_AF_PP;
    GPIO_InitTypeDef.Speed = GPIO_SPEED_LOW;
    GPIO_InitTypeDef.Alternate = GPIO_AF2_TIM4;
    HAL_GPIO_Init(GPIOB, &GPIO_InitTypeDef ); 
}

void runStartupSong(void){
    Tim_Init();    
    addCallbackTimer(1, startup, NULL);  
}

Timer_Return failure(void*dummy){//plays the windows shutdown sounds with callback timers
    if (idx < sizeof(shutdown)/sizeof(note ))
    {
        playNote(shutdown[idx].freq);
        addCallbackTimer(noteslen[shutdown[idx].numnotes], failure, NULL);
        idx+=1 ;                                      
    }                                             
    else if (idx == sizeof(shutdown)/sizeof(note))
    {
        playNote(0);
    }

    return DISABLE_TIMER;
}

void runErrorSong(void){
    addCallbackTimer(1, failure, NULL);
}
