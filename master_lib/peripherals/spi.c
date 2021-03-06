/**
  ******************************************************************************
  * File Name          : SPI.c
  * Description        : This file provides code for the configuration
  *                      of the SPI instances.
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

#include "spi.h"
#include "gpio.h"
#include "config.h"

SPI_HandleTypeDef hspi3;

/* SPI1 init function */
void MX_SPI3_Init(void)
{
    hspi3.Instance = SPI3;
    hspi3.Init.Mode = SPI_MODE_MASTER;
    hspi3.Init.Direction = SPI_DIRECTION_2LINES;
    hspi3.Init.DataSize = SPI_DATASIZE_8BIT;
    hspi3.Init.CLKPolarity = SPI_POLARITY_LOW;
    hspi3.Init.CLKPhase = SPI_PHASE_1EDGE;
    hspi3.Init.NSS = SPI_NSS_SOFT;
    hspi3.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
    hspi3.Init.FirstBit = SPI_FIRSTBIT_MSB;
    hspi3.Init.TIMode = SPI_TIMODE_DISABLED;
    hspi3.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLED;
    hspi3.Init.CRCPolynomial = 10;
    HAL_SPI_Init(&hspi3);
}

void HAL_SPI_MspInit(SPI_HandleTypeDef* hspi)
{

    GPIO_InitTypeDef GPIO_InitStruct;
    if(hspi->Instance==SPI3)
    {
        SPI_CLOCKS_ENABLE();

        // SPI3 GPIO Configuration
        GPIO_InitStruct.Pin = SPI3_SCK | SPI3_MISO | SPI3_MOSI;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF6_SPI3;
        HAL_GPIO_Init(SPI3_PRT, &GPIO_InitStruct);

        // Interrupt  Pins
        GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;

        GPIO_InitStruct.Pin = APOL_IT_PIN;
        HAL_GPIO_Init(APOL_IT_PRT, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = HEPH_IT_PIN;
        HAL_GPIO_Init(HEPH_IT_PRT, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = HERM_IT_PIN;
        HAL_GPIO_Init(HERM_IT_PRT, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = JANU_IT_PIN;
        HAL_GPIO_Init(JANU_IT_PRT, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = ZEUS_IT_PIN;
        HAL_GPIO_Init(ZEUS_IT_PRT, &GPIO_InitStruct);

        // Slave Select Pins
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;

        GPIO_InitStruct.Pin = APOL_SS_PIN;
        HAL_GPIO_WritePin(APOL_SS_PRT, GPIO_InitStruct.Pin, GPIO_PIN_SET);
        HAL_GPIO_Init(APOL_SS_PRT, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = HEPH_SS_PIN;
        HAL_GPIO_WritePin(HEPH_SS_PRT, GPIO_InitStruct.Pin, GPIO_PIN_SET);
        HAL_GPIO_Init(HEPH_SS_PRT, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = HERM_SS_PIN;
        HAL_GPIO_WritePin(HERM_SS_PRT, GPIO_InitStruct.Pin, GPIO_PIN_SET);
        HAL_GPIO_Init(HERM_SS_PRT, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = JANU_SS_PIN;
        HAL_GPIO_WritePin(JANU_SS_PRT, GPIO_InitStruct.Pin, GPIO_PIN_SET);
        HAL_GPIO_Init(JANU_SS_PRT, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = ZEUS_SS_PIN;
        HAL_GPIO_WritePin(ZEUS_SS_PRT, GPIO_InitStruct.Pin, GPIO_PIN_SET);
        HAL_GPIO_Init(ZEUS_SS_PRT, &GPIO_InitStruct);
    }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
