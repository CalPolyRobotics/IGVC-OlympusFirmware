
#include "stm32f2xx_hal.h"
#include "pwradc.h"
#include "dac.h"
#include "dma.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "usb_otg.h"
#include "gpio.h"
#include "timerCallback.h"
#include "steering.h"
#include "main.h"
#include "buffer8.h"
#include "speedDAC.h"
#include "kill.h"
#include "sevenSeg.h"
#include "led.h"
#include "console.h"
#include "motorControl.h"
#include "adc.h"
#include "encoder.h"
#include "fnr.h"
#include "config.h"
#include "doubleBuffer.h"

#include <stddef.h>

void SystemClock_Config(void);

void togglePin(GPIO_TypeDef* gpio, uint32_t pin)
{
    gpio->ODR ^= pin;
}

Timer_Return led6Toggle(void* dummy)
{
    static uint32_t num = 0;
    //HAL_GPIO_TogglePin(GPIO_DEBUG_6);
    togglePin(GPIO_DEBUG_6);

    //setSevenSeg(num);
    num++;
    if (num > 15)
    {
        num = 0;
    }

    return CONTINUE_TIMER;
}

void zeusDataCallback(void* dummy, uint8_t* data, uint32_t len, I2CStatus status)
{
    if (status == I2C_ACK)
    {
        printf("Zeus Data: ");
        while (len--)
        {
            printf("%X ", *data++);
        }
        printf("\r\n");
    } else if (status == I2C_NACK) {
        printf("Zeus NACK\r\n");
    } else {
        printf("Zeus ERR\r\n");
    }
}


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
    initIGVCCallbackTimer();

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_DMA_Init();
    MX_ADC2_Init();
    i2cInit();

    commsUsartInit();
    MX_USB_OTG_FS_USB_Init();

    adc_init();

    initSteeringMotor();
    initSteering();
    initSpeedDAC();
    initEncoderInputCapture();
    initAutomanInt();

    addCallbackTimer(1000, led6Toggle, NULL);

    printf("Hello.\r\n");

    while(1)
    {
        serviceTxDma();
        serviceCallbackTimer();
        serviceUSBWrite();
        serviceI2C();
        serviceAutoman();

        consoleProcessBytes();

        uint8_t dataIn;

        while (doubleBuffer_read(&usbReceiveBuffer, &dataIn, 1))
        {
            runCommsFSM((char)dataIn);
        }
    }
}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct;
    RCC_ClkInitTypeDef RCC_ClkInitStruct;

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
    RCC_OscInitStruct.LSIState = RCC_LSI_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 16;
    RCC_OscInitStruct.PLL.PLLN = 192;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 5;
    HAL_RCC_OscConfig(&RCC_OscInitStruct);

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_PCLK1
                              |RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
    HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3);

    HAL_RCC_EnableCSS();

    HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
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

/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
