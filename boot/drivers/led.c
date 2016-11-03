/**
 ******************************************************************************
 * File Name          : led.c
 * Description        : This file provides code for the configuration
 *                      of the GPIO LEDs.
 ******************************************************************************
*/
#include "gpio.h"
#include "led.h"
#include "config.h"

typedef struct 
{
    GPIO_TypeDef* port;
    uint16_t pin;
} LED_t;

LED_t LEDtable[] = {{},
                    {GPIO_DEBUG_1},
                    {GPIO_DEBUG_2},
                    {GPIO_DEBUG_3},
                    {GPIO_DEBUG_4},
                    {GPIO_DEBUG_5},
                    {GPIO_DEBUG_6},
                    {GPIO_DEBUG_7},
                    {GPIO_DEBUG_8},
                    {GPIO_DEBUG_9},
                    {GPIO_DEBUG_10},
                    {GPIO_DEBUG_11},
                    {GPIO_DEBUG_12}
                   };

void setLED(uint8_t ledNum, LEDState state)
{
    if (((state == LED_ON) || (state == LED_OFF)) && (ledNum > 0) && (ledNum <= NUM_DEBUG_PINS))
    {
        HAL_GPIO_WritePin(LEDtable[ledNum].port, LEDtable[ledNum].pin, state);
    }
}
