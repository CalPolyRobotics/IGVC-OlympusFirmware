#ifndef __SPEED_H_
#define __SPEED_H_

#include <stdint.h>

extern int16_t ch1Speed[2];
extern volatile int ch1SpeedWr;

void checkTimeout(void);
int16_t diffCountsToSpeedInMMs(uint32_t diffCounts);


#endif
