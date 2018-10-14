
#include <stdint.h>

void mapCharacter(uint8_t inChar, uint8_t* outStr)
{

    uint16_t unicodeChar;

    if (inChar < 0x80)
    {
        unicodeChar = 0x2F60 + inChar;
    } else {
        unicodeChar = 0x2E80 + (inChar - 0x80);
    }

    outStr[0] = 0xE0 | ((unicodeChar >> 12) & 0xF);
    outStr[1] = 0x80 | ((unicodeChar >> 6) & 0x3F);
    outStr[2] = 0x80 | ((unicodeChar) & 0x3F);
    outStr[3] = 0;
}
