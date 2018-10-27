#include "speed.h"
#include "stm32f0xx_hal.h"
#include "systemClock.h"
#include <stdint.h>

#define MM_PER_INT ((uint32_t) 220)
#define CK_FREQ ((uint32_t) 48000000)
#define MIN_TIMEOUT ((uint32_t) 2500)

static uint32_t ch1Count = 0;
static uint32_t ch1Diff = 0;
static uint32_t ch1SysCount = 0;

volatile uint32_t ch1Speed[2];
volatile int ch1SpeedWr = 0;

static uint32_t ch2Count = 0;
static uint32_t ch2Diff = 0;
static uint32_t ch2SysCount = 0;

volatile uint32_t ch2Speed[2];
volatile int ch2SpeedWr = 0;

void TIM2_IRQHandler() {
    uint32_t curr = TIM2 -> CNT;
    if (TIM2 -> SR & TIM_SR_CC1IF) {
        ch1SysCount = systickCount;
        TIM2 -> SR &= ~TIM_SR_CC1IF; 
        if (curr >= ch1Count) {
            ch1Diff = curr - ch1Count;
        } else {
            ch1Diff = UINT32_MAX - ch1Count + curr;
        }
        ch1Count = curr;  
        ch1Speed[ch1SpeedWr] = diffCountsToSpeedInMMs(ch1Diff);
        ch1SpeedWr = (ch1SpeedWr + 1) % 2;
    }
    if (TIM2 -> SR & TIM_SR_CC2IF) { 
        ch2SysCount = systickCount;
        TIM2 -> SR &= ~TIM_SR_CC2IF;
        if (curr >= ch2Count) {
            ch2Diff = curr - ch2Count;
        } else {
            ch2Diff = UINT32_MAX - ch2Count + curr;
        }
        ch2Count = curr;  
        ch2Speed[ch2SpeedWr] = diffCountsToSpeedInMMs(ch2Diff);
        ch2SpeedWr = (ch2SpeedWr + 1) % 2;
    }
}

uint32_t diffCountsToSpeedInMMs(uint32_t diffCounts) {
    if (diffCounts != 0) {
        return (MM_PER_INT * CK_FREQ) / diffCounts;
    }
    return UINT32_MAX;
}

void checkTimeout() {
    uint32_t currSystick = systickCount;  
    uint32_t curr = TIM2 -> CNT;
    if (currSystick > (ch1SysCount + MIN_TIMEOUT)) {
       TIM2 -> DIER &= ~TIM_DIER_CC1IE; 
       ch1Speed[ch1SpeedWr] = 0;
       ch1SysCount = systickCount;
       ch1Count = curr;
       TIM2 -> DIER |= TIM_DIER_CC1IE; 
    } 
    if (currSystick > (ch2SysCount + MIN_TIMEOUT)) {
       TIM2 -> DIER &= ~TIM_DIER_CC2IE; 
       ch2Speed[ch2SpeedWr] = 0;
       ch2SysCount = systickCount;
       ch2Count = curr;
       TIM2 -> DIER |= TIM_DIER_CC2IE; 
    }
}
