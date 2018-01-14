#ifndef __LED_H__
#define __LED_H__

typedef enum {LED_OFF = 0, LED_ON} LEDState;

// Writes a GPIO LED. ledNum is the GPIO number from 1-12. state is 0 or 1
void writeLED(uint8_t ledNum, LEDState state);

void commsSetLightsCallback();

#endif
