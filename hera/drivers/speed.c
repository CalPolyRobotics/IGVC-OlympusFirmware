#include "speed.h"
#include "stm32f0xx_hal.h"
#include "systemClock.h"
#include <stdint.h>
#include <string.h>

#define UM_PER_INT     ((uint32_t) 1075u)
#define CK_FREQ        ((uint32_t) 48000000u)
#define MIN_TIMEOUT    ((uint32_t) 1075u)

#define PING_PONG_SIZE ((size_t) 2u)

int16_t ch1Speed[2];

volatile int ch1SpeedWr = 0;
volatile uint32_t ch1SysCount = 0;

void TIM2_IRQHandler() {
    static uint32_t ch1Count = 0;

    if (TIM2->SR & TIM_SR_CC2IF) {
        uint32_t chDiff;
        uint32_t cap = TIM2->CCR2;

        if (cap >= ch1Count) {
            chDiff = cap - ch1Count;
        } else {
            chDiff = (UINT32_MAX - ch1Count) + cap;
        }

        ch1Count = cap;

        ch1Speed[ch1SpeedWr] = diffCountsToSpeedInMMs(chDiff);
        ch1SpeedWr = (ch1SpeedWr + 1) % PING_PONG_SIZE;

        ch1SysCount = systickCount;
    }
}

int16_t diffCountsToSpeedInMMs(uint32_t diffCounts) {
    // diffCounts of 0 is mapped to 0 speed
    int16_t direction = 1;
    if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_3) == GPIO_PIN_SET){
        direction = -1;
    }

    if(diffCounts != 0u){
        // Multiply CK_FREQ by 64 to decrease error of integer division
        uint32_t ckpdiff = ((CK_FREQ * 64u) / diffCounts);


        // Remove 64 to get final speed and divide by 1000 to get speed in mm/s
        return direction * (int16_t)((ckpdiff * UM_PER_INT) / 64000u);
    }

    return 0u;
}

void checkTimeout() {
    uint32_t currSystick = systickCount;

    TIM2 -> DIER &= ~TIM_DIER_CC1IE;
    if (currSystick > (ch1SysCount + MIN_TIMEOUT)) {
        ch1Speed[ch1SpeedWr] = 0;
        ch1SpeedWr = (ch1SpeedWr + 1) % PING_PONG_SIZE;

        ch1SysCount = systickCount;
    }
    TIM2 -> DIER &= ~TIM_DIER_CC1IE;
}
