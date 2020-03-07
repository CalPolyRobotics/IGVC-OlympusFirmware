#ifndef _THROTTLE_H_
#define _THROTTLE_H_

#include "stdint.h"
//defines the max speed to be 4095mm/s
#define MAX_THROTTLE ((uint32_t) 4095)

void setThrottle(uint32_t mms);
void updateThrottle(void);

#endif
