#include "main.h"
#include "config.h"
#include "spi.h"

#define MCP23C17_ADDR ((uint8_t)0x20)

#define MCP23C17_WRITE (MCP23C17_ADDR << 1 )
#define REG_GPIOA_ADDR ((uint8_t)0x12)

uint8_t charToSevenSeg(char c){
    uint8_t lookupTable[] = {0x12, 0x7B, 0x26, 0x23, 0x4B, 0x83, 0x82, 0x3B,
                             0x02, 0x03, 0x0A, 0xC2, 0x96, 0x62, 0x86, 0x8E};
    uint8_t magicNumber;

    if (c > 15) {
        switch(c) {
            case ' ':
                magicNumber = 0xFF;
                break;
            case 'h':
                magicNumber = 0x4A;
                break;
            case 'l': 
                magicNumber = 0xD6;
                break;
            case 'n': 
                magicNumber = 0xEA;
                break;
            case 'o': 
                magicNumber = 0xE2;
                break;
            case 'r': 
                magicNumber = 0xEE;
                break;
            case 'A': 
                magicNumber = 0xBF;
                break;
            case 'B': 
                magicNumber = 0x7F;
                break;
            case 'C': 
                magicNumber = 0xFB;
                break;
            case 'D': 
                magicNumber = 0xF7;
                break;
            case 'E': 
                magicNumber = 0xFE;
                break;
            case 'F': 
                magicNumber = 0xDF;
                break;
            case 'G': 
                magicNumber = 0xEF;
                break;
            case 'H': 
                magicNumber = 0xFD;
                break;
            default:
                break;
        }
    } else {
        magicNumber = lookupTable[c & 0xF];
    }    

    return magicNumber;
}


/**
 * Communicating over SPI with MCP23S17 GPIO Expander
 * to two seven segment displays
 *
 * Write (Address 7)(R/W 1) | Register Address | Data ....
 * to set registers in the GPIO expander
 */
void setSevenSeg(char c1, char c2)
{

    uint8_t message[4] = {MCP23C17_WRITE, REG_GPIOA_ADDR, charToSevenSeg(c1), charToSevenSeg(c2)};

    /** TODO Move SPI to DMA **/
    HAL_GPIO_WritePin(PORT_SS_IRIS, PIN_SS_IRIS, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi3, (uint8_t*)&message, sizeof(message), 500);
    HAL_GPIO_WritePin(PORT_SS_IRIS, PIN_SS_IRIS, GPIO_PIN_SET);
}

