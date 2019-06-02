#include "config.h"
#include "spi.h"

#define MCP23C17_ADDR ((uint8_t)0x20)

#define MCP23C17_WRITE (MCP23C17_ADDR << 1 )
#define REG_GPIOA_ADDR ((uint8_t)0x12)
#define REG_IODIRA_ADDR ((uint8_t)0x00)


// Register Masks for Each Individual Segment
#define SEG_1A ((uint16_t) 1 << 13)
#define SEG_1B ((uint16_t) 1 << 12)
#define SEG_1C ((uint16_t) 1 << 2)
#define SEG_1D ((uint16_t) 1 << 1)
#define SEG_1E ((uint16_t) 1 << 0)
#define SEG_1F ((uint16_t) 1 << 14)
#define SEG_1G ((uint16_t) 1 << 15)
#define SEG_1P ((uint16_t) 1 << 3)
#define SEG_2A ((uint16_t) 1 << 9)
#define SEG_2B ((uint16_t) 1 << 8)
#define SEG_2C ((uint16_t) 1 << 6)
#define SEG_2D ((uint16_t) 1 << 5)
#define SEG_2E ((uint16_t) 1 << 4)
#define SEG_2F ((uint16_t) 1 << 10)
#define SEG_2G ((uint16_t) 1 << 11)
#define SEG_2P ((uint16_t) 1 << 7)

#define FUNC_A ((uint8_t) 1 << 7)
#define FUNC_B ((uint8_t) 1 << 6)
#define FUNC_C ((uint8_t) 1 << 5)
#define FUNC_D ((uint8_t) 1 << 4)
#define FUNC_E ((uint8_t) 1 << 3)
#define FUNC_F ((uint8_t) 1 << 2)
#define FUNC_G ((uint8_t) 1 << 1)
#define FUNC_P ((uint8_t) 1 << 0)

typedef union u16{
    uint16_t u16;
    uint8_t u8[2];
}u16_t;

static u16_t uint8ToGPIOReg(uint8_t a, uint8_t b){
    // Register Stores
    // GPIOA | GPIOB

    u16_t reg;
    reg.u16 = 0;

    reg.u16 |= FUNC_A & a ? SEG_1A : 0;
    reg.u16 |= FUNC_B & a ? SEG_1B : 0;
    reg.u16 |= FUNC_C & a ? SEG_1C : 0;
    reg.u16 |= FUNC_D & a ? SEG_1D : 0;
    reg.u16 |= FUNC_E & a ? SEG_1E : 0;
    reg.u16 |= FUNC_F & a ? SEG_1F : 0;
    reg.u16 |= FUNC_G & a ? SEG_1G : 0;
    reg.u16 |= FUNC_P & a ? SEG_1P : 0;

    reg.u16 |= FUNC_A & b ? SEG_2A : 0;
    reg.u16 |= FUNC_B & b ? SEG_2B : 0;
    reg.u16 |= FUNC_C & b ? SEG_2C : 0;
    reg.u16 |= FUNC_D & b ? SEG_2D : 0;
    reg.u16 |= FUNC_E & b ? SEG_2E : 0;
    reg.u16 |= FUNC_F & b ? SEG_2F : 0;
    reg.u16 |= FUNC_G & b ? SEG_2G : 0;
    reg.u16 |= FUNC_P & b ? SEG_2P : 0;

    return reg;
}

/**
 * Only Handles 0-9 and A-F
 * Returns byte to control segment
 * 0 means on 1 mean off
 * MSB -> LSB
 * a b c d e f g p
 */
uint8_t charToSevenSeg(char c){
    switch(c){
        case '0':
            return 0x03;
        case '1':
            return 0x9F;
        case '2':
            return 0x25;
        case '3':
            return 0x0D;
        case '4':
            return 0x99;
        case '5':
            return 0x49;
        case '6':
            return 0x41;
        case '7':
            return 0x1F;
        case '8':
            return 0x01;
        case '9':
            return 0x19;
        case 'A':
            return 0x11;
        case 'B':
            return 0xC1;
        case 'C':
            return 0x63;
        case 'D':
            return 0x85;
        case 'E':
            return 0x61;
        case 'F':
            return 0x71;
    }

    return 0xFF;
}

/**
 * Communicating over SPI with MCP23S17 GPIO Expander
 * to two seven segment displays
 *
 * Write (Address 7)(R/W 1) | Register Address | Data ....
 * to set registers in the GPIO expander
 *
 * A low value in the IODIR register make the GPIO output
 * whatever is in the GPIO register for that pin
 */
void setSevenSeg(char* mess)
{
    return;
}

