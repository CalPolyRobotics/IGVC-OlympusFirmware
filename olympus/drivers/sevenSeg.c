#include "config.h"
#include "gpio.h"

#define ALL_PINS (GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6)

void initSevenSeg(){
    __GPIOD_CLK_ENABLE();
    __GPIOE_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct;

    // SEVEN SEG GPIO Configuration
    GPIO_InitStruct.Pin = ALL_PINS; 
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_LOW;

    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
}

/**
 * Only Handles 0-9 and A-F
 * Returns byte to control segment
 * 1 means on 0 mean off
 * MSB -> LSB
 * x g f e d c b a
 */
uint8_t charToSevenSeg(char c){
    switch(c){
        case '0':
            return 0x3F;
        case '1':
            return 0x06;
        case '2':
            return 0x5B;
        case '3':
            return 0x4F;
        case '4':
            return 0x66;
        case '5':
            return 0x6D;
        case '6':
            return 0x7D;
        case '7':
            return 0x07;
        case '8':
            return 0x7F;
        case '9':
            return 0x67;
        case 'A':
            return 0x77;
        case 'B':
            return 0x7C;
        case 'C':
            return 0x39;
        case 'D':
            return 0x5E;
        case 'E':
            return 0x79;
        case 'F':
            return 0x71;
    }

    return 0xFF;
}

void setSevenSeg(char* mess)
{
    // Seven segments are active low
 
    HAL_GPIO_WritePin(GPIOE, ~charToSevenSeg(mess[0]), GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOE, charToSevenSeg(mess[0]), GPIO_PIN_RESET);

    HAL_GPIO_WritePin(GPIOD, ~charToSevenSeg(mess[1]), GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOD, charToSevenSeg(mess[1]), GPIO_PIN_RESET);
}
