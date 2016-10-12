
#include "main.h"
#include "config.h"
#include "i2c.h"

void setSevenSeg(char a)
{

    uint8_t lookupTable[] = {0x12, 0x7B, 0x26, 0x23, 0x4B, 0x83, 0x82, 0x3B,
                             0x02, 0x03, 0x0A, 0xC2, 0x96, 0x62, 0x86, 0x8E};

    uint8_t magicNumber;

    if (a > 15) {
        switch(a) {
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
        magicNumber = lookupTable[a & 0xF];
    }    

    i2cAddTxTransaction(IRIS_SEG_I2C_ADDR, &magicNumber, 1, NULL, NULL);
}

