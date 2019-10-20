#include "throttle.h"
#include "i2c.h"

#define MCP4726 ((uint8_t) 0xC6)

void setThrottle(uint8_t *data)
{
    // Protect against invalid command that could cause I2C DAC to shutdown
    data[0] = data[0] & 0x0F;
    HAL_I2C_Master_Transmit(&hi2c1, MCP4726, data, 2 , 10);
}
