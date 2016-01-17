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

/* Includes ------------------------------------------------------------------*/
#include "usart.h"

#include "gpio.h"
#include "dma.h"

/* USER CODE BEGIN 0 */
#define UART_TIMEOUT  10000
/* USER CODE END 0 */

UART_HandleTypeDef huart3;
DMA_HandleTypeDef hdma_usart3_rx;

/* USART3 init function */

void MX_USART3_UART_Init(void)
{

  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  HAL_UART_Init(&huart3);

}

void HAL_UART_MspInit(UART_HandleTypeDef* huart)
{

  GPIO_InitTypeDef GPIO_InitStruct;
  if(huart->Instance==USART3)
  {
  /* USER CODE BEGIN USART3_MspInit 0 */

  /* USER CODE END USART3_MspInit 0 */
    /* Peripheral clock enable */
    __USART3_CLK_ENABLE();
  
    /**USART3 GPIO Configuration    
    PC10     ------> USART3_TX
    PC11     ------> USART3_RX 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART3;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    /* Peripheral DMA init*/
  
    hdma_usart3_rx.Instance = DMA1_Stream1;
    hdma_usart3_rx.Init.Channel = DMA_CHANNEL_4;
    hdma_usart3_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_usart3_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart3_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart3_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart3_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart3_rx.Init.Mode = DMA_NORMAL;
    hdma_usart3_rx.Init.Priority = DMA_PRIORITY_HIGH;
    hdma_usart3_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    hdma_usart3_rx.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
    hdma_usart3_rx.Init.MemBurst = DMA_MBURST_SINGLE;
    hdma_usart3_rx.Init.PeriphBurst = DMA_PBURST_SINGLE;
    HAL_DMA_Init(&hdma_usart3_rx);

    __HAL_LINKDMA(huart,hdmarx,hdma_usart3_rx);

    /* Peripheral interrupt init*/
    HAL_NVIC_SetPriority(USART3_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART3_IRQn);
  /* USER CODE BEGIN USART3_MspInit 1 */

  /* USER CODE END USART3_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* huart)
{

  if(huart->Instance==USART3)
  {
  /* USER CODE BEGIN USART3_MspDeInit 0 */

  /* USER CODE END USART3_MspDeInit 0 */
    /* Peripheral clock disable */
    __USART3_CLK_DISABLE();
  
    /**USART3 GPIO Configuration    
    PC10     ------> USART3_TX
    PC11     ------> USART3_RX 
    */
    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_10|GPIO_PIN_11);

    /* Peripheral DMA DeInit*/
    HAL_DMA_DeInit(huart->hdmarx);

    /* Peripheral interrupt Deinit*/
    HAL_NVIC_DisableIRQ(USART3_IRQn);

  }
  /* USER CODE BEGIN USART3_MspDeInit 1 */

  /* USER CODE END USART3_MspDeInit 1 */
} 

/* USER CODE BEGIN 1 */
char dmaData[8];
volatile int dmaDone;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *husart)
{
  //usartWrite("Read: ", 5);
  usartWrite(dmaData, 8);
  //usartWrite("\r\n", 2);
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *husart)
{
  dmaDone = 1;
}

/* USER CODE BEGIN 1 */

void usartWrite(uint8_t *buf, uint16_t len)
{
  uart_transmit(buf, len);
}

void uart_transmit(uint8_t *buf, uint16_t len)
{
  //HAL_UART_Transmit(&huart3, buf, len, UART_TIMEOUT);
  HAL_UART_Transmit_IT(&huart3, buf, len);
}

void uart_relay()
{
  while(1)
  {
    dmaData[0] = 0;
    dmaData[1] = 0;
    dmaData[2] = 0;
    dmaData[3] = 0;
    dmaData[4] = 0;
    dmaData[5] = 0;
    dmaData[6] = 0;
    dmaData[7] = 0;
    dmaData[8] = 0;

    dmaDone = 0;

    HAL_UART_Receive_DMA(&huart3, dmaData, 8);

    while(!dmaDone)
      ;
  }
}

void uart_receive(uint8_t *buf, uint16_t len)
{
  do
  {
    HAL_UART_Receive(&huart3, buf, len, UART_TIMEOUT);
  } while (HAL_UART_GetState(&huart3) == 0x03);
}

/* USER CODE END 1 */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
