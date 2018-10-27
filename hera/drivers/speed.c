#include "speed.h"
#include "stm32f0xx_hal.h"
#include <stdint.h>

volatile uint32_t ch1Count = 0;
volatile uint32_t ch1Diff = 0;
volatile uint32_t ch2Count = 0;
volatile uint32_t ch2Diff = 0;

void TIM2_IRQHandler() {
    if (TIM2 -> SR & TIM_SR_CC1IF) {
        TIM2 -> SR &= ~TIM_SR_CC1IF; 
        uint32_t curr = TIM2 -> CNT;
        if (curr >= ch1Diff) {
            ch1Diff = curr - ch1Count;
        } else {
            ch1Diff = UINT32_MAX - ch1Count + curr;
        }
        ch1Count = curr;  
    }
    if (TIM2 -> SR & TIM_SR_CC2IF) {
        TIM2 -> SR &= ~TIM_SR_CC2IF;
        uint32_t curr = TIM2 -> CNT;
        if (curr >= ch2Diff) {
            ch2Diff = curr - ch2Count;
        } else {
            ch2Diff = UINT32_MAX - ch2Count + curr;
        }
        ch2Count = curr;  
    }
}
