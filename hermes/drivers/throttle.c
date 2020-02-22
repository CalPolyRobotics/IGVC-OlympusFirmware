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
    static int32_t prevError = 0;

    int16_t currentSpeed = getSpeed();

    int32_t error  = goalSpeed - currentSpeed;
    int32_t dError = prevError - error;
    prevError = error;

    /* Kp = 0.125, Kd = 0.875 */
    int32_t nextSpeed = currentSpeed + ((125 * error) / 100 - (875 * dError) / 100);

    /* convert speed in mm/s to a DAC value (0 -> 4096) */
    uint32_t dacout = (nextSpeed * 0xFFF) / MAX_THROTTLE;

    // Top bits are MSB
    uint8_t dacval[2] = {0x0, 0x0};
    dacval[0] = (dacout >> 8u) & 0x0F ;
    dacval[1] = dacout;

    HAL_I2C_Master_Transmit(&hi2c1, MCP4726, dacval, 2 , 10);
}

