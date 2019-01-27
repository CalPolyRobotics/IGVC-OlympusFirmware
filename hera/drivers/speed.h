#ifndef __SPEED_H_
#define __SPEED_H_

#include <stdint.h>

extern uint16_t ch1Speed[2];
extern uint16_t ch2Speed[2];
extern volatile int ch1SpeedWr;
extern volatile int ch2SpeedWr;

void checkTimeout(void);

#endif
