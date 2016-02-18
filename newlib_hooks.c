#include "usart.h"

int _write(int fd, char *buf, int nbytes)
{
    usartWrite(buf, nbytes);
    return nbytes;
}
