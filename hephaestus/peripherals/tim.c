#include "tim.h"
#include "stm32f0xx_hal.h"

// TODO - Need to figure out the exact value for the proper drive frequency
#define ARR_20KHZ ((uint16_t)24000)
#define ARR_DEFAULT ARR_20KHZ

// NOTE, a higher value coreesonds with a lower duty cycle
uint16_t tim1_duty_lut[] = {
    (((uint32_t)ARR_DEFAULT * 100) / 100), (((uint32_t)ARR_DEFAULT * 99) / 100), 
    (((uint32_t)ARR_DEFAULT * 99) / 100), (((uint32_t)ARR_DEFAULT * 98) / 100), 
    (((uint32_t)ARR_DEFAULT * 98) / 100), (((uint32_t)ARR_DEFAULT * 97) / 100), 
    (((uint32_t)ARR_DEFAULT * 97) / 100), (((uint32_t)ARR_DEFAULT * 96) / 100), 
    (((uint32_t)ARR_DEFAULT * 96) / 100), (((uint32_t)ARR_DEFAULT * 95) / 100), 
    (((uint32_t)ARR_DEFAULT * 95) / 100), (((uint32_t)ARR_DEFAULT * 94) / 100), 
    (((uint32_t)ARR_DEFAULT * 94) / 100), (((uint32_t)ARR_DEFAULT * 93) / 100), 
    (((uint32_t)ARR_DEFAULT * 93) / 100), (((uint32_t)ARR_DEFAULT * 92) / 100), 
    (((uint32_t)ARR_DEFAULT * 92) / 100), (((uint32_t)ARR_DEFAULT * 91) / 100), 
    (((uint32_t)ARR_DEFAULT * 91) / 100), (((uint32_t)ARR_DEFAULT * 90) / 100), 
    (((uint32_t)ARR_DEFAULT * 90) / 100), (((uint32_t)ARR_DEFAULT * 89) / 100), 
    (((uint32_t)ARR_DEFAULT * 89) / 100), (((uint32_t)ARR_DEFAULT * 88) / 100), 
    (((uint32_t)ARR_DEFAULT * 88) / 100), (((uint32_t)ARR_DEFAULT * 87) / 100), 
    (((uint32_t)ARR_DEFAULT * 87) / 100), (((uint32_t)ARR_DEFAULT * 86) / 100), 
    (((uint32_t)ARR_DEFAULT * 86) / 100), (((uint32_t)ARR_DEFAULT * 85) / 100), 
    (((uint32_t)ARR_DEFAULT * 85) / 100), (((uint32_t)ARR_DEFAULT * 84) / 100), 
    (((uint32_t)ARR_DEFAULT * 84) / 100), (((uint32_t)ARR_DEFAULT * 83) / 100), 
    (((uint32_t)ARR_DEFAULT * 83) / 100), (((uint32_t)ARR_DEFAULT * 82) / 100), 
    (((uint32_t)ARR_DEFAULT * 82) / 100), (((uint32_t)ARR_DEFAULT * 81) / 100), 
    (((uint32_t)ARR_DEFAULT * 81) / 100), (((uint32_t)ARR_DEFAULT * 80) / 100), 
    (((uint32_t)ARR_DEFAULT * 80) / 100), (((uint32_t)ARR_DEFAULT * 79) / 100), 
    (((uint32_t)ARR_DEFAULT * 79) / 100), (((uint32_t)ARR_DEFAULT * 78) / 100), 
    (((uint32_t)ARR_DEFAULT * 78) / 100), (((uint32_t)ARR_DEFAULT * 77) / 100), 
    (((uint32_t)ARR_DEFAULT * 77) / 100), (((uint32_t)ARR_DEFAULT * 76) / 100), 
    (((uint32_t)ARR_DEFAULT * 76) / 100), (((uint32_t)ARR_DEFAULT * 75) / 100), 
    (((uint32_t)ARR_DEFAULT * 75) / 100), (((uint32_t)ARR_DEFAULT * 74) / 100), 
    (((uint32_t)ARR_DEFAULT * 74) / 100), (((uint32_t)ARR_DEFAULT * 73) / 100), 
    (((uint32_t)ARR_DEFAULT * 73) / 100), (((uint32_t)ARR_DEFAULT * 72) / 100), 
    (((uint32_t)ARR_DEFAULT * 72) / 100), (((uint32_t)ARR_DEFAULT * 71) / 100), 
    (((uint32_t)ARR_DEFAULT * 71) / 100), (((uint32_t)ARR_DEFAULT * 70) / 100), 
    (((uint32_t)ARR_DEFAULT * 70) / 100), (((uint32_t)ARR_DEFAULT * 69) / 100), 
    (((uint32_t)ARR_DEFAULT * 69) / 100), (((uint32_t)ARR_DEFAULT * 68) / 100), 
    (((uint32_t)ARR_DEFAULT * 68) / 100), (((uint32_t)ARR_DEFAULT * 67) / 100), 
    (((uint32_t)ARR_DEFAULT * 67) / 100), (((uint32_t)ARR_DEFAULT * 66) / 100), 
    (((uint32_t)ARR_DEFAULT * 66) / 100), (((uint32_t)ARR_DEFAULT * 65) / 100), 
    (((uint32_t)ARR_DEFAULT * 65) / 100), (((uint32_t)ARR_DEFAULT * 64) / 100), 
    (((uint32_t)ARR_DEFAULT * 64) / 100), (((uint32_t)ARR_DEFAULT * 63) / 100), 
    (((uint32_t)ARR_DEFAULT * 63) / 100), (((uint32_t)ARR_DEFAULT * 62) / 100), 
    (((uint32_t)ARR_DEFAULT * 62) / 100), (((uint32_t)ARR_DEFAULT * 61) / 100), 
    (((uint32_t)ARR_DEFAULT * 61) / 100), (((uint32_t)ARR_DEFAULT * 60) / 100), 
    (((uint32_t)ARR_DEFAULT * 60) / 100), (((uint32_t)ARR_DEFAULT * 59) / 100), 
    (((uint32_t)ARR_DEFAULT * 59) / 100), (((uint32_t)ARR_DEFAULT * 58) / 100), 
    (((uint32_t)ARR_DEFAULT * 58) / 100), (((uint32_t)ARR_DEFAULT * 57) / 100), 
    (((uint32_t)ARR_DEFAULT * 57) / 100), (((uint32_t)ARR_DEFAULT * 56) / 100), 
    (((uint32_t)ARR_DEFAULT * 56) / 100), (((uint32_t)ARR_DEFAULT * 55) / 100), 
    (((uint32_t)ARR_DEFAULT * 55) / 100), (((uint32_t)ARR_DEFAULT * 54) / 100), 
    (((uint32_t)ARR_DEFAULT * 54) / 100), (((uint32_t)ARR_DEFAULT * 53) / 100), 
    (((uint32_t)ARR_DEFAULT * 53) / 100), (((uint32_t)ARR_DEFAULT * 52) / 100), 
    (((uint32_t)ARR_DEFAULT * 52) / 100), (((uint32_t)ARR_DEFAULT * 51) / 100), 
    (((uint32_t)ARR_DEFAULT * 51) / 100), (((uint32_t)ARR_DEFAULT * 50) / 100), 
    (((uint32_t)ARR_DEFAULT * 50) / 100), (((uint32_t)ARR_DEFAULT * 49) / 100), 
    (((uint32_t)ARR_DEFAULT * 49) / 100), (((uint32_t)ARR_DEFAULT * 48) / 100), 
    (((uint32_t)ARR_DEFAULT * 48) / 100), (((uint32_t)ARR_DEFAULT * 47) / 100), 
    (((uint32_t)ARR_DEFAULT * 47) / 100), (((uint32_t)ARR_DEFAULT * 46) / 100), 
    (((uint32_t)ARR_DEFAULT * 46) / 100), (((uint32_t)ARR_DEFAULT * 45) / 100), 
    (((uint32_t)ARR_DEFAULT * 45) / 100), (((uint32_t)ARR_DEFAULT * 44) / 100), 
    (((uint32_t)ARR_DEFAULT * 44) / 100), (((uint32_t)ARR_DEFAULT * 43) / 100), 
    (((uint32_t)ARR_DEFAULT * 43) / 100), (((uint32_t)ARR_DEFAULT * 42) / 100), 
    (((uint32_t)ARR_DEFAULT * 42) / 100), (((uint32_t)ARR_DEFAULT * 41) / 100), 
    (((uint32_t)ARR_DEFAULT * 41) / 100), (((uint32_t)ARR_DEFAULT * 40) / 100), 
    (((uint32_t)ARR_DEFAULT * 40) / 100), (((uint32_t)ARR_DEFAULT * 39) / 100), 
    (((uint32_t)ARR_DEFAULT * 39) / 100), (((uint32_t)ARR_DEFAULT * 38) / 100), 
    (((uint32_t)ARR_DEFAULT * 38) / 100), (((uint32_t)ARR_DEFAULT * 37) / 100), 
    (((uint32_t)ARR_DEFAULT * 37) / 100), (((uint32_t)ARR_DEFAULT * 36) / 100), 
    (((uint32_t)ARR_DEFAULT * 36) / 100), (((uint32_t)ARR_DEFAULT * 35) / 100), 
    (((uint32_t)ARR_DEFAULT * 35) / 100), (((uint32_t)ARR_DEFAULT * 34) / 100), 
    (((uint32_t)ARR_DEFAULT * 34) / 100), (((uint32_t)ARR_DEFAULT * 33) / 100), 
    (((uint32_t)ARR_DEFAULT * 33) / 100), (((uint32_t)ARR_DEFAULT * 32) / 100), 
    (((uint32_t)ARR_DEFAULT * 32) / 100), (((uint32_t)ARR_DEFAULT * 31) / 100), 
    (((uint32_t)ARR_DEFAULT * 31) / 100), (((uint32_t)ARR_DEFAULT * 30) / 100), 
    (((uint32_t)ARR_DEFAULT * 30) / 100), (((uint32_t)ARR_DEFAULT * 29) / 100), 
    (((uint32_t)ARR_DEFAULT * 29) / 100), (((uint32_t)ARR_DEFAULT * 28) / 100), 
    (((uint32_t)ARR_DEFAULT * 28) / 100), (((uint32_t)ARR_DEFAULT * 27) / 100), 
    (((uint32_t)ARR_DEFAULT * 27) / 100), (((uint32_t)ARR_DEFAULT * 26) / 100), 
    (((uint32_t)ARR_DEFAULT * 26) / 100), (((uint32_t)ARR_DEFAULT * 25) / 100), 
    (((uint32_t)ARR_DEFAULT * 25) / 100), (((uint32_t)ARR_DEFAULT * 24) / 100), 
    (((uint32_t)ARR_DEFAULT * 24) / 100), (((uint32_t)ARR_DEFAULT * 23) / 100), 
    (((uint32_t)ARR_DEFAULT * 23) / 100), (((uint32_t)ARR_DEFAULT * 22) / 100), 
    (((uint32_t)ARR_DEFAULT * 22) / 100), (((uint32_t)ARR_DEFAULT * 21) / 100), 
    (((uint32_t)ARR_DEFAULT * 21) / 100), (((uint32_t)ARR_DEFAULT * 20) / 100), 
    (((uint32_t)ARR_DEFAULT * 20) / 100), (((uint32_t)ARR_DEFAULT * 19) / 100), 
    (((uint32_t)ARR_DEFAULT * 19) / 100), (((uint32_t)ARR_DEFAULT * 18) / 100), 
    (((uint32_t)ARR_DEFAULT * 18) / 100), (((uint32_t)ARR_DEFAULT * 17) / 100), 
    (((uint32_t)ARR_DEFAULT * 17) / 100), (((uint32_t)ARR_DEFAULT * 16) / 100), 
    (((uint32_t)ARR_DEFAULT * 16) / 100), (((uint32_t)ARR_DEFAULT * 15) / 100), 
    (((uint32_t)ARR_DEFAULT * 15) / 100), (((uint32_t)ARR_DEFAULT * 14) / 100), 
    (((uint32_t)ARR_DEFAULT * 14) / 100), (((uint32_t)ARR_DEFAULT * 13) / 100), 
    (((uint32_t)ARR_DEFAULT * 13) / 100), (((uint32_t)ARR_DEFAULT * 12) / 100), 
    (((uint32_t)ARR_DEFAULT * 12) / 100), (((uint32_t)ARR_DEFAULT * 11) / 100), 
    (((uint32_t)ARR_DEFAULT * 11) / 100), (((uint32_t)ARR_DEFAULT * 10) / 100), 
    (((uint32_t)ARR_DEFAULT * 10) / 100), (((uint32_t)ARR_DEFAULT * 9) / 100), 
    (((uint32_t)ARR_DEFAULT * 9) / 100), (((uint32_t)ARR_DEFAULT * 8) / 100), 
    (((uint32_t)ARR_DEFAULT * 8) / 100), (((uint32_t)ARR_DEFAULT * 7) / 100), 
    (((uint32_t)ARR_DEFAULT * 7) / 100), (((uint32_t)ARR_DEFAULT * 6) / 100), 
    (((uint32_t)ARR_DEFAULT * 6) / 100), (((uint32_t)ARR_DEFAULT * 5) / 100), 
    (((uint32_t)ARR_DEFAULT * 5) / 100), (((uint32_t)ARR_DEFAULT * 4) / 100), 
    (((uint32_t)ARR_DEFAULT * 4) / 100), (((uint32_t)ARR_DEFAULT * 3) / 100), 
    (((uint32_t)ARR_DEFAULT * 3) / 100), (((uint32_t)ARR_DEFAULT * 2) / 100), 
    (((uint32_t)ARR_DEFAULT * 2) / 100), (((uint32_t)ARR_DEFAULT * 1) / 100), 0
};

void MX_TIM1_Init()
{
    /*
     * Initialize Motor GPIOs
     * PA8  -> TIM1_CH1 -> BDRV
     * PA9  -> TIM1_CH2 -> ADRV
     * PA10 -> TIM1_CH3 -> BRK_BDRV
     * PA11 -> TIM1_CH4 -> BRK_ADRV
     */

    GPIO_InitTypeDef GPIO_InitStruct;

    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF2_TIM1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* 
     * Initialize Timer
     * Up counter PWM compare with CCR
     * A larger CCR value corresponds with a greater duty cycle
     *
     * Note:
     * ADRV and BDRV should not be driven at the same time. If one has a duty
     * cycle greater than 0, than the other should be set to a duty cycl of
     * zero. Changing ADRV or BDRV allows control over the direction of the
     * motor
     */
    __HAL_RCC_TIM1_CLK_ENABLE();

    /*
     * TIM1_CH1: Ouput Compare Mode 111
     * OUT = 0 TIMx_CNT < TIMx_CCR1 else 1 (when upcounting [default])
     * The same is applied to CH2, CH3, and CH4
     */
    TIM1->CCMR1 = TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_0 |
                  TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_0;

    TIM1->CCMR2 = TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3M_0 |
                  TIM_CCMR2_OC4M_2 | TIM_CCMR2_OC4M_1 | TIM_CCMR2_OC4M_0;

    // Enable compare output
    TIM1->CCER = TIM_CCER_CC1E | TIM_CCER_CC2E | TIM_CCER_CC3E | TIM_CCER_CC4E;

    // Enable timer
    TIM1->CR1 = TIM_CR1_CEN;

    TIM1->ARR = ARR_DEFAULT;

    // Initialize drive frequency
    TIM1->ARR = ARR_DEFAULT;

    // Set outputs to be 0% duty 
    TIM1->CCR1 = ARR_DEFAULT;
    TIM1->CCR2 = ARR_DEFAULT;
    TIM1->CCR3 = ARR_DEFAULT;
    TIM1->CCR4 = ARR_DEFAULT;

}

void tim1_set_channel_duty(uint8_t chn, uint8_t duty)
{
    if(duty > 100){
        return;
    }

    switch(chn){
        case 1:
            TIM1->CCR1 = tim1_duty_lut[duty];
            break;
        case 2:
            TIM1->CCR2 = tim1_duty_lut[duty];
            break;
        case 3:
            TIM1->CCR3 = tim1_duty_lut[duty];
            break;
        case 4:
            TIM1->CCR4 = tim1_duty_lut[duty];
            break;
        default:
            break;
    }
}

void MX_TIM2_Init()
{
    // Initialize Hall-effect GPIOs
    __HAL_RCC_GPIOB_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct;

    //PB10 -> STR_ENC A -> TIM2_CH3
    //PB11 -> STR_ENC B -> TIM2_CH4

    // Encoder Channel A
    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF2_TIM2;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    // Encoder Channel B
    GPIO_InitStruct.Pin = GPIO_PIN_11;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    // Initializes TIM2 settings
    __HAL_RCC_TIM2_CLK_ENABLE();

    // Timer input & input capture filter fdts/32 N=3
    TIM2->CCMR2 = TIM_CCMR2_CC3S_0 | TIM_CCMR2_IC3F_3 |TIM_CCMR2_IC3F_2 |
                  TIM_CCMR2_IC3F_1 | TIM_CCMR2_IC3F_0;

    TIM2->CCER = TIM_CCER_CC3E;
    TIM2->DIER = TIM_DIER_CC3IE;

    TIM2->ARR = 0xFFFFFFFF;

    NVIC_SetPriority(TIM2_IRQn, 0);
    NVIC_EnableIRQ(TIM2_IRQn);

    TIM2->CR1 = TIM_CR1_CEN;
}
