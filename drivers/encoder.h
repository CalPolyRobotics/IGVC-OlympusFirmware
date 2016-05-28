#ifndef __encoder
#define __encoder

#include "gpio.h"
#include <stdio.h>

void initEncoderInputCapture(); //for initializing the encoders and timers
void setMinimumMetersPerSecond(uint32_t); //for adjusting the resolution of the encoders
uint32_t getMaximumMetersPerSecond(); //returns the maximum fps measured by the encoders


#endif
