#ifndef _THROTTLE_H_
#define _THROTTLE_H_

#include "stdint.h"
//defines the max speed to be 4000mm/s
#define MAX_THROTTLE ((uint32_t) 4000)

void setThrottle(int32_t mms);
void updateThrottle(void);

#endif
