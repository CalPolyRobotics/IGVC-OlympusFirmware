/**
  ******************************************************************************
  * File Name          : gpio.c
  * Description        : This file provides code for the configuration
  *                      of all used GPIO pins.
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

#include "gpio.h"
#include "kill.h"
#include "config.h"

void MX_GPIO_Init(void)
{

    GPIO_InitTypeDef GPIO_InitStruct;

    //Initiialize LEDS
    LED_CLOCKS_ENABLE();

    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_LOW;

    GPIO_InitStruct.Pin =  LED_1_PIN;
    HAL_GPIO_WritePin(LED_1_PRT, LED_1_PIN, GPIO_PIN_RESET);
    HAL_GPIO_Init(LED_1_PRT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin =  LED_2_PIN;
    HAL_GPIO_WritePin(LED_2_PRT, LED_2_PIN, GPIO_PIN_RESET);
    HAL_GPIO_Init(LED_2_PRT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin =  LED_3_PIN;
    HAL_GPIO_WritePin(LED_3_PRT, LED_3_PIN, GPIO_PIN_RESET);
    HAL_GPIO_Init(LED_3_PRT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin =  LED_4_PIN;
    HAL_GPIO_WritePin(LED_4_PRT, LED_4_PIN, GPIO_PIN_RESET);
    HAL_GPIO_Init(LED_4_PRT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin =  LED_5_PIN;
    HAL_GPIO_WritePin(LED_5_PRT, LED_5_PIN, GPIO_PIN_RESET);
    HAL_GPIO_Init(LED_5_PRT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin =  LED_6_PIN;
    HAL_GPIO_WritePin(LED_6_PRT, LED_6_PIN, GPIO_PIN_RESET);
    HAL_GPIO_Init(LED_6_PRT, &GPIO_InitStruct);

    // Initialize Communication Select
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_LOW;

    GPIO_InitStruct.Pin = COM_SEL_PIN;
    HAL_GPIO_Init(COM_SEL_PRT, &GPIO_InitStruct);
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
