/**
  ******************************************************************************
  * File Name          : USART.c
  * Description        : This file provides code for the configuration
  *                      of the USART instances.
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

#include "main.h"
#include "config.h"
#include "stm32f2xx_hal_usart.h"

#include "usart.h"
#include "gpio.h"
#include "dma.h"
#include "utils/buffer8.h"

#include <stdio.h>

static uint8_t txBuffer[COMMS_TX_BUFFER_SIZE];
static uint8_t rxBuffer[COMMS_RX_BUFFER_SIZE];

static buffer8_t txFifo;
static buffer8_t rxFifo;

UART_HandleTypeDef huart1;
DMA_HandleTypeDef hdma_usart1_rx;

/* USART1 init function */

void MX_USART1_UART_Init(void)
{
    //For some reason printf starts out as line buffered.
    setbuf(stdout, NULL);

    huart1.Instance = COMMS_USART;
    huart1.Init.BaudRate = 115200;
    huart1.Init.WordLength = UART_WORDLENGTH_8B;
    huart1.Init.StopBits = UART_STOPBITS_1;
    huart1.Init.Parity = UART_PARITY_NONE;
    huart1.Init.Mode = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;
    HAL_UART_Init(&huart1);

    __USART_ENABLE(&huart1);

    buffer8_init(&txFifo, txBuffer, COMMS_TX_BUFFER_SIZE);
    buffer8_init(&rxFifo, rxBuffer, COMMS_RX_BUFFER_SIZE);

    __USART_ENABLE_IT(&huart1, USART_IT_RXNE);
}

void HAL_UART_MspInit(UART_HandleTypeDef* huart)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    if(huart->Instance==USART1)
    {
        /* Peripheral clock enable */
        __USART1_CLK_ENABLE();

        /**USART1 GPIO Configuration    
        PB6     ------> USART1_TX
        PB7     ------> USART1_RX 
        */
        GPIO_InitStruct.Pin = GPIO_USART_TX|GPIO_USART_RX;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
        GPIO_InitStruct.Alternate = GPIO_USART_AF;
        HAL_GPIO_Init(GPIO_USART_PORT, &GPIO_InitStruct);

        // /* Peripheral interrupt init*/
        NVIC_SetPriority(COMMS_IRQ, 2);
        NVIC_EnableIRQ(COMMS_IRQ);
    }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* huart)
{
    if(huart->Instance==USART1)
    {
        /* Peripheral clock disable */
        __USART1_CLK_DISABLE();

        /**USART1 GPIO Configuration    
        PB6     ------> USART1_TX
        PB7     ------> USART1_RX 
        */
        HAL_GPIO_DeInit(GPIO_USART_PORT, 
                        GPIO_USART_TX|GPIO_USART_RX);

        /* Peripheral interrupt Deinit*/
        HAL_NVIC_DisableIRQ(COMMS_IRQ);
    }
} 

void COMMS_ISR()
{
    if (__HAL_USART_GET_FLAG(&huart1, USART_FLAG_TXE) != RESET &&
        COMMS_USART->CR1 & USART_CR1_TXEIE)
    {
        if (buffer8_empty(&txFifo))
        {
            __USART_DISABLE_IT(&huart1, USART_IT_TXE);
        } else {
            uint8_t data = buffer8_get(&txFifo);
            COMMS_USART->DR = data;
        }

        __HAL_USART_CLEAR_FLAG(&huart1, USART_FLAG_TXE);
    }

    if (__HAL_USART_GET_FLAG(&huart1, USART_FLAG_RXNE) != RESET) {
        buffer8_put(&rxFifo, COMMS_USART->DR);
        __HAL_USART_CLEAR_FLAG(&huart1, USART_FLAG_RXNE);
    } 
}

void usartPut(uint8_t data)
{
    if(COMMS_USART->CR1 & USART_CR1_TXEIE)
    {
        buffer8_put(&txFifo, data);
    } else {
        __USART_ENABLE_IT(&huart1, USART_IT_TXE);
        COMMS_USART->DR = data;
    } 
}

void usartWrite(char* data, uint16_t size)
{
    if (size > 0)
    {
        if (COMMS_USART->CR1 & USART_CR1_TXEIE)
        {
            buffer8_write(&txFifo, (uint8_t*)data, size);
        } else {
            buffer8_write(&txFifo, (uint8_t*)data + 1, size - 1);
            __disable_irq();
            COMMS_USART->DR = *data;
            __USART_ENABLE_IT(&huart1, USART_IT_TXE);
            __enable_irq();
        }
    }
}

char usartGet()
{
    while (buffer8_empty(&rxFifo))
    ;

    return buffer8_get(&rxFifo);
}

uint32_t usartRead(uint8_t* data, uint32_t len)
{
    uint32_t bytesRead = 0;

    while(!buffer8_empty(&rxFifo) && bytesRead < len)
    {
        *data++ = buffer8_get(&rxFifo);
        bytesRead++;
    }

    return bytesRead;
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
