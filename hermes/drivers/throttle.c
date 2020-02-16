#include "throttle.h"
#include "speed.h"
#include "i2c.h"

#define MCP4726 ((uint8_t) 0xC6)

int16_t goalSpeed = 0;

void setThrottle(int32_t mms)
{
    goalSpeed = mms;
}

void updateThrottle(void) {
    static uint16_t prevError = 0;

    uint8_t dacval[2] = {0x0, 0x0};
    uint16_t error, dError;
    uint32_t dacout;

    int16_t currentSpeed, nextSpeed;

    currentSpeed = getSpeed();

    error = goalSpeed - currentSpeed;
    dError = error - prevError;
    prevError = error;

    nextSpeed = 0.5 * error + 0.5 * dError; /* Kp = 0.5, Kd = 0.5 */

    /* convert speed in mm/s to a DAC value (0 -> 4096) */
    dacout = (nextSpeed * 0xFFF) / MAX_THROTTLE;

    // Top bits are MSB
    dacval[0] = (dacout >> 8u) & 0x0F ;
    dacval[1] = dacout & 0xFF;

    HAL_I2C_Master_Transmit(&hi2c1, MCP4726, dacval, 2 , 10);
}

