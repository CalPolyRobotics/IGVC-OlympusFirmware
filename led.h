#ifndef __LED_H__
#define __LED_H__
/**
  ******************************************************************************
  * File Name          : led.c
  * Description        : This file provides code for the configuration
  *                      of the GPIO LEDs.
  ******************************************************************************

*/

// Sets a GPIO LED. ledNum is the GPIO number from 1-12. state is 0 or 1
void setLED(uint8_t ledNum, uint8_t state);

#endif