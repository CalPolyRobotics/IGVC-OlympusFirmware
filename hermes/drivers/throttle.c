#include "throttle.h"
#include "speed.h"
#include "i2c.h"

#define MCP4726 ((uint8_t) 0xC6)

volatile static int16_t goalSpeed = 0;

void setThrottle(uint32_t mms)
{
    goalSpeed = (int16_t) (mms & 0xFFFF);
}

void updateThrottle(void) {
    static int16_t kp = 1;
    //static int32_t prevError = 0;

    int16_t currentSpeed = getSpeed();
    int16_t error  = goalSpeed - currentSpeed;
    //int32_t dError = prevError - error;
    //prevError = error;

    /* Kp = 0.125, Kd = 0.875 */
    int32_t nextSpeed =  kp * error;//- ((875 * dError) / 100)) + currentSpeed;
    if (nextSpeed < 0)
        nextSpeed = 0;

    /* convert speed in mm/s to a DAC value (0 -> 4095) */
    uint32_t dacout = (((uint32_t) nextSpeed) & 0xFFF) ;

    // Top bits are MSB
    uint8_t dacval[2] = {0x0, 0x0};
    dacval[0] = (dacout >> 8u) & 0xF;
    dacval[1] = dacout & 0xFF;

    HAL_I2C_Master_Transmit(&hi2c1, MCP4726, dacval, 2 , 10);
}

