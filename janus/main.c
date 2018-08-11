#include "stm32f0xx_hal.h"
#include "spi.h"
#include "commsLib.h"
#include "i2c.h"
#include "fnr.h"
#include "gpio.h"
#include "sevenSeg.h"
#include "systemClock.h"

int main(void)
{
    HAL_StatusTypeDef status;
    HAL_Init();

    /* Configure the system clock to 48 MHz */
    SystemClock_Config();

    MX_SPI1_Init();
    MX_GPIO_Init();
    MX_I2C1_Init();

    fnr_t currentState = NEUTRAL;
    setSevenSegFNR(currentState);

    uint8_t data;
    while (1)
    {
        status = HAL_SPI_Receive(&hspi1, &data, 1, 100);
        if(status == HAL_OK){
            runCommsFSM(data);
        }

        fnr_t newState = getFNR();
        if(newState != currentState){
            currentState = newState;
            setSevenSegFNR(currentState);
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
