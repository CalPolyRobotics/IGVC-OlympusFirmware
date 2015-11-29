
#include <string.h>

#include "main.h"
#include "config.h"

#include "buffer8.h"
#include "gpio.h"


static uint8_t rxBuffer[COMMS_RX_FIFO_SIZE];
static uint8_t txBuffer[COMMS_TX_FIFO_SIZE];

static buffer8_t rxFifo;
static buffer8_t txFifo;
static USART_HandleTypeDef usart;

void initIGVCUsart()
{

  usart.Instance = COMMS_USART;

  usart.Init.BaudRate = COMMS_BAUDRATE;
  usart.Init.WordLength = USART_WORDLENGTH_8B;
  usart.Init.StopBits = USART_STOPBITS_1;
  usart.Init.Parity = USART_PARITY_NONE;
  usart.Init.Mode = USART_MODE_TX_RX;

  usart.Lock = HAL_UNLOCKED;
  usart.State = HAL_USART_STATE_RESET;

  HAL_USART_Init(&usart);

  NVIC_SetPriority(COMMS_IRQ, 2);
  NVIC_EnableIRQ(COMMS_IRQ);

  __HAL_USART_ENABLE_IT(&usart, USART_IT_RXNE);

  //USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
  //USART_Cmd(USART1, ENABLE);


  buffer8_init(&rxFifo, rxBuffer, COMMS_RX_FIFO_SIZE);
  buffer8_init(&txFifo, txBuffer, COMMS_TX_FIFO_SIZE);
}

void HAL_USART_MspInit(USART_HandleTypeDef* husart)
{
  /*GPIO_InitTypeDef gpio;

  gpio.Pin = GPIO_PIN_6 | GPIO_PIN_7;
  gpio.Mode = GPIO_MODE_AF_PP;
  gpio.Pull = GPIO_NOPULL;
  gpio.Speed = GPIO_SPEED_FREQ_HIGH;
  gpio.Alternate = GPIO_AF7_USART1;

  HAL_GPIO_Init(GPIOB, &gpio);*/

  initGPIOAlternate_PP(GPIO_USART_TX, USART_TX_AF);
  initGPIOAlternate_PP(GPIO_USART_RX, USART_RX_AF);
}

void usartPut(uint8_t data)
{
   if(COMMS_USART->CR1 & USART_CR1_TXEIE)
   {
      buffer8_put(&txFifo, data);
   } else {
      //USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
      __HAL_USART_ENABLE_IT(&usart, USART_IT_TXE);
      COMMS_USART->DR = data;
   }
}

void usartWrite(uint8_t* data, uint32_t size)
{
   if (COMMS_USART->CR1 & USART_CR1_TXEIE)
   {
      buffer8_write(&txFifo, data, size);
   } else {
      buffer8_write(&txFifo, data + 1, size - 1);
      __disable_irq();
      //USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
      __HAL_USART_ENABLE_IT(&usart, USART_IT_TXE);
      COMMS_USART->DR = *data;
      __enable_irq();
   }
}

void usartPrint(char* data)
{
   usartWrite((uint8_t*)data, strlen(data));
}

uint8_t usartGet()
{
   return buffer8_get(&rxFifo);
}

void usartRead(uint8_t* buf, uint32_t bytes)
{
   while (bytes--)
   {
      *buf++ = usartGet();
   }
}

uint32_t usartHaveBytes()
{
   return buffer8_bytes(&rxFifo);
}

void COMMS_ISR()
{
   //if (USART_GetITStatus(USART1, USART_IT_TXE) != RESET)
   if (__HAL_USART_GET_FLAG(&usart, USART_FLAG_TXE) != RESET)
   {
      if (buffer8_empty(&txFifo))
      {
        //USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
        __HAL_USART_DISABLE_IT(&usart, USART_IT_TXE);
      } else {
         uint8_t data = buffer8_get(&txFifo);
         COMMS_USART->DR = data;
      }
      //USART_ClearITPendingBit(USART1, USART_IT_TXE);
      __HAL_USART_CLEAR_FLAG(&usart, USART_FLAG_TXE);
   //} else if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
    } else if (__HAL_USART_GET_FLAG(&usart, USART_FLAG_RXNE) != RESET) {
      buffer8_put(&rxFifo, COMMS_USART->DR);
    }
}

