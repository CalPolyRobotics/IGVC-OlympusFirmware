#ifndef __USART_H__
#define __USART_H__

#include <stdint.h>

// void initIGVCUsart()
// Initializes the USART peripheral and the pins required for it
void initIGVCUsart();

// void usartPut(uint8_t data)
// Outputs one character using the USART peripheral. The peripheral
// must have been initialized with initIGVCUsart prior to this call.
// This function relies on the USART TX interrupt and is non-blocking.
// Arguments:
//   uint8_t data: The character to print.
void usartPut(uint8_t data);

// void usartWrite(uint8_t* data, uint32_t size)
// Writes a number of characters out the USART peripheral. The peripheral
// must have been initialized with initIGVCUsart prior to this call.
// This function relies on the USART TX interrupt and is non-blocking.
// Arguments:
//   uint8_t* data: Pointer to an array containing the data to send.
//   uint32_t size: The number of bytes to transmit.
void usartWrite(uint8_t* data, uint32_t size);

// void usartPrint(char* data)
// Writes a string out the USART peripheral. Argument must be a string
// which has a Null zero for its last byte. This function uses usartWrite
// internally and has the same set of advantages and limitations.
// Arguments:
//    char* data: Null zero termintate string to send.
void usartPrint(char* data);

// uint8_t usartGet()
// Recieves one byte from the USART peripheral. The peripheral must have
// been initialized with initIGVCUsart prior to this call. When a character
// is recieved it gets placed into a FIFO. This call dequeues one character
// from the input FIFO. this call is non-blocking.
// Returns:
//   uint8_t: The first character that hasn't been read. 0 if no data is avaliable.
uint8_t usartGet();

// void usartRead(uint8_t* bus, uint32_t bytes)
// Recieves (bytes) characters from the input FIFO buffer and places them
// in (buf). This call uses usartGet() internally and has the same advantages
// and limitations. This call is non-blocking.
// Arguments:
//   uint8_t* buf: Pointer to the location to store data
//   uint32_t bytes: The number of bytes to read. If fewer bytes are avaliable
//                   then 0 will be filled in for all missing bytes.
void usartRead(uint8_t* buf, uint32_t bytes);

// uint32_t usartHaveBytes()
// Returns the number of bytes currently in the recieve FIFO. The peripheral
// must have been initialized with initIGVCUsart prior to this call.
// Returns:
//   uint32_t: The number of bytes that can be read with usartGet().
uint32_t usartHaveBytes();


#endif