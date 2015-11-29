#include "main.h"
#include "config.h"
#include "gpio.h"

static I2C_HandleTypeDef i2c;

void initIGVCI2C()
{
    i2c.Instance = IGVC_I2C;

    i2c.Init.ClockSpeed = I2C_CLOCK_SPEED;
    i2c.Init.DutyCycle = I2C_DUTYCYCLE_2;
    i2c.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    i2c.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    i2c.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

    i2c.Lock = HAL_UNLOCKED;
    i2c.State = HAL_I2C_STATE_RESET;

    HAL_I2C_Init(&i2c);

}

void HAL_I2C_MspInit(I2C_HandleTypeDef *hi2c)
{
  // GPIO_InitTypeDef gpio;

  // gpio.Pin = GPIO_PIN_10 | GPIO_PIN_11;
  // gpio.Mode = GPIO_MODE_AF_OD;
  // gpio.Pull = GPIO_NOPULL;
  // gpio.Speed = GPIO_SPEED_FREQ_HIGH;
  // gpio.Alternate = GPIO_AF4_I2C1;

  // HAL_GPIO_Init(GPIOB, &gpio); 

  initGPIOAlternate_OD(GPIO_I2C_SDA, I2C_SDA_AF);
  initGPIOAlternate_OD(GPIO_I2C_SCL, I2C_SCL_AF);
}

void testI2C()
{
    uint8_t data = 9;
    HAL_I2C_Master_Transmit(&i2c, 0xA0, &data, 1, 1000);
}

