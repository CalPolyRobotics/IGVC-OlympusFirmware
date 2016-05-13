//A circular buffer for 8bit values
#include "main.h"
#include <stdint.h>

#include "buffer8.h"

void buffer8_init(buffer8_t* buffer, uint8_t* base, uint32_t size)
{
    buffer->base = base;
    buffer->size = size;
    buffer->start = base;
    buffer->end = base;
}

int bufferFullCount = 0;

void buffer8_put(buffer8_t* buffer, uint8_t data)
{
    if (!(buffer8_full(buffer)))
    {
        *buffer->start = data;
        buffer->start++;
        if(buffer->start > (buffer->base + buffer->size))
        {
            buffer->start = buffer->base;
        }
    } else {
        printf("FULL!!!\r\n");
        bufferFullCount++;
    }
}

void buffer8_write(buffer8_t* buffer, uint8_t* data, uint32_t len)
{
    if (buffer8_space(buffer) >= len)
    {
        while (len--)
        {
            buffer8_put(buffer, *data++);
        }
    } else {
        printf("Write Full\r\n");
    }

    /*if (buffer8_space(buffer) >= len)
    {
        while (len--)
        {
            *buffer->start++ = *data++;
            if (buffer->start > (buffer->base + buffer->size))
            {
                buffer->start = buffer->base;
            }
        }
    }*/
}

uint8_t buffer8_get(buffer8_t* buffer)
{
    uint8_t retVal = 0;
    if(!buffer8_empty(buffer))
    {
        retVal = *buffer->end;
        buffer->end++;
        if (buffer->end > (buffer->base + buffer->size))
        {
            buffer->end = buffer->base;
        }
    }
    return retVal;
}

uint8_t buffer8_peek(buffer8_t* buffer)
{
    if(!buffer8_empty(buffer))
    {
        return *buffer->end;
    } else {
        return 0;
    }
}

uint32_t buffer8_space(buffer8_t* buffer)
{
    return buffer->size - buffer8_bytes(buffer);
}

uint32_t buffer8_bytes(buffer8_t* buffer)
{
    if ((uintptr_t)buffer->start >= (uintptr_t)buffer->end)
    {
        return (uintptr_t)buffer->start - (uintptr_t)buffer->end;
    } else {
        return buffer->size - ((uintptr_t)buffer->end - (uintptr_t)buffer->start);
    }
}

uint8_t buffer8_empty(buffer8_t* buffer)
{
    return (uintptr_t)buffer->start == (uintptr_t)buffer->end;
}

uint8_t buffer8_full(buffer8_t* buffer)
{
    return buffer8_bytes(buffer) >= buffer->size;

    /*if (buffer->end >= buffer->start)
    {
        return buffer->end - 1 == buffer->start;
    } else {
        return (buffer->end == buffer->base) &&
               (buffer->base == buffer->base + buffer->size - 1);
    }*/
}
