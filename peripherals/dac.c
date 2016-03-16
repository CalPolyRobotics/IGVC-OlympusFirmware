/**
  ******************************************************************************
  * File Name          : DAC.c
  * Description        : This file provides code for the configuration
  *                      of the DAC instances.
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

#include "dac.h"

#include "gpio.h"

DAC_HandleTypeDef hdac;

/* DAC init function */
void MX_DAC_Init(void)
{
    DAC_ChannelConfTypeDef sConfig;

    /**DAC Initialization 
    */
    hdac.Instance = DAC;
    HAL_DAC_Init(&hdac);

    /**DAC channel OUT1 config 
    */
    sConfig.DAC_Trigger = DAC_TRIGGER_NONE;
    sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
    HAL_DAC_ConfigChannel(&hdac, &sConfig, DAC_CHANNEL_1);

    /**DAC channel OUT2 config 
    */
    HAL_DAC_ConfigChannel(&hdac, &sConfig, DAC_CHANNEL_2);

}

void HAL_DAC_MspInit(DAC_HandleTypeDef* hdac)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    if(hdac->Instance==DAC)
    {
        /* Peripheral clock enable */
        __DAC_CLK_ENABLE();

        /**DAC GPIO Configuration    
        PA4     ------> DAC_OUT1
        PA5     ------> DAC_OUT2 
        */
        GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5;
        GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    }
}

void HAL_DAC_MspDeInit(DAC_HandleTypeDef* hdac)
{
    if(hdac->Instance==DAC)
    {
        /* Peripheral clock disable */
        __DAC_CLK_DISABLE();

        /**DAC GPIO Configuration    
        PA4     ------> DAC_OUT1
        PA5     ------> DAC_OUT2 
        */
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_4|GPIO_PIN_5);
    }
} 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
