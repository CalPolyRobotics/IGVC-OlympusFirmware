#ifndef FNR_H_
#define FNR_H_

#include "stm32f0xx_hal.h"
#include <stdint.h>

typedef enum fnr{
    NEUTRAL = 0,
    FORWARD = 1,
    REVERSE = 2,
    INVALID = 3
}fnr_t;

#define GPIO_FNR_SENSE_PORT GPIOB
#define GPIO_REVERSE_SENSE_PIN GPIO_PIN_3
#define GPIO_NEUTRAL_SENSE_PIN GPIO_PIN_4
#define GPIO_FORWARD_SENSE_PIN GPIO_PIN_5

#define GPIO_FNR_CTRL_PORT GPIOA
#define GPIO_FORWARD_CTRL_PIN GPIO_PIN_8
#define GPIO_REVERSE_CTRL_PIN GPIO_PIN_10

#define GPIO_CTRL_MODE_PORT GPIOA
#define GPIO_CTRL_MODE_PIN GPIO_PIN_9

fnr_t getFNR();
void setFNR(fnr_t state);
uint8_t isMCUCtrlMode();

#endif
