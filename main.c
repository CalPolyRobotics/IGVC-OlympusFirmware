/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
  *
  * COPYRIGHT(c) 2016 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
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

#include <stddef.h>

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */


/* USER CODE END 0 */

int main(void)
{

  /* USER CODE BEGIN 1 */
  RCC->AHB1ENR |= 0xFFFFFFFF;
  RCC->AHB2ENR |= 0xFFFFFFFF;
  RCC->AHB3ENR |= 0xFFFFFFFF;
  RCC->APB1ENR |= 0xFFFFFFFF;
  RCC->APB2ENR |= 0xFFFFFFFF;
  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  // MX_ADC1_Init();
  MX_ADC2_Init();
  // MX_DAC_Init();
  MX_I2C2_Init();
  // MX_SPI3_Init();
  // MX_TIM1_Init();
  // MX_TIM2_Init();
  // MX_TIM3_Init();
  // MX_TIM4_Init();
  // MX_TIM5_Init();
  // MX_TIM8_Init();
  // MX_TIM9_Init();
  // MX_TIM10_Init();
  // MX_TIM12_Init();
  // MX_TIM13_Init();
  // MX_TIM14_Init();
  MX_USART1_UART_Init();
  MX_USB_OTG_FS_USB_Init();

  initSpeedDAC();

  adc_init();
  printf("Hello.\r\n");

  initSteeringMotor();
  //printf("Hello.\r\n");

  //i2cScan();
  //usbWrite((uint8_t*)"USBTest\r\n", 9);
  //printf("Test\r\n");
  //HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);

  /* USER CODE BEGIN 2 */
  //uart_relay();
  //while(1);

  startConversion();
  initIGVCCallbackTimer();
  initSteering();
  /* USER CODE END 2 */

  while(1)
  {
    consoleProcessBytes();
  }

  uint8_t buf[32];
  uint32_t bytesRead;

  while(1)
  {

    HAL_Delay(2000);

    bytesRead = usartRead(buf, 32);
    usartWrite((char*)buf, bytesRead);
  }

//-------------------------------------------------------------------
// This makes a fancy "loading" pattern on the 7segment display
  char sequence[] = {'A','B','G','E','D','C','G','F'};
  uint8_t ledSequence[] = {1,2,3,7,8,9,10,11,4,5,6};
  uint8_t i = 0;
  uint8_t j = 0;
  uint8_t k = 1;

  while(1)
  {
    if (i == 7)
    {
      i = 0;
    } else {
      i++;
    }
    
    if (j == 10)
    {
      j = 0;
    } else {
      j++;
    }

    if (k == 10)
    {
      k = 0;
    } else {
      k++;
    }


    setLED(ledSequence[j],0);
    setLED(ledSequence[k],1);
    setSevenSeg(sequence[i]);
    HAL_Delay(50);
      
  }
//-------------------------------------------------------------------


  while(1);

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    uint8_t data;
    if (!buffer8_empty(&usbRecieveBuffer))
    {
      //runCommsFSM(buffer8_get(&usbRecieveBuffer));
      data = buffer8_get(&usbRecieveBuffer);
      usbWrite(&data, 1);
    }
    //HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_4);
    //HAL_Delay(100);
  /* USER CODE END WHILE */
    //USBD_CDC_SetTxBuffer(&USBD_Device, "Hello\r\n", 7);
    //USBD_CDC_TransmitPacket(&USBD_Device);
    //HAL_Delay(100);
  /* USER CODE BEGIN 3 */

  }
  /* USER CODE END 3 */

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

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

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
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
