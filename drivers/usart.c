
#include <string.h>

#include "main.h"

#include "utils/buffer8.h"


static uint8_t rxBuffer[64];
static uint8_t txBuffer[64];

static buffer8_t rxFifo;
static buffer8_t txFifo;

void initIGVCUsart()
{
  GPIO_InitTypeDef gpio;

  gpio.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
  gpio.GPIO_Mode = GPIO_Mode_AF;
  gpio.GPIO_Speed = GPIO_Speed_25MHz;
  gpio.GPIO_OType = GPIO_OType_PP;
  gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;

  GPIO_Init(GPIOB, &gpio);


  GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_USART1);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_USART1);

  USART_InitTypeDef usart;

  usart.USART_BaudRate = 115200;
  usart.USART_WordLength = USART_WordLength_8b;
  usart.USART_StopBits = USART_StopBits_1;
  usart.USART_Parity = USART_Parity_No;
  usart.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
  usart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;

  USART_Init(USART1, &usart);

  //USART1->BRR = (32 << 4)|(11);

  NVIC_InitTypeDef usartInt;
  usartInt.NVIC_IRQChannel = USART1_IRQn;
  usartInt.NVIC_IRQChannelPreemptionPriority = 0xF;; 
  usartInt.NVIC_IRQChannelSubPriority = 0xF; 

  //NVIC_Init(&usartInt);
  NVIC_SetPriority(USART1_IRQn, 2);
  NVIC_EnableIRQ(USART1_IRQn);

  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
  USART_Cmd(USART1, ENABLE);

  buffer8_init(&rxFifo, rxBuffer, 64);
  buffer8_init(&txFifo, txBuffer, 64);
}

void usartPut(uint8_t data)
{

   //while( USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
   //USART_SendData(USART1, data);

   //return;

   if(USART1->CR1 & USART_CR1_TXEIE)
   {
      buffer8_put(&txFifo, data);
   } else {
      USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
      USART1->DR = data;
   }
}

void usartWrite(uint8_t* data, uint32_t size)
{
   if (USART1->CR1 & USART_CR1_TXEIE)
   {
      buffer8_write(&txFifo, data, size);
   } else {
      buffer8_write(&txFifo, data + 1, size - 1);
      __disable_irq();
      USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
      USART1->DR = *data;
      __enable_irq();
   }
}

void usartPrint(char* data)
{
   usartWrite(data, strlen(data));
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

void USART1_IRQHandler()
{
   if (USART_GetITStatus(USART1, USART_IT_TXE) != RESET)
   {
      if (buffer8_empty(&txFifo))
      {
         USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
      } else {
         uint8_t data = buffer8_get(&txFifo);
         USART1->DR = data;
      }
      USART_ClearITPendingBit(USART1, USART_IT_TXE);
   } else if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
      buffer8_put(&rxFifo, USART1->DR);
   }
}

