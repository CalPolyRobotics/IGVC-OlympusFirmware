#include "stm32f0xx.h"

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
    /* No HSE Oscillator on Nucleo, Activate PLL with HSI as source */
    RCC->CFGR2 |= RCC_CFGR2_PREDIV_DIV1;
    RCC->CFGR |= RCC_CFGR_PLLSRC_HSI_PREDIV | RCC_CFGR_PLLMUL6;

    RCC->CR |= RCC_CR_PLLON;
    while(!(RCC->CR & RCC_CR_PLLRDY));

    /* Init HCLK */
    RCC->CFGR |= RCC_CFGR_HPRE_DIV1;

    /* Init System Clock to PLL **/
    RCC->CFGR |= RCC_CFGR_SW_PLL;
    while(!(RCC->CFGR & RCC_CFGR_SWS_PLL));

    /* Flash Wait State of 1 */
    FLASH->ACR = (FLASH->ACR&(~FLASH_ACR_LATENCY_Msk)) | FLASH_ACR_LATENCY;

    /* Set APB divider */
    RCC->CFGR = (RCC->CFGR&(~RCC_CFGR_PPRE_Msk)) | RCC_CFGR_PPRE_DIV1;

    /* Update clock speed LUT */
    SystemCoreClock = 48000000u;
}