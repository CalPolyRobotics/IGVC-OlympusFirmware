#include "stm32f0xx_hal.h"
#include "spi.h"
#include "commsLib.h"
#include "systemClock.h"
#include "tinySpi.h"

int main(void)
{
    wrError_t status;
    HAL_Init();

    /* Configure the system clock to 48 MHz */
    SystemClock_Config();

    MX_SPI1_Init();

    GPIO_InitTypeDef gpioInit;
    __HAL_RCC_GPIOB_CLK_ENABLE();

    gpioInit.Pin = GPIO_PIN_3;
    gpioInit.Pull = GPIO_NOPULL;
    gpioInit.Mode = GPIO_MODE_OUTPUT_PP;
    gpioInit.Speed = GPIO_SPEED_FREQ_MEDIUM;
    HAL_GPIO_Init(GPIOB, &gpioInit);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET);

    uint8_t data;
    while (1)
    {
        status = readResponse(&data, 1);
        if(status == WR_OK){
            runCommsFSM(data);
        }
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
