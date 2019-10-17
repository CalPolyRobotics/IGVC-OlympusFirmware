#ifndef __STEER_SPEED_H_
#define __STEER_SPEED_H_

#include <stdint.h>

extern int16_t ch3Speed[2];
extern volatile int ch3SpeedWr;

void check_steer_speed_timeout(void);
int16_t clk_ticks_to_speed_mRev_s(uint32_t diffCounts);

#endif
