
#include <stdio.h>

#include "i2c.h"
#include "gpio.h"

void i2cInit()
{
    I2C2->CR1 = 0;

    I2C2->CR2 = I2C_CR2_ITBUFEN | // Enable TxE and RxNE events
                I2C_CR2_ITEVTEN | // Enable event interrupts
                I2C_CR2_ITERREN | // Enable error event interrupts
                (0x1E);           // APB Clk speed is 30 MHz (max speed)

    I2C2->CCR = 150;    // Slow speed and high and low period times
    I2C2->TRISE = 0x1F; // Rist time for slow I2C. This needs to change for fast mode

    (void) I2C2->SR1; // Clear some flags
    (void) I2C2->SR2; // Clear some flags

    I2C2->CR1 |= I2C_CR1_PE; // Enable the peripheral
}



