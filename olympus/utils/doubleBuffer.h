#ifndef __DOUBLE_BUFFER_H__
#define __DOUBLE_BUFFER_H__

typedef struct {
    uint8_t* buffers[2];
    uint32_t bufferLengths[2];
    uint32_t bufferSize;
    uint32_t readIdx;
    uint8_t activeBuffer;
} DoubleBuffer_t;

void doubleBuffer_init(DoubleBuffer_t* buffer, uint8_t* base, uint32_t bufLen);

void doubleBuffer_switchActive(DoubleBuffer_t* buffer);

uint8_t doubleBuffer_write(DoubleBuffer_t* buffer, uint8_t* data, uint32_t len);

uint32_t doubleBuffer_getReadLength(DoubleBuffer_t* buffer);

uint32_t doubleBuffer_read(DoubleBuffer_t* buffer, uint8_t* data, uint32_t len);

#endif
