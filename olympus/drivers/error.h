#ifndef __error_H
#define __error_H

#include "config.h"
#include "gpio.h"
#include "kill.h"
#include "sounds.h"
#include "sevenSeg.h"

typedef enum {KILL = 0, ALERT, NOTIFY} THREATLevel;

void assert_failed(uint8_t* file, uint32_t line);
void ErrorHandler(char* type, THREATLevel severity);

#endif
