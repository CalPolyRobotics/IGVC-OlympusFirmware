#include "throttle.h"
#include "i2c.h"

#define MCP4726 ((uint8_t) 0xC6)

void setThrottle(uint8_t *data)
{
    HAL_I2C_Master_Transmit(&hi2c1, MCP4726, data, sizeof(data), 10);
}
