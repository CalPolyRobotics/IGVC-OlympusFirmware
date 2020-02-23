//#define HAL_TIM_MODULE_ENABLED
#include "stm32f0xx_hal.h"
//#include "stm32f0xx_hal_dma.h"
#include "stm32f0xx_hal_tim.h"
//#include "stm32f0xx_hal_tim_ex.h"
#include "systemClock.h"
#include "boot.h"
#include "commsLib.h"
#include "spi.h"
#include "adc.h"
#include "tim.h"
#include "gpio.h"
#include "steer.h"
#include "steer_speed.h"

int main(void)
{
    remapVectorTable();

    HAL_StatusTypeDef status;
    HAL_Init();

    /* Configure the system clock to 48 MHz */
    SystemClock_Config();

    /* Enable interrupts */
    __enable_irq();

    MX_COMMS_SPI_Init();
    MX_GPIO_Init();
    //MX_TIM1_Init();
    MX_TIMER_1_INIT();
    MX_TIM2_Init();
    MX_ADC1_Init();
    //init_Timer();

    uint8_t data;

    //uint32_t DELAY_CNT = 100000;
    //uint8_t duty_cycle = 0;
    //uint8_t direction = 0;
    while (1)
    {

        /*
        for(int i = 0; i < DELAY_CNT; i++) {
            i++;
        }
        */
        
        /* // COOL PWM
        if(direction == 1) {
            duty_cycle = (duty_cycle + 1) % 101;
            if(duty_cycle >= 100) {
                direction = 0;
            }
        } else {
            duty_cycle = (duty_cycle - 1) % 101;
            if(duty_cycle == 0) {
                direction = 1;
            }
        }

        tim1_set_channel_duty(1, duty_cycle);
        tim1_set_channel_duty(2, 100 - duty_cycle);
        tim1_set_channel_duty(3, duty_cycle);
        tim1_set_channel_duty(4, duty_cycle);
        */

        status = readByte(&data);
        if(status == WR_OK){
            runCommsFSM(data);
        }

        //HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);
        //TIM1->CCR1 = 12000;
        //TIM1->CCR2 = 12000;
        //TIM1->CCR3 = 12000;
        //TIM1->CCR4 = 12000;

        update_steer_control_loop();
        //check_steer_speed_timeout();
    }
}



/*
int main(void)
{
    //remapVectorTable();

    //HAL_StatusTypeDef status;
    HAL_Init();

    // Configure the system clock to 48 MHz
    //SystemClock_Config();

    // Enable interrupts
    __enable_irq();


    //MX_COMMS_SPI_Init();
    //MX_GPIO_Init();
    //MX_TIM1_Init();
    //MX_TIM2_Init();
    //MX_ADC1_Init();
    MX_TIMER_1_INIT();

    //uint8_t data;
    while (1)
    {
        //status = readByte(&data);
        //if(status == WR_OK){
            //runCommsFSM(data);
        //}
        //HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);
        //TIM1->CCR1 = 12000;
        //TIM1->CCR2 = 12000;
        //TIM1->CCR3 = 12000;
        //TIM1->CCR4 = 12000;

        //update_steer_control_loop();
        //check_steer_speed_timeout();
    }
}
*/

#ifdef  USE_FULL_ASSERT

/**
* @brief  Reports the name of the source file and the source line number
*         where the assert_param error has occurred.
* @param  file: pointer to the source file name
* @param  line: assert_param error line source number
* @retval None
*/
void assert_failed(char* file, uint32_t line)
{ 
/* User can add his own implementation to report the file name and line number,
 ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

/* Infinite loop */
    while (1)
    {
    }
}
#endif
