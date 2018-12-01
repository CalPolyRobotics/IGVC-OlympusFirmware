#include "speed.h"
#include "stm32f0xx_hal.h"
#include "systemClock.h"
#include <stdint.h>

#define PING_PONG_SIZE ((size_t) 2u)

#define MM_PER_INT     ((uint32_t) 220u)
#define CK_FREQ        ((uint32_t) 48000000u)
#define MIN_TIMEOUT    ((uint32_t) 2500u)

uint16_t ch1Speed[2];
uint16_t ch2Speed[2];


/**
 * The distance between magnets is different enough that the speed value without averaging
 * is a 6 pulse periodic signal with varying speed values all offset from that actual
 * speed value
 */
#define NUM_IN_AVGS 6
uint16_t ch1SpeedAvg[NUM_IN_AVGS] = {0};
uint16_t ch2SpeedAvg[NUM_IN_AVGS] = {0};
uint32_t ch1SpeedSum = 0;
uint32_t ch2SpeedSum = 0;
volatile int ch1SpeedAvgIdx = 0;
volatile int ch2SpeedAvgIdx = 0;

volatile int ch1SpeedWr = 0;
volatile int ch2SpeedWr = 0;

static uint16_t diffCountsToSpeedInMMs(uint32_t diffCounts);

void TIM2_IRQHandler() {
    static uint32_t ch1Count = 0;
    static uint32_t ch2Count = 0;

    if (TIM2->SR & TIM_SR_CC2IF) {
        uint32_t chDiff;
        uint32_t cap = TIM2->CCR2;

        if (cap >= ch1Count) {
            chDiff = cap - ch1Count;
        } else {
            chDiff = (UINT32_MAX - ch1Count) + cap;
        }

        ch1Count = cap;

        // Update Sum By Subtracting previous value from sum and adding new value
        ch1SpeedSum -= ch1SpeedAvg[ch1SpeedAvgIdx];
        ch1SpeedAvg[ch1SpeedAvgIdx] = diffCountsToSpeedInMMs(chDiff);
        ch1SpeedSum += ch1SpeedAvg[ch1SpeedAvgIdx];
        ch1SpeedAvgIdx = (ch1SpeedAvgIdx + 1) % NUM_IN_AVGS;

        ch1Speed[ch1SpeedWr] = ch1SpeedSum / NUM_IN_AVGS;
        ch1SpeedWr = (ch1SpeedWr + 1) % 2;
    }

    if (TIM2->SR & TIM_SR_CC4IF) {
        uint32_t chDiff;
        uint32_t cap = TIM2->CCR4;

        if (cap >= ch2Count) {
            chDiff = cap - ch2Count;
        } else {
            chDiff = (UINT32_MAX - ch2Count) + cap;
        }

        ch2Count = cap;

        // Update Sum By Subtracting previous value from sum and adding new value
        ch2SpeedSum -= ch2SpeedAvg[ch2SpeedAvgIdx];
        ch2SpeedAvg[ch2SpeedAvgIdx] = diffCountsToSpeedInMMs(chDiff);
        ch2SpeedSum += ch2SpeedAvg[ch2SpeedAvgIdx];
        ch2SpeedAvgIdx = (ch2SpeedAvgIdx + 1) % NUM_IN_AVGS;

        ch2Speed[ch2SpeedWr] = ch2SpeedSum / NUM_IN_AVGS;
        ch2SpeedWr = (ch2SpeedWr + 1) % 2;
    }
}

uint16_t diffCountsToSpeedInMMs(uint32_t diffCounts) {
    if (diffCounts != 0) {
        // Multiply CK_FREQ by 64 to decrease error of integer division
        uint32_t ckpdiff = ((CK_FREQ * 64) / diffCounts);

        // Remove 64 to get final speed
        return (uint16_t)((ckpdiff * MM_PER_INT) / 64);
    }

    return UINT16_MAX;
}

/**
void checkTimeout() {
    uint32_t currSystick = systickCount;
    uint32_t curr = TIM2 -> CNT;

   TIM2 -> DIER &= ~TIM_DIER_CC1IE;
    if (currSystick > (ch1SysCount + MIN_TIMEOUT)) {
       ch1Speed[ch1SpeedWr] = 0;
       ch1SysCount = systickCount;
       ch1Count = curr;
    }
   TIM2 -> DIER |= TIM_DIER_CC1IE;

   TIM2 -> DIER &= ~TIM_DIER_CC2IE;
   if (currSystick > (ch2SysCount + MIN_TIMEOUT)) {
       ch2Speed[ch2SpeedWr] = 0;
       ch2SysCount = systickCount;
       ch2Count = curr;
    }
   TIM2 -> DIER |= TIM_DIER_CC2IE;
}
**/
