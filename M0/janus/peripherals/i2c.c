#include "i2c.h"

I2C_HandleTypeDef hi2c1;

void MX_I2C1_Init(void){
    hi2c1.Instance = I2C1;
    /* 
     * PRESC  - 0x5
     * SCLL   - 0x9
     * SCLH   - 0x3
     * SDADEL - 0x3
     * SCLDEL - 0x3
     *
     * Register Representation
     * MSB                                                                 LSB
     * PRESC[3:0] | 0b0000 | SCLDEL[3:0] | SDADEL[3:0] | SCLH[7:0] | SCLL[7:0]
     */
    hi2c1.Init.Timing =((uint32_t)0x30930303);
    hi2c1.Init.OwnAddress1 = 0;
    hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c1.Init.DualAddressMode =I2C_DUALADDRESS_DISABLE;
    hi2c1.Init.OwnAddress2 = 0;
    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

    HAL_I2C_Init(&hi2c1);
}

void HAL_I2C_MspInit(I2C_HandleTypeDef* hi2c){
    /* I2C Peripheral Clock */
    __HAL_RCC_I2C1_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct;

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOA_CLK_ENABLE();

    /* RX and TX */
    GPIO_InitStruct.Pin = GPIO_PIN_11|GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}
