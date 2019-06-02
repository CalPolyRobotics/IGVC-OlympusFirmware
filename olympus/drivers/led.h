#ifndef __LED_H__
#define __LED_H__

#include "timerCallback.h"

typedef enum {LED_OFF = 0, LED_ON} LEDState;

Timer_Return heartbeat(void *dummy);

// Writes a GPIO LED. ledNum is the GPIO number from 1-12. state is 0 or 1
void writeLED(uint8_t ledNum, LEDState state);

void commsSetLeds(uint8_t* data);

#endif
