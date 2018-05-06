#include "main.h"
#include "i2c.h"
#include "sevenSeg.h"

#define MCP23008_ADDR ((uint8_t)0x20)
#define REG_GPIO_ADDR ((uint8_t)0x09)

uint8_t charToSevenSeg(char c){
    uint8_t lookupTable[] = {0x12, 0x7B, 0x26, 0x23, 0x4B, 0x83, 0x82, 0x3B,
                             0x02, 0x03, 0x0A, 0xC2, 0x96, 0x62, 0x86, 0x8E};

    uint8_t magicNumber = lookupTable[c & 0xF];

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
    }   

    return magicNumber;
}


/**
 * Communicating over I2C with MCP23008 GPIO Expander
 * to two seven segment displays
 *
 * Write (Address 7)(R/W 1) | Register Address | Data ....
 * to set registers in the GPIO expander
 */
void setSevenSeg(char c1)
{
    uint8_t message[2] = {REG_GPIO_ADDR, charToSevenSeg(c1)};

    HAL_I2C_Master_Transmit(&hi2c1, MCP23008_ADDR, message, sizeof(message), 500);
}

void setSevenSegFNR(fnr_t state)
{
    switch(state){
        case NEUTRAL:
            setSevenSeg('0');
            break;
        case FORWARD:
            setSevenSeg('1');
            break;
        case REVERSE:
            setSevenSeg('2');
            break;
        case INVALID:
            setSevenSeg('3');
            break;
        default:
            setSevenSeg('-');
            break;
    }
}

