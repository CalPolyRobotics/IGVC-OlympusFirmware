/**
  ******************************************************************************
  * File Name          : led.c
  * Description        : This file provides code for the configuration
  *                      of the GPIO LEDs.
  ******************************************************************************
*/

#include <stdio.h>
#include "gpio.h"

typedef struct 
{
    GPIO_TypeDef* port;
    uint16_t pin;
} LED_t;

LED_t LEDtable[] = {{},
                    {GPIOA, GPIO_PIN_4},
                    {GPIOA, GPIO_PIN_5},
                    {GPIOA, GPIO_PIN_7},
                    {GPIOB, GPIO_PIN_3},
                    {GPIOB, GPIO_PIN_4},
                    {GPIOB, GPIO_PIN_5},
                    {GPIOC, GPIO_PIN_4},
                    {GPIOC, GPIO_PIN_5},
                    {GPIOC, GPIO_PIN_7},
                    {GPIOC, GPIO_PIN_8},
                    {GPIOD, GPIO_PIN_2},
                    {GPIOH, GPIO_PIN_1}};

void setLED(uint8_t ledNum, uint8_t state)
{
    if (((state == 0) || (state == 1)) && (ledNum > 0) && (ledNum <= 12))
    {
        HAL_GPIO_WritePin(LEDtable[ledNum].port,LEDtable[ledNum].pin,state);
    }
}

