#ifndef BOOT_H
#define BOOT_H

#include <stdint.h>

void remapVectorTable();

uint8_t bootload(uint8_t *data);

#endif
