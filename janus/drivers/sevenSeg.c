#include "i2c.h"
#include "sevenSeg.h"

#define MCP23008_ADDR ((uint8_t)0x40)
#define REG_GPIO_ADDR ((uint8_t)0x09)
#define REG_IODIR_ADDR ((uint8_t)0x00)

/**
 * Returns the value for the gpio expande
 *
 * Note only works for 0-9 and A/a-F/f
 */
uint8_t charToSevenSeg(char c){
    switch(c){
        case '0':
            return 0x88;
        case '1':
            return 0xEB;
        case '2':
            return 0x4C;
        case '3':
            return 0x49;
        case '4':
            return 0x2B;
        case '5':
            return 0x19;
        case '6':
            return 0x18;
        case '7':
            return 0x8B;
        case '8':
            return 0x08;
        case '9':
            return 0x0B;
        case 'A':
        case 'a':
            return 0x0A;
        case 'B':
        case 'b':
            return 0x38;
        case 'C':
        case 'c':
            return 0x9C;
        case 'D':
        case 'd':
            return 0x68;
        case 'E':
        case 'e':
            return 0x1C;
        case 'F':
        case 'f':
            return 0x1E;
        default:
            return 0xFF;
    }
}


/**
 * Communicating over I2C with MCP23008 GPIO Expander
 * to two seven segment displays
 *
 * Write (Address 7)(R/W 1) | Register Address | Data ....
 * to set registers in the GPIO expander and set pins as outputs
 */
void setSevenSeg(char c)
{
    uint8_t segVal = charToSevenSeg(c);

    // Set low pins as outputs
    uint8_t message[2] = {REG_IODIR_ADDR, segVal};
    HAL_I2C_Master_Transmit(&hi2c1, MCP23008_ADDR, message, sizeof(message), 1);

    // Set pins
    message[0] = REG_GPIO_ADDR;
    HAL_I2C_Master_Transmit(&hi2c1, MCP23008_ADDR, message, sizeof(message), 1);
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

