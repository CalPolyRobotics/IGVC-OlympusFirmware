#ifndef __STEER_H_
#define __STEER_H_

#include <stdint.h>
#include "commsLib.h"

#define MAX_LEFT_POT  ((uint16_t)975u)
#define MIDDLE_POT    ((uint16_t)1500u)
#define MAX_RIGHT_POT ((uint16_t)1917u)

#define MAX_LEFT_DEG  ((uint8_t)45u)
#define MIDDLE_DEG    ((uint8_t)90u)
#define MAX_RIGHT_DEG ((uint8_t)125u)

extern uint16_t steeringPot;

void update_steer_control_loop();
void set_steer_target(uint8_t angle);

#endif
