#include "lights.h"
#include "main.h"

uint16_t arr[257]=
{24000,23906,23812,23718,23625,23531,23437,23343,
23250,23156,23062,22968,22875,22781,22687,22593,
22500,22406,22312,22218,22125,22031,21937,21843,
21750,21656,21562,21468,21375,21281,21187,21093,
21000,20906,20812,20718,20625,20531,20437,20343,
20250,20156,20062,19968,19875,19781,19687,19593,
19500,19406,19312,19218,19125,19031,18937,18843,
18750,18656,18562,18468,18375,18281,18187,18093,
18000,17906,17812,17718,17625,17531,17437,17343,
17250,17156,17062,16968,16875,16781,16687,16593,
16500,16406,16312,16218,16125,16031,15937,15843,
15750,15656,15562,15468,15375,15281,15187,15093,
15000,14906,14812,14718,14625,14531,14437,14343,
14250,14156,14062,13968,13875,13781,13687,13593,
13500,13406,13312,13218,13125,13031,12937,12843,
12750,12656,12562,12468,12375,12281,12187,12093,
12000,11906,11812,11718,11625,11531,11437,11343,
11250,11156,11062,10968,10875,10781,10687,10593,
10500,10406,10312,10218,10125,10031,9937,9843,
9750,9656,9562,9468,9375,9281,9187,9093,
9000,8906,8812,8718,8625,8531,8437,8343,
8250,8156,8062,7968,7875,7781,7687,7593,
7500,7406,7312,7218,7125,7031,6937,6843,
6750,6656,6562,6468,6375,6281,6187,6093,
6000,5906,5812,5718,5625,5531,5437,5343,
5250,5156,5062,4968,4875,4781,4687,4593,
4500,4406,4312,4218,4125,4031,3937,3843,
3750,3656,3562,3468,3375,3281,3187,3093,
3000,2906,2812,2718,2625,2531,2437,2343,
2250,2156,2062,1968,1875,1781,1687,1593,
1500,1406,1312,1218,1125,1031,937,843,
750,656,562,468,375,281,187,93,0};

void init_Timer(){
    __HAL_RCC_TIM3_CLK_ENABLE();
    TIM3 -> CCMR1 =(TIM_CCMR1_OC1M_1|TIM_CCMR1_OC1M_2|TIM_CCMR1_OC1M_0|TIM_CCMR1_OC2M_1|TIM_CCMR1_OC2M_2|TIM_CCMR1_OC2M_0);
    TIM3 -> CCMR2 =(TIM_CCMR1_OC1M_1|TIM_CCMR1_OC1M_2|TIM_CCMR1_OC1M_0|TIM_CCMR1_OC2M_1|TIM_CCMR1_OC2M_2|TIM_CCMR1_OC2M_0);
    TIM3 -> CCER=TIM_CCER_CC1E_Msk;
    TIM3 -> CR1 = TIM_CR1_CEN;
    TIM3 -> CCR1= 16000;
    TIM3 -> ARR= 24000;
    TIM3 -> CCR2= 32000;
    TIM3 -> CCR3= 8000;
    TIM3 -> CCR4= 4000;
    __HAL_RCC_GPIOA_CLK_ENABLE();
    GPIO_InitTypeDef GPIO_InitTypeDef;
    GPIO_InitTypeDef.Pin= TIMER_OUT;
    GPIO_InitTypeDef.Pull= GPIO_NOPULL;
    GPIO_InitTypeDef.Mode=GPIO_MODE_AF_PP;
    GPIO_InitTypeDef.Speed=GPIO_SPEED_FREQ_LOW;
    GPIO_InitTypeDef.Alternate=GPIO_AF1_TIM3;
    HAL_GPIO_Init( TIMER_PORT, &GPIO_InitTypeDef );


}

void init_apollo(){
    init_Timer(); 
    //Initializes pins 3 and 4 to outputs
    //below needs to modified if GPIOB isn't used
    __HAL_RCC_GPIOB_CLK_ENABLE();
    GPIO_InitTypeDef GPIO_InitTypeDef;

    GPIO_InitTypeDef.Pin= TURN_SIGNAL_L | TURN_SIGNAL_R | HEADLIGHTS;
    GPIO_InitTypeDef.Pull= GPIO_NOPULL;
    GPIO_InitTypeDef.Mode=GPIO_MODE_OUTPUT_PP;
    GPIO_InitTypeDef.Speed=GPIO_SPEED_FREQ_LOW;

    HAL_GPIO_Init( TURN_SIGNAL_PORT, &GPIO_InitTypeDef );

    //GPIOB ->BSRR |=GPIO_PIN_
    TURN_SIGNAL_PORT -> ODR |= TURN_SIGNAL_L | TURN_SIGNAL_R | HEADLIGHTS;


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
    TIM3 -> CCR1=arr[speed];    
}

void misc(uint8_t light, uint8_t state, uint8_t speed){

}

uint16_t get_light_sensor(){
    return 0;
}
