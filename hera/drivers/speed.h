#ifndef __SPEED_H_
#define __SPEED_H_

#include <stdint.h>

uint16_t diffCountsToSpeedInMMs(uint32_t diffCounts);
void checkTimeout(void);

#endif
