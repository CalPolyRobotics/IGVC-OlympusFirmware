#ifndef __SPEED_H_
#define __SPEED_H_

#include <stdint.h>

uint32_t diffCountsToSpeedInMMs(uint32_t diffCounts);
void checkTimeout(void);

#endif
