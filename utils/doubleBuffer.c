
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "doubleBuffer.h"

void doubleBuffer_init(DoubleBuffer_t* buffer, uint8_t* base, uint32_t bufLen)
{
    buffer->buffers[0] = base;
    buffer->buffers[1] = base + (bufLen / 2);

    buffer->bufferLengths[0] = 0;
    buffer->bufferLengths[1] = 0;

    buffer->bufferSize = bufLen / 2;
    buffer->readIdx = 0;
    buffer->activeBuffer = 0;

}

void doubleBuffer_switchActive(DoubleBuffer_t* buffer)
{
    if (buffer->activeBuffer == 0)
    {
        buffer->bufferLengths[1] = 0;
        buffer->activeBuffer = 1;
    } else {
        buffer->bufferLengths[0] = 0;
        buffer->activeBuffer = 0;
    }

    buffer->readIdx = 0;
}

static uint8_t doubleBuffer_getInactive(DoubleBuffer_t* buffer)
{
    if (buffer->activeBuffer == 0)
    {
        return 1;
    } else {
        return 0;
    }
}

uint8_t doubleBuffer_write(DoubleBuffer_t* buffer, uint8_t* data, uint32_t len)
{
    uint8_t active = buffer->activeBuffer;
    uint8_t status = 0;

    if (buffer->bufferSize - buffer->bufferLengths[active] >= len)
    {
        memcpy(&buffer->buffers[active][buffer->bufferLengths[active]],
               data,
               len);

        buffer->bufferLengths[active] += len;

        status = true;
    } else {
        status = false;
    }

    return status;
}

uint32_t doubleBuffer_getReadLength(DoubleBuffer_t* buffer)
{
    uint8_t inactive = doubleBuffer_getInactive(buffer);
    uint32_t len = buffer->bufferLengths[inactive] - buffer->readIdx;

    if (len == 0)
    {
        doubleBuffer_switchActive(buffer);

        inactive = doubleBuffer_getInactive(buffer);

        len = buffer->bufferLengths[inactive] - buffer->readIdx;
    }

    return len;
}

uint32_t doubleBuffer_read(DoubleBuffer_t* buffer, uint8_t* data, uint32_t len)
{
    uint32_t bytesAvailable;
    uint32_t numBytesToRead = 0;
    uint8_t inactive;

    bytesAvailable = doubleBuffer_getReadLength(buffer);

    inactive = doubleBuffer_getInactive(buffer);

    if (bytesAvailable > 0)
    {
        numBytesToRead = len < bytesAvailable ? len : bytesAvailable;

        memcpy(data, &buffer->buffers[inactive][buffer->readIdx], numBytesToRead);

        buffer->readIdx += numBytesToRead;
    }

    return numBytesToRead;
}

