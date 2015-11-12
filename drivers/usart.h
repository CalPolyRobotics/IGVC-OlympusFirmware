#ifndef __USART_H__
#define __USART_H__

#include <stdint.h>

void initIGVCUsart();
void usartPut(uint8_t data);
void usartWrite(uint8_t* data, uint32_t size);
void usartPrint(char* data);
uint8_t usartGet();
void usartRead(uint8_t* buf, uint32_t bytes);
uint32_t usartHaveBytes();


#endif