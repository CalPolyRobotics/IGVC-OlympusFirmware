#include "stm32f2xx_hal.h"
#include "gpio.h"
#include "boot.h"
#include "doubleBuffer.h"
#include "usb_otg.h"
#include "flash.h"
#include "spi.h"

#include <stddef.h>
#include <stdio.h>

#include "usbd_def.h"

void SystemClock_Config(void);

int main(void)
{
    RCC->AHB1ENR |= 0xFFFFFFFF;
    RCC->AHB2ENR |= 0xFFFFFFFF;
    RCC->AHB3ENR |= 0xFFFFFFFF;
    RCC->APB1ENR |= 0xFFFFFFFF;
    RCC->APB2ENR |= 0xFFFFFFFF;

    /* MCU Configuration----------------------------------------------------------*/

    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* Configure the system clock */
    SystemClock_Config();

    /* Initialize all configured peripherals */
    MX_GPIO_Init();

    /*
     * Start Bootloader if Software Reset
     */
    if((RCC -> CSR & RCC_CSR_SFTRSTF) == 0 )
    {

        jumpToApp(USER_APP_BASE_PTR);
    }

    /* Clear Reset Flags for Detection on Startup */
    RCC -> CSR |= RCC_CSR_RMVF;

    MX_SPI3_Init();
    MX_USB_OTG_FS_USB_Init();

    uint32_u dataIn;
    uint32_t offset = 0;
    while(1)
    {
        offset += doubleBuffer_read(&usbReceiveBuffer, dataIn.u8 + offset, 4 - offset);
        if(offset == 4){
            runBootFSM(dataIn.u32);
            offset = 0;
        }
    }
}

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
    /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
}

#endif
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
