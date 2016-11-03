#ifndef __BUFFER8_H__
#define __BUFFER8_H__

#include <stdint.h>

typedef struct {
    uint8_t* base;
    uint32_t size;
    uint8_t* start;
    uint8_t* end; 
} buffer8_t;

void buffer8_init(buffer8_t* buffer, uint8_t* base, uint32_t size);
void buffer8_put(buffer8_t* buffer, uint8_t data);
void buffer8_write(buffer8_t* buffer, uint8_t* data, uint32_t len);
uint8_t buffer8_get(buffer8_t* buffer);
uint8_t buffer8_peek(buffer8_t* buffer);
uint32_t buffer8_space(buffer8_t* buffer);
uint8_t buffer8_empty(buffer8_t* buffer);
uint8_t buffer8_full(buffer8_t* buffer);
uint32_t buffer8_bytes(buffer8_t* buffer);

#endif