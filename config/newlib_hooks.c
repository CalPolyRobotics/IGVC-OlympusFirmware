#include "usart.h"

int _write(int fd, char *buf, int nbytes)
{
   usartWrite((uint8_t*)buf, nbytes);

   return nbytes;
}
