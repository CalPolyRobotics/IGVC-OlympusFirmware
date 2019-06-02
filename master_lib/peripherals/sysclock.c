#include "stm32f2xx_hal.h"

#ifdef DEV
    #define HSE_PLLM 8
    #define HSE_PLLN 240
#else
    #define HSE_PLLM 16
    #define HSE_PLLN 192
#endif

/** 
 * System Clock Configuration
 *
 * For Dev Board:
 *  Incoming 8 MHz OSC on HSE
 *
 * For Olympus:
 *  Incoming 20 MHz OSC on HSE
 *
 * Using PLL - Convert to:
 *  120 MHz System Clock
 *  120 MHz HCLK
 *  48 MHz PLL48 Clock
 *  30 MHz APB1 Clock
 *  60 MHz APB2 Clock
 */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct;
    RCC_ClkInitTypeDef RCC_ClkInitStruct;

    /* Enable HSE Oscillator and activate PLL with HSE as source */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;

    RCC_OscInitStruct.PLL.PLLM = HSE_PLLM;
    RCC_OscInitStruct.PLL.PLLN = HSE_PLLN;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 5;
    HAL_RCC_OscConfig(&RCC_OscInitStruct);

    /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
     clocks dividers */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK |
                            RCC_CLOCKTYPE_SYSCLK |
                            RCC_CLOCKTYPE_PCLK1|
                            RCC_CLOCKTYPE_PCLK2;

    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

    HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3);

    HAL_RCC_EnableCSS();

    HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

    HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}
