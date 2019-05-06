#include "stm32f0xx.h"

#define SYSTEM_CORE_CLOCK 48000000u

volatile uint32_t systickCount = 0;

static void SysTick_Init(uint32_t tickNums);

/**
* @brief  System Clock Configuration
*         The system Clock is configured as follow : 
*            System Clock source            = PLL (HSI)
*            SYSCLK(Hz)                     = 48000000
*            HCLK(Hz)                       = 48000000
*            AHB Prescaler                  = 1
*            APB1 Prescaler                 = 1
*            HSI Frequency(Hz)              = 8000000 *            PREDIV                         = 1
*            PLLMUL                         = 6
*            Flash Latency(WS)              = 1
* @param  None
* @retval None */
void SystemClock_Config()
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

    /* Enable 1ms systick clock */
    SysTick_Init(SYSTEM_CORE_CLOCK/1000u);
}

void SysTick_Delay(int ms)
{
    uint32_t finish = systickCount + ms;
    while(systickCount < finish);
}

void SysTick_Handler()
{
    systickCount++;
}

static void SysTick_Init(uint32_t tickNums)
{
    SysTick->LOAD = tickNums - 1u;
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk |
                    SysTick_CTRL_TICKINT_Msk |
                    SysTick_CTRL_ENABLE_Msk;

     NVIC->ISER[0U] = (uint32_t)(1UL << (((uint32_t)(int32_t)SysTick_IRQn) & 0x1FUL));
}
