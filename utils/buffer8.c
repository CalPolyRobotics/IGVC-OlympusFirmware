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

void buffer8_put(buffer8_t* buffer, uint8_t data)
{
   __disable_irq();
   if (!(buffer8_full(buffer)))
   {
      *buffer->start = data;
      buffer->start++;
      if(buffer->start > (buffer->base + buffer->size))
      {
         buffer->start = buffer->base;
      }
   }
   __enable_irq();
}

void buffer8_write(buffer8_t* buffer, uint8_t* data, uint32_t len)
{
   __disable_irq();
   if (buffer8_space(buffer) >= len)
   {
      while (len--)
      {
         *buffer->start++ = *data++;
         if (buffer->start > (buffer->base + buffer->size))
         {
            buffer->start = buffer->base;
         }
      }
   }
   __enable_irq();
}

uint8_t buffer8_get(buffer8_t* buffer)
{
   uint8_t retVal = 0;
   __disable_irq();
   if(!buffer8_empty(buffer))
   {
      retVal = *buffer->end;
      buffer->end++;
      if (buffer->end > (buffer->base + buffer->size))
      {
         buffer->end = buffer->base;
      }
   }
   __enable_irq();
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
   if (buffer->start >= buffer->end)
   {
      return buffer->size - (buffer->start - buffer->end);
   } else {
      return buffer->end - buffer->start;
   }
}

uint32_t buffer8_bytes(buffer8_t* buffer)
{
   return buffer->size - buffer8_space(buffer);
}

uint8_t buffer8_empty(buffer8_t* buffer)
{
   return buffer->start == buffer->end;
}

uint8_t buffer8_full(buffer8_t* buffer)
{
   return buffer8_space(buffer) == 0;
}