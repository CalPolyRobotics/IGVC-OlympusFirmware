#include "stm32f0xx_hal.h"
#include "adc.h"
#include "boot.h"
#include "commsLib.h"
#include "spi.h"
#include "systemClock.h"
#include "gpio.h"
#include "tim.h"
#include "speed.h"

int main(void)
{
    remapVectorTable();

    HAL_StatusTypeDef status;
    HAL_Init();

    /* Configure the system clock to 48 MHz */
    SystemClock_Config();

    /* Enable interrupts */
    __enable_irq();

    MX_ADC1_Init();
    MX_GPIO_Init();
    MX_TIM_Init();
    MX_COMMS_SPI_Init();

    uint8_t data;
    while (1)
    {
        status = readByte(&data);
        if(status == WR_OK){
            runCommsFSM(data);
        }
        checkTimeout();
    }
}

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
