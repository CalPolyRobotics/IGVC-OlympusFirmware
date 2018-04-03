#ifndef __COMMS_H_
#define __COMMS_H_

#include <stdint.h>

void commsFSM(uint8_t data);

#define COMMS_START_BYTE ((uint8_t)0xEA)

#endif
