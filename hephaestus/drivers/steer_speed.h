#ifndef __STEER_SPEED_H_
#define __STEER_SPEED_H_

#include <stdint.h>

#define PING_PONG_SIZE ((size_t) 2u)

extern int16_t ch3Speed[2];
extern volatile int ch3SpeedWr;

void checkTimeout(void);
int16_t clk_ticks_to_speed_mRev_s(uint32_t diffCounts);

#endif
