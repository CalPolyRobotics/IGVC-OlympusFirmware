#include "stm32f0xx_hal.h"
#include "boot.h"
#include "spi.h"
#include "commsLib.h"
#include "lights.h"
#include "systemClock.h"

int main(void)
{
    remapVectorTable();

    //HAL_StatusTypeDef status;
    HAL_Init();


    /* Configure the system clock to 48 MHz */
    SystemClock_Config();

    /* Enable interrupts */
    __enable_irq();

    MX_COMMS_SPI_Init();

    init_apollo();

    //uint8_t data;
    while (1)
    {
        TIM1->CCR1 = 12000;
        TIM1->CCR2 = 12000;
        /*
        status = readByte(&data);
        if(status == WR_OK){
            runCommsFSM(data);
        }
        */
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
