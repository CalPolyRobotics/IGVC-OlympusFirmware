/**
  ******************************************************************************
  * @file    Templates/Src/main.c 
  * @author  MCD Application Team
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2016 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice, *      this list of conditions and the following disclaimer in the documentation *      and/or other materials provided with the distribution.
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
#include "main.h"
#include "spi.h"

/** @addtogroup STM32F0xx_HAL_Examples
  * @{
  */

/** @addtogroup Templates
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void test_master_tx(void);
void test_slave_mode(void);

/* Private functions ---------------------------------------------------------*/
void test_master_tx(void){
    uint8_t reg = 0xAA;
    uint8_t val[4];
    GPIOA -> ODR &= ~GPIO_PIN_4;
    HAL_SPI_Transmit(&hspi1, &reg, 1, 1000);
    HAL_Delay(1);
    HAL_SPI_Receive(&hspi1, (uint8_t*)val, 4, 1000);
    GPIOA -> ODR |= GPIO_PIN_4;
}

void test_slave_mode(void){
    uint8_t reg; 
    uint8_t data[2] = {'a', 'b'};
    HAL_SPI_Receive(&hspi1, &reg, 1, 1000);
    if(reg == 0xAA){
        GPIOB -> ODR ^= GPIO_PIN_3;
        HAL_SPI_Transmit(&hspi1, (uint8_t*)data, 2, 1000);
    }
}

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{

    /* STM32F0xx HAL library initialization:
       - Configure the Flash prefetch
       - Systick timer is configured by default as source of time base, but user 
             can eventually implement his proper time base source (a general purpose 
             timer for example or other time source), keeping in mind that Time base 
             duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and 
             handled in milliseconds basis.
       - Low Level Initialization
     */
    HAL_Init();

    /* Configure the system clock to 48 MHz */
    SystemClock_Config();

    __HAL_RCC_GPIOB_CLK_ENABLE();

    //LED3_GPIO_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitTypeDef;

    GPIO_InitTypeDef.Pin = GPIO_PIN_3;
    GPIO_InitTypeDef.Pull = GPIO_NOPULL;
    GPIO_InitTypeDef.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitTypeDef.Speed = GPIO_SPEED_FREQ_LOW;

    HAL_GPIO_Init(GPIOB, &GPIO_InitTypeDef);

    //GPIOB -> BSRR |= GPIO_PIN_3;
    GPIOB -> ODR &= ~GPIO_PIN_3;

    MX_SPI1_Init();

    /* Infinite loop */
    /**
    uint8_t reg; 
    uint8_t data[4] = {'h', 'e', 'l', 'l'};
    while(1){
        HAL_SPI_Receive(&hspi1, &reg, 1, 1000);
        if(reg == 0xAA){
            GPIOB -> ODR |= GPIO_PIN_3;
            HAL_SPI_Transmit(&hspi1, (uint8_t*)&data[0], 4, 1000);
        }else{
            GPIOB -> ODR &= ~GPIO_PIN_3;
        }
    }
    **/
    init_spi_master();

    while (1)
    {
        test_master_tx();
    }
}

/**
* @brief  System Clock Configuration
*         The system Clock is configured as follow : 
*            System Clock source            = PLL (HSI)
*            SYSCLK(Hz)                     = 48000000
*            HCLK(Hz)                       = 48000000
*            AHB Prescaler                  = 1
*            APB1 Prescaler                 = 1
*            HSI Frequency(Hz)              = 8000000
*            PREDIV                         = 1
*            PLLMUL                         = 6
*            Flash Latency(WS)              = 1
* @param  None
* @retval None */
void SystemClock_Config(void)
{
    RCC_ClkInitTypeDef RCC_ClkInitStruct;
    RCC_OscInitTypeDef RCC_OscInitStruct;

    /* No HSE Oscillator on Nucleo, Activate PLL with HSI as source */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_NONE;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct)!= HAL_OK)
    { /* Initialization Error */
        while(1); 
    }

    /* Select PLL as system clock source and configure the HCLK, PCLK1 clocks dividers */
    RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1);
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1)!= HAL_OK)
    {
        /* Initialization Error */
        while(1); 
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

/**
* @}
*/

/**
* @}
*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
