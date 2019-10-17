#include "steer_speed.h"
#include "stm32f0xx_hal.h"
#include "systemClock.h"
#include <stdint.h>
#include <string.h>

#define PING_PONG_SIZE (2u)

#define UM_PER_INT     ((uint32_t) 1075u)
#define CK_FREQ        ((uint32_t) 48000000u)
// TODO - verify this number
#define TICKS_PER_REV  ((uint32_t) 200u)

// TODO
// Assuming 1:1 ratio
// (5mRev/s) * (200ticks/rev) => 1 tick/s
// Thus after 1 second and no tick, we know we are going less than
// 5mRev/s and the speed will get set to zero
// Therefore 5mRev/s is the slowest speed we can mesasure
#define MIN_TIMEOUT    ((uint32_t) 1000u)

int16_t ch3Speed[2];

volatile int ch3SpeedWr = 0;
volatile uint32_t ch3SysCount = 0;

void TIM2_IRQHandler() {
    static uint32_t ch3Count = 0;

    if (TIM2->SR & TIM_SR_CC3IF) {
        uint32_t chDiff;
        uint32_t cap = TIM2->CCR3;

        if (cap >= ch3Count) {
            chDiff = cap - ch3Count;
        } else {
            chDiff = (UINT32_MAX - ch3Count) + cap;
        }

        ch3Count = cap;

        // TODO - No need to do this costly calculation until requested
        ch3Speed[ch3SpeedWr] = clk_ticks_to_speed_mRev_s(chDiff);
        ch3SpeedWr = (ch3SpeedWr + 1) % PING_PONG_SIZE;

        ch3SysCount = systickCount;
    }
}

// TODO - Check that speed does not exceed 32000 mRev/s
int16_t clk_ticks_to_speed_mRev_s(uint32_t diffCounts) {
    // Return speed in mRevolutions/s of steering motor
 
    // diffCounts of 0 is mapped to 0 speed
    if(diffCounts == 0){
        return 0;
    }

    int16_t direction = 1;
    if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_11) == GPIO_PIN_SET){
        direction = -1;
    }

    return direction * (int16_t)(CK_FREQ / TICKS_PER_REV / diffCounts);
}

void check_steer_speed_timeout() {
    uint32_t currSystick = systickCount;

    TIM2 -> DIER &= ~TIM_DIER_CC3IE;
    if (currSystick > (ch3SysCount + MIN_TIMEOUT)) {
        ch3Speed[ch3SpeedWr] = 0;
        ch3SpeedWr = (ch3SpeedWr + 1) % PING_PONG_SIZE;

        ch3SysCount = systickCount;
    }
    TIM2 -> DIER &= ~TIM_DIER_CC1IE;
}
