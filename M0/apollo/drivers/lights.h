#ifndef _LIGHTS_H_
#define _LIGHTS_H_

#include <stdint.h>

#define TURN_SIGNAL_L GPIO_PIN_1
#define TURN_SIGNAL_PORT GPIOA
#define TURN_SIGNAL_R GPIO_PIN_0

#define STATE_R_MASK 0x01
#define STATE_L_MASK 0x02

#define MISC_PORT GPIOB

#define HEADLIGHTS GPIO_PIN_2

#define MISC1 GPIO_PIN_7
#define MISC2 GPIO_PIN_6
#define MISC3 GPIO_PIN_5
#define MISC4 GPIO_PIN_4
#define MISC5 GPIO_PIN_3
#define MISC6 GPIO_PIN_11

void init_apollo();

void set_turn_signal(uint8_t state);
void set_headlights(uint8_t speed);
void set_misc_lights(uint8_t light, uint8_t speed);

uint16_t get_light_sensor();

#endif
