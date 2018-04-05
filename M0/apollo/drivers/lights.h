#ifndef _LIGHTS_H_
#define _LIGHTS_H_

#include <stdint.h>

#define TURN_SIGNAL_L GPIO_PIN_3
#define TURN_SIGNAL_PORT GPIOB
#define TURN_SIGNAL_R GPIO_PIN_4

#define STATE_R_MASK 0x01
#define STATE_L_MASK 0x02

#define TIMER_PORT GPIOA
#define TIMER_OUT GPIO_PIN_6

#define HEADLIGHTS GPIO_PIN_5

void init_apollo();

void set_turn_signal(uint8_t state);
void set_headlights(uint8_t speed);
void set_misc_lights(uint8_t light, uint8_t state, uint8_t speed);

uint16_t get_light_sensor();

#endif
