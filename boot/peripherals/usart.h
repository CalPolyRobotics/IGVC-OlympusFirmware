
#ifndef __usart_H
#define __usart_H

#include <stdint.h>

void commsUsartInit(void);

// Resets the Tx Dma for new transmission. If there is more data to send
// start a new transmission.
void serviceTxDma();

// Write a single byte into the Tx send buffer
void usartPut(uint8_t data);

// Write size bytes from data into the Tx send buffer
// If the Tx DMA is not sending data, enable the Tx DMA
// and send the buffer
void usartWrite(char *buf, uint16_t len);

// Read a single character from the USART
// Blocks until a character is received
char usartGet();

// Reads up to len bytes into data. Returns the number of actual bytes read.
// Does not block
uint32_t usartRead(uint8_t* data, uint32_t len);

#endif /*__ usart_H */
