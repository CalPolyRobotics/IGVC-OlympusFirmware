#ifndef __KILL_H__
#define __KILL_H__

#include "gpio.h"

#define BOARD_KILL_PIN  GPIO_PIN_9
#define BOARD_KILL_PORT GPIOA

void killBoard();

#endif