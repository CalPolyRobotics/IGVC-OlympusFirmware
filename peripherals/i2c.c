
#include <stdio.h>

#include "i2c.h"
#include "gpio.h"

I2C_HandleTypeDef hi2c2;

/* I2C2 init function */
void MX_I2C2_Init(void)
{
    hi2c2.Instance = I2C2;
    hi2c2.Init.ClockSpeed = 100000;
    hi2c2.Init.DutyCycle = I2C_DUTYCYCLE_2;
    hi2c2.Init.OwnAddress1 = 0;
    hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLED;
    hi2c2.Init.OwnAddress2 = 0;
    hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLED;
    hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLED;
    HAL_I2C_Init(&hi2c2);
}

void HAL_I2C_MspInit(I2C_HandleTypeDef* hi2c)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    if(hi2c->Instance==I2C2)
    {
        /**I2C2 GPIO Configuration    
        PB10     ------> I2C2_SCL
        PB11     ------> I2C2_SDA 
        */
        GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_11;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF4_I2C2;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

        /* Peripheral clock enable */
        __I2C2_CLK_ENABLE();
    }
}

void HAL_I2C_MspDeInit(I2C_HandleTypeDef* hi2c)
{
    if(hi2c->Instance==I2C2)
    {
        /* Peripheral clock disable */
        __I2C2_CLK_DISABLE();

        /**I2C2 GPIO Configuration    
        PB10     ------> I2C2_SCL
        PB11     ------> I2C2_SDA 
        */
        HAL_GPIO_DeInit(GPIOB, GPIO_PIN_10|GPIO_PIN_11);
    }
} 

void i2cTransmit(uint8_t address, uint8_t* data, uint32_t len)
{
    HAL_I2C_Master_Transmit(&hi2c2, address, data, len, 10000);
}

void i2cReceive(uint8_t address, uint8_t *data, uint32_t len) {
    HAL_I2C_Master_Receive(&hi2c2, address, data, len, 10000);
}

void i2cScan()
{
    uint8_t address;

    for(address = 0; address < 0x80; address++)
    {
        if (HAL_I2C_IsDeviceReady(&hi2c2, address << 1, 1, 10000) == HAL_OK)
        {
            printf("0x%X ", address);
        }  
    }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
