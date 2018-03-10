#include "main.h"

void init_spi_master(){
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_SPI1_CLK_ENABLE();

    // (+++) Configure these SPI pins as alternate function push-pull

    GPIO_InitTypeDef gpioInit;

    gpioInit.Pin = GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
    gpioInit.Pull = GPIO_NOPULL;
    gpioInit.Mode = GPIO_MODE_AF_PP;
    gpioInit.Speed = GPIO_SPEED_FREQ_MEDIUM;
    gpioInit.Alternate = GPIO_AF0_SPI1;

    HAL_GPIO_Init(GPIOA, &gpioInit);

    gpioInit.Pin = GPIO_PIN_4;
    gpioInit.Pull = GPIO_NOPULL;
    gpioInit.Mode = GPIO_MODE_OUTPUT_PP;
    gpioInit.Speed = GPIO_SPEED_FREQ_MEDIUM;
    gpioInit.Alternate = GPIO_AF0_SPI1;

    HAL_GPIO_Init(GPIOA, &gpioInit);
    GPIOA -> ODR |= GPIO_PIN_4;

    SPI_InitTypeDef ispi;
    ispi.Mode = SPI_MODE_MASTER;
    ispi.Direction = SPI_DIRECTION_2LINES;
    ispi.DataSize = SPI_DATASIZE_8BIT;
    ispi.CLKPolarity = SPI_POLARITY_LOW;
    ispi.CLKPhase = SPI_PHASE_1EDGE;
    ispi.NSS = SPI_NSS_SOFT;
    ispi.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
    ispi.FirstBit = SPI_FIRSTBIT_MSB;
    ispi.TIMode = SPI_TIMODE_DISABLE;
    ispi.NSSPMode =  SPI_NSS_PULSE_DISABLE;

    SPI_HandleTypeDef hspi;
    hspi.Instance = SPI1;
    hspi.Init = ispi;

    //HAL_StatusTypeDef stat = HAL_SPI_Init(&hspi);
    HAL_SPI_Init(&hspi);


    uint8_t reg = 0xAA;
    uint8_t val[2];
    while(1){
        GPIOA -> ODR &= ~GPIO_PIN_4;
        HAL_SPI_Transmit(&hspi, &reg, 1, 1000);
        HAL_Delay(1);
        HAL_SPI_Receive(&hspi, (uint8_t*)val, 2, 1000);
        GPIOA -> ODR |= GPIO_PIN_4;
    }
}

void init_spi_slave(){
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_SPI1_CLK_ENABLE();

    // (+++) Configure these SPI pins as alternate function push-pull

    GPIO_InitTypeDef gpioInit;

    gpioInit.Pin = GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
    gpioInit.Pull = GPIO_NOPULL;
    gpioInit.Mode = GPIO_MODE_AF_PP;
    gpioInit.Speed = GPIO_SPEED_FREQ_MEDIUM;
    gpioInit.Alternate = GPIO_AF0_SPI1;

    HAL_GPIO_Init(GPIOA, &gpioInit);

    SPI_InitTypeDef ispi;
    ispi.Mode = SPI_MODE_SLAVE;
    ispi.Direction = SPI_DIRECTION_2LINES;
    ispi.DataSize = SPI_DATASIZE_8BIT;
    ispi.CLKPolarity = SPI_POLARITY_LOW;
    ispi.CLKPhase = SPI_PHASE_1EDGE;
    ispi.NSS = SPI_NSS_HARD_INPUT;
    ispi.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
    ispi.FirstBit = SPI_FIRSTBIT_MSB;
    ispi.TIMode = SPI_TIMODE_DISABLE;
    ispi.NSSPMode =  SPI_NSS_PULSE_DISABLE;

    SPI_HandleTypeDef hspi;
    hspi.Instance = SPI1;
    hspi.Init = ispi;

    //HAL_StatusTypeDef stat = HAL_SPI_Init(&hspi);
    HAL_SPI_Init(&hspi);

    uint8_t reg; 
    uint8_t data[2] = {'a', 'b'};
    while(1){
        HAL_SPI_Receive(&hspi, &reg, 1, 1000);
        if(reg == 0xAA){
            GPIOB -> ODR ^= GPIO_PIN_3;
            HAL_SPI_Transmit(&hspi, (uint8_t*)data, 2, 1000);
        }
    }
}

void send_reg(SPI_HandleTypeDef *hspi, uint8_t reg){
    uint16_t size = 1;
    while (size > 0U)
    {
        /* Wait until TXE flag is set to send data */
        if (__HAL_SPI_GET_FLAG(hspi, SPI_FLAG_TXE))
        {
          *((__IO uint8_t *)&hspi->Instance->DR) = reg;
          size--;
        }
    }
}
void send_data(SPI_HandleTypeDef *hspi, uint8_t* pData, uint16_t size){
    while (size > 0U)
    {
        /* Wait until TXE flag is set to send data */
        if (__HAL_SPI_GET_FLAG(hspi, SPI_FLAG_TXE))
        {
            if (size > 1U) { /* write on the data register in packing mode */
              hspi->Instance->DR = *((uint16_t *)pData);
              pData += sizeof(uint16_t);
              size -= 2U;
            } else {
              *((__IO uint8_t *)&hspi->Instance->DR) = (*pData++);
              size--;
            }
        }
    }
}

void receive_datum(SPI_HandleTypeDef *hspi, uint8_t* pData){
    /* set fiforxthresold according the reception data length: 8bit */
    SET_BIT(hspi->Instance->CR2, SPI_RXFIFO_THRESHOLD);
    int size = 1;
    while (size > 0U)
    {
        /* Check the RXNE flag */
        if (__HAL_SPI_GET_FLAG(hspi, SPI_FLAG_RXNE))
        {
            /* read the received data */
            *pData = *(__IO uint8_t *)&hspi->Instance->DR;
            pData += sizeof(uint8_t);
            size--;
        }
    }

}

void receive_data(SPI_HandleTypeDef *hspi, uint8_t* pData, uint16_t size){
    while (size > 0U)
    {
        /* Check the RXNE flag */
        if (__HAL_SPI_GET_FLAG(hspi, SPI_FLAG_RXNE))
        {
            /* read the received data */
            *pData = *(__IO uint8_t *)&hspi->Instance->DR;
            pData += sizeof(uint8_t);
            size--;
        }
    }
}
