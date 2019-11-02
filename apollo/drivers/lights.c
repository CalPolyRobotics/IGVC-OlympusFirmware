#include "stm32f0xx_hal.h"
#include "lights.h"

//Look up table for PWMing the lights
uint16_t arr[256] = {
    0,     94,    188,   282,   376,   470,   564,   658,
    752,   847,   941,   1035,  1129,  1223,  1317,  1411,
    1505,  1600,  1694,  1788,  1882,  1976,  2070,  2164,
    2258,  2352,  2447,  2541,  2635,  2729,  2823,  2917,
    3011,  3105,  3200,  3294,  3388,  3482,  3576,  3670,
    3764,  3858,  3952,  4047,  4141,  4235,  4329,  4423,
    4517,  4611,  4705,  4800,  4894,  4988,  5082,  5176,
    5270,  5364,  5458,  5552,  5647,  5741,  5835,  5929,
    6023,  6117,  6211,  6305,  6400,  6494,  6588,  6682,
    6776,  6870,  6964,  7058,  7152,  7247,  7341,  7435,
    7529,  7623,  7717,  7811,  7905,  8000,  8094,  8188,
    8282,  8376,  8470,  8564,  8658,  8752,  8847,  8941,
    9035,  9129,  9223,  9317,  9411,  9505,  9600,  9694,
    9788,  9882,  9976,  10070, 10164, 10258, 10352, 10447,
    10541, 10635, 10729, 10823, 10917, 11011, 11105, 11200,
    11294, 11388, 11482, 11576, 11670, 11764, 11858, 11952,
    12047, 12141, 12235, 12329, 12423, 12517, 12611, 12705,
    12800, 12894, 12988, 13082, 13176, 13270, 13364, 13458,
    13552, 13647, 13741, 13835, 13929, 14023, 14117, 14211,
    14305, 14400, 14494, 14588, 14682, 14776, 14870, 14964,
    15058, 15152, 15247, 15341, 15435, 15529, 15623, 15717,
    15811, 15905, 16000, 16094, 16188, 16282, 16376, 16470,
    16564, 16658, 16752, 16847, 16941, 17035, 17129, 17223,
    17317, 17411, 17505, 17600, 17694, 17788, 17882, 17976,
    18070, 18164, 18258, 18352, 18447, 18541, 18635, 18729,
    18823, 18917, 19011, 19105, 19200, 19294, 19388, 19482,
    19576, 19670, 19764, 19858, 19952, 20047, 20141, 20235,
    20329, 20423, 20517, 20611, 20705, 20800, 20894, 20988,
    21082, 21176, 21270, 21364, 21458, 21552, 21647, 21741,
    21835, 21929, 22023, 22117, 22211, 22305, 22400, 22494,
    22588, 22682, 22776, 22870, 22964, 23058, 23152, 23247,
    23341, 23435, 23529, 23623, 23717, 23811, 23905, 24000
}

void init_Timer(){

    /* enable GPIOA and GPIOB clocks */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    /* enable TIM2 clock */
    __HAL_RCC_TIM2_CLK_ENABLE();
    /* set output compare modes for compare 1, 2, and 3 to '111' (PWM mode 2) */
    TIM2 -> CCMR1 = (TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_0) |
                    (TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_0);
    TIM2 -> CCMR2 = (TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3M_0);
    /* enable capture/compare output for compare 1, 2, and 3 */
    TIM2 -> CCER = (TIM_CCER_CC3E_Msk | TIM_CCER_CC2E_Msk | TIM_CCER_CC1E_Msk);
    /* enable counter */
    TIM2 -> CR1 = TIM_CR1_CEN;
    /* set auto-reload to 24000 (max. PWM value) */
    TIM2 -> ARR = 24000;
    /* initialize capture/compare registers for channel 1 and 2 to 24000 */
    TIM2 -> CCR1 = 24000;
    TIM2 -> CCR2 = 24000;

    /* enable TIM3 clock */
    __HAL_RCC_TIM3_CLK_ENABLE();
    /* set output compare modes for compare 1 and 2 to '111' (PWM mode 2) */
    TIM3 -> CCMR1 = (TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_0) |
                    (TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_0);
    /* enable capture/compare output for compare 1 and 2 */
    TIM3 -> CCER = (TIM_CCER_CC2E_Msk | TIM_CCER_CC1E_Msk);
    /* enable counter */
    TIM3 -> CR1 = TIM_CR1_CEN;
    /* set auto-reload to 24000 (max. PWM value) */
    TIM3 -> ARR = 24000;
    /* initialize capture/compare registers for channel 1 and 2 to 24000 */
    TIM3 -> CCR1 = 24000;
    TIM3 -> CCR2 = 24000;

    /* enable TIM16 clock */
    __HAL_RCC_TIM16_CLK_ENABLE();
    /* set output compare mode for compare 1 to '111' (PWM mode 2) */
    TIM16 -> CCMR1 = (TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_0);
    /* enable main output in break and dead-time register */
    TIM16 -> BDTR = TIM_BDTR_MOE_Msk;
    /* enable capture/compare and complementary output for compare 1 */
    TIM16 -> CCER = TIM_CCER_CC1E_Msk | TIM_CCER_CC1NE_Msk;
    /* enable counter */
    TIM16 -> CR1 = TIM_CR1_CEN;
    /* set auto-reload to 24000 (max. PWM value) */
    TIM16 -> ARR = 24000;
    /* initialize capture/compare registers for channel 1 to 24000 */
    TIM16 -> CCR1 = 24000;

    /* enable TIM17 clock */
    __HAL_RCC_TIM17_CLK_ENABLE();
    /* set output compare mode for compare 1 to '111' (PWM mode 2) */
    TIM17 -> CCMR1 = (TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_0);
    /* enable main output in break and dead-time register */
    TIM17 -> BDTR = TIM_BDTR_MOE_Msk;
    /* enable capture/compare and complementary output for compare 1 */
    TIM17 -> CCER = TIM_CCER_CC1E_Msk | TIM_CCER_CC1NE_Msk;
    /* enable counter */
    TIM17 -> CR1 = TIM_CR1_CEN;
    /* set auto-reload to 24000 (max. PWM value) */
    TIM17 -> ARR = 24000;
    /* initialize capture/compare registers for channel 1 to 24000 */
    TIM17 -> CCR1 = 24000;

    GPIO_InitTypeDef GPIO_InitTypeDef;

    /* configure GPIO for HEADLIGHTS */
    GPIO_InitTypeDef.Pin = HEADLIGHTS;
    GPIO_InitTypeDef.Pull = GPIO_NOPULL;
    GPIO_InitTypeDef.Mode = GPIO_MODE_AF_PP;
    GPIO_InitTypeDef.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitTypeDef.Alternate = GPIO_AF2_TIM2;
    HAL_GPIO_Init( TURN_SIGNAL_PORT, &GPIO_InitTypeDef );

    /* configure GPIO for MISC1 */
    GPIO_InitTypeDef.Pin = MISC1;
    GPIO_InitTypeDef.Pull = GPIO_NOPULL;
    GPIO_InitTypeDef.Mode = GPIO_MODE_AF_PP;
    GPIO_InitTypeDef.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitTypeDef.Alternate = GPIO_AF2_TIM2;
    HAL_GPIO_Init( TURN_SIGNAL_PORT, &GPIO_InitTypeDef );

    /* configure GPIO for MISC2 */
    GPIO_InitTypeDef.Pin = MISC2;
    GPIO_InitTypeDef.Pull = GPIO_NOPULL;
    GPIO_InitTypeDef.Mode = GPIO_MODE_AF_PP;
    GPIO_InitTypeDef.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitTypeDef.Alternate = GPIO_AF2_TIM2;
    HAL_GPIO_Init( MISC_PORT, &GPIO_InitTypeDef );

    /* configure GPIO for MISC3 and 4 */
    GPIO_InitTypeDef.Pin = (MISC3 | MISC4);
    GPIO_InitTypeDef.Pull = GPIO_NOPULL;
    GPIO_InitTypeDef.Mode = GPIO_MODE_AF_PP;
    GPIO_InitTypeDef.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitTypeDef.Alternate = GPIO_AF1_TIM3;
    HAL_GPIO_Init( MISC_PORT, &GPIO_InitTypeDef );

    /* configure GPIO for MISC5 */
    GPIO_InitTypeDef.Pin = MISC5;
    GPIO_InitTypeDef.Pull = GPIO_NOPULL;
    GPIO_InitTypeDef.Mode = GPIO_MODE_AF_PP;
    GPIO_InitTypeDef.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitTypeDef.Alternate = GPIO_AF2_TIM16;
    HAL_GPIO_Init( MISC_PORT, &GPIO_InitTypeDef );

    /* configure GPIO for MISC6 */
    GPIO_InitTypeDef.Pin = MISC6;
    GPIO_InitTypeDef.Pull = GPIO_NOPULL;
    GPIO_InitTypeDef.Mode = GPIO_MODE_AF_PP;
    GPIO_InitTypeDef.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitTypeDef.Alternate = GPIO_AF2_TIM17;
    HAL_GPIO_Init( MISC_PORT, &GPIO_InitTypeDef );

    /* configure GPIO for turn signals */
    GPIO_InitTypeDef.Pin = (TURN_SIGNAL_L | TURN_SIGNAL_R);
    GPIO_InitTypeDef.Pull = GPIO_NOPULL;
    GPIO_InitTypeDef.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitTypeDef.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init( TURN_SIGNAL_PORT, &GPIO_InitTypeDef );
    //Start with turn signals off
    TURN_SIGNAL_PORT -> ODR &= ~(TURN_SIGNAL_L | TURN_SIGNAL_R);

}

void init_apollo(){
    init_Timer();
}

void set_turn_signal(uint8_t state){
    if (state & STATE_R_MASK){
        TURN_SIGNAL_PORT -> ODR |= TURN_SIGNAL_R;
    }
    else {
        TURN_SIGNAL_PORT -> ODR &= ~TURN_SIGNAL_R;
    }
    if (state & STATE_L_MASK){
        TURN_SIGNAL_PORT -> ODR |= TURN_SIGNAL_L;
    }
    else {
        TURN_SIGNAL_PORT -> ODR &= ~TURN_SIGNAL_L;
    }
}

void set_headlights( uint8_t speed){
    TIM2 -> CCR1 = arr[speed];
}

void set_misc_lights(uint8_t light, uint8_t speed){
    if (light == 0){
        TIM2 -> CCR1 = arr[speed];
    }

    if (light == 1){
        TIM2 -> CCR2 = arr[speed];
    }

    if (light == 2){
        TIM3 -> CCR1 = arr[speed];
    }

    if (light == 3){
        TIM3 -> CCR2 = arr[speed];
    }

    if (light == 4){
        TIM16 -> CCR1 = arr[255 - speed]; /* inverted to correct PWM */
    }

    if (light == 5){
        TIM17 -> CCR1 = arr[255 - speed]; /* inverted to correct PWM */
    }
}

uint16_t get_light_sensor(){
    return 0xF0F0;
}
