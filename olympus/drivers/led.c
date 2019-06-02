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
                    {LED_1_PRT, LED_1_PIN},
                    {LED_2_PRT, LED_2_PIN},
                    {LED_3_PRT, LED_3_PIN},
                    {LED_4_PRT, LED_4_PIN},
                    {LED_5_PRT, LED_5_PIN},
                    {LED_6_PRT, LED_6_PIN},
                   };

Timer_Return heartbeat(void *dummy){
    HAL_GPIO_TogglePin(LED_1_PRT, LED_1_PIN);

    return CONTINUE_TIMER;
}

void writeLED(uint8_t ledNum, LEDState state)
{
    if (((state == LED_ON) || (state == LED_OFF)) && (ledNum > 0) &&
        (ledNum <= sizeof(LEDtable)/sizeof(LEDtable[0])))
    {
        HAL_GPIO_WritePin(LEDtable[ledNum].port, LEDtable[ledNum].pin, state);
    }
}

void commsSetLeds(uint8_t* data){
    uint8_t byte0 = data[1];
    uint8_t byte1 = data[0];

    int i;
    for(i = 0; i < 8; i++){
        writeLED(i + 1, (byte0 >> i) & 1);
    }

    for(i = 0; i < 4; i++){
        writeLED(i + 9, (byte1 >> i) & 1);
    }
}
