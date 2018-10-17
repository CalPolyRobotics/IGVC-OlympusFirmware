#include "stm32f0xx_hal.h"
#include "lights.h"

uint16_t arr[256]={
    24000,23905,23811,23717,23623,23529,23435,23341,
    23247,23152,23058,22964,22870,22776,22682,22588,
    22494,22400,22305,22211,22117,22023,21929,21835,
    21741,21647,21552,21458,21364,21270,21176,21082,
    20988,20894,20800,20705,20611,20517,20423,20329,
    20235,20141,20047,19952,19858,19764,19670,19576,
    19482,19388,19294,19200,19105,19011,18917,18823,
    18729,18635,18541,18447,18352,18258,18164,18070,
    17976,17882,17788,17694,17600,17505,17411,17317,
    17223,17129,17035,16941,16847,16752,16658,16564,
    16470,16376,16282,16188,16094,16000,15905,15811,
    15717,15623,15529,15435,15341,15247,15152,15058,
    14964,14870,14776,14682,14588,14494,14400,14305,
    14211,14117,14023,13929,13835,13741,13647,13552,
    13458,13364,13270,13176,13082,12988,12894,12800,
    12705,12611,12517,12423,12329,12235,12141,12047,
    11952,11858,11764,11670,11576,11482,11388,11294,
    11200,11105,11011,10917,10823,10729,10635,10541,
    10447,10352,10258,10164,10070,9976 ,9882 ,9788 ,
    9694 ,9600 ,9505 ,9411 ,9317 ,9223 ,9129 ,9035 ,
    8941 ,8847 ,8752 ,8658 ,8564 ,8470 ,8376 ,8282 ,
    8188 ,8094 ,8000 ,7905 ,7811 ,7717 ,7623 ,7529 ,
    7435 ,7341 ,7247 ,7152 ,7058 ,6964 ,6870 ,6776 ,
    6682 ,6588 ,6494 ,6400 ,6305 ,6211 ,6117 ,6023 ,
    5929 ,5835 ,5741 ,5647 ,5552 ,5458 ,5364 ,5270 ,
    5176 ,5082 ,4988 ,4894 ,4800 ,4705 ,4611 ,4517 ,
    4423 ,4329 ,4235 ,4141 ,4047 ,3952 ,3858 ,3764 ,
    3670 ,3576 ,3482 ,3388 ,3294 ,3200 ,3105 ,3011 ,
    2917 ,2823 ,2729 ,2635 ,2541 ,2447 ,2352 ,2258 ,
    2164 ,2070 ,1976 ,1882 ,1788 ,1694 ,1600 ,1505 ,
    1411 ,1317 ,1223 ,1129 ,1035 ,941  ,847  ,752  ,
    658  ,564  ,470  ,376  ,282  ,188  ,94   ,0   };

void init_Timer(){
    __HAL_RCC_TIM2_CLK_ENABLE();
    TIM2 -> CCMR2 = (TIM_CCMR2_OC3M_2|TIM_CCMR2_OC3M_1|TIM_CCMR2_OC3M_0);
    TIM2 -> CCMR1 = (TIM_CCMR1_OC2M_2|TIM_CCMR1_OC2M_1|TIM_CCMR1_OC2M_0);
    TIM2 -> CCER = (TIM_CCER_CC3E_Msk|TIM_CCER_CC2E_Msk);
    TIM2 -> CR1 = TIM_CR1_CEN;
    TIM2 -> ARR = 24000;
    TIM2 -> CCR2 = 24000;
    TIM2 -> CCR3 = 24000;
    __HAL_RCC_GPIOB_CLK_ENABLE();
    GPIO_InitTypeDef GPIO_InitTypeDef;
    GPIO_InitTypeDef.Pin = (HEADLIGHTS|MISC5);
    GPIO_InitTypeDef.Pull = GPIO_NOPULL;
    GPIO_InitTypeDef.Mode = GPIO_MODE_AF_PP;
    GPIO_InitTypeDef.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitTypeDef.Alternate = GPIO_AF2_TIM2;
    HAL_GPIO_Init( MISC_PORT, &GPIO_InitTypeDef );
    __HAL_RCC_GPIOA_CLK_ENABLE();
    GPIO_InitTypeDef.Pin = (HEADLIGHTS|MISC5);
    GPIO_InitTypeDef.Pull = GPIO_NOPULL;
    GPIO_InitTypeDef.Mode = GPIO_MODE_AF_PP;
    GPIO_InitTypeDef.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitTypeDef.Alternate = GPIO_AF2_TIM2;
    HAL_GPIO_Init( TURN_SIGNAL_PORT, &GPIO_InitTypeDef );

    __HAL_RCC_TIM16_CLK_ENABLE();                                               
    TIM16 -> CCMR1 = ( TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1| TIM_CCMR1_OC1M_0);
    TIM16 -> CCER = TIM_CCER_CC1E_Msk;
    TIM16 -> CR1 = TIM_CR1_CEN;
    TIM16 -> ARR = 24000;
    TIM16 -> CCR1 = 24000;
    GPIO_InitTypeDef.Pin = MISC2;
    GPIO_InitTypeDef.Pull = GPIO_NOPULL;
    GPIO_InitTypeDef.Mode = GPIO_MODE_AF_PP;
    GPIO_InitTypeDef.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitTypeDef.Alternate = GPIO_AF2_TIM16;
    HAL_GPIO_Init( MISC_PORT, &GPIO_InitTypeDef );

    __HAL_RCC_TIM3_CLK_ENABLE();
     TIM3 -> CCMR1 = ( TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1| TIM_CCMR1_OC1M_0|TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1| TIM_CCMR1_OC2M_0);
     TIM3 -> CCER = (TIM_CCER_CC2E_Msk|TIM_CCER_CC1E_Msk);
     TIM3 -> CR1 = TIM_CR1_CEN;
     TIM3 -> ARR = 24000;
     TIM3 -> CCR1 = 24000;
     TIM3 -> CCR2 = 24000;
     __HAL_RCC_GPIOB_CLK_ENABLE();  
     GPIO_InitTypeDef.Pin = ( MISC3| MISC4); 
     GPIO_InitTypeDef.Pull = GPIO_NOPULL;
     GPIO_InitTypeDef.Mode = GPIO_MODE_AF_PP;
     GPIO_InitTypeDef.Speed = GPIO_SPEED_FREQ_LOW;
     GPIO_InitTypeDef.Alternate = GPIO_AF1_TIM3; 
     HAL_GPIO_Init( MISC_PORT, &GPIO_InitTypeDef );
    
    __HAL_RCC_TIM1_CLK_ENABLE();
    TIM1 -> CCMR2 = (TIM_CCMR2_OC4M_2|TIM_CCMR2_OC4M_1|TIM_CCMR2_OC4M_0);     
    TIM1 -> CCMR1 = (TIM_CCMR1_OC1M_2|TIM_CCMR1_OC1M_1|TIM_CCMR1_OC1M_0);
    TIM1 -> CCER = (TIM_CCER_CC4E_Msk|TIM_CCER_CC1E_Msk);
    TIM1 -> CR1 = TIM_CR1_CEN;
    TIM1 -> ARR = 24000;
    TIM1 -> CCR4 = 24000;
    __HAL_RCC_GPIOA_CLK_ENABLE();
    GPIO_InitTypeDef.Pin = (MISC6|MISC1);
    GPIO_InitTypeDef.Pull = GPIO_NOPULL;
    GPIO_InitTypeDef.Mode = GPIO_MODE_AF_PP;
    GPIO_InitTypeDef.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitTypeDef.Alternate = GPIO_AF2_TIM1;
    HAL_GPIO_Init( TURN_SIGNAL_PORT, &GPIO_InitTypeDef );
    
    __HAL_RCC_GPIOA_CLK_ENABLE();
                                                            
    GPIO_InitTypeDef.Pin = TURN_SIGNAL_L | TURN_SIGNAL_R; 
    GPIO_InitTypeDef.Pull = GPIO_NOPULL;  
    GPIO_InitTypeDef.Mode = GPIO_MODE_OUTPUT_PP; 
    GPIO_InitTypeDef.Speed = GPIO_SPEED_FREQ_LOW; 
                                                            
    HAL_GPIO_Init( TURN_SIGNAL_PORT, &GPIO_InitTypeDef );
                                                            
    //GPIOA ->BSRR |=GPIO_PIN_
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
    TIM2 -> CCR3 = arr[speed];
}

void set_misc_lights(uint8_t light, uint8_t speed){
    if (light == 0){
    TIM17 -> CCR1 = arr[speed];    
    }
    
    if (light == 1){
    TIM16 -> CCR1 = arr[speed];
    }
    
    if (light == 2){
   TIM3 -> CCR2 = arr[speed]; 
    }
    
    if (light == 3){
    TIM3 -> CCR1 = arr[speed];
    }
    
    if (light == 4){
    TIM2 -> CCR2 = arr[speed];
    }
    
    if (light == 5){
    TIM1 -> CCR4 = arr[speed];
    }
}

uint16_t get_light_sensor(){
    return 0xF0F0;
}
