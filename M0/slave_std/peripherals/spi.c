#include "stm32f0xx_hal.h"
#include "spi.h"

SPI_HandleTypeDef hspi1;

void MX_SPI1_Init(){
    hspi1.Instance = SPI1;
    hspi1.Init.Mode = SPI_MODE_SLAVE;
    hspi1.Init.Direction = SPI_DIRECTION_2LINES;
    hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
    hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
    hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
    hspi1.Init.NSS = SPI_NSS_HARD_INPUT;
    hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
    hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
    hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
    hspi1.Init.NSSPMode =  SPI_NSS_PULSE_DISABLE;

    HAL_SPI_Init(&hspi1);
}

void HAL_SPI_MspInit(SPI_HandleTypeDef* hspi){
    GPIO_InitTypeDef gpioInit;
    if(hspi -> Instance == SPI1){
        /** Enable GPIO and SPI Clocks **/
        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_SPI1_CLK_ENABLE();


        /** SPI1 GPIO Configuration
         *  PA4 ----------> SPI1_NSS 
         *  PA5 ----------> SPI1_SCK 
         *  PA6 ----------> SPI1_MISO 
         *  PA7 ----------> SPI1_MOSI
         */

        gpioInit.Pin = GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
        gpioInit.Pull = GPIO_NOPULL;
        gpioInit.Mode = GPIO_MODE_AF_PP;
        gpioInit.Speed = GPIO_SPEED_FREQ_MEDIUM;
        gpioInit.Alternate = GPIO_AF0_SPI1;
        HAL_GPIO_Init(GPIOA, &gpioInit);
    }
}

void HAL_SPI_MspDeInit(SPI_HandleTypeDef* hspi){
    if(hspi -> Instance == SPI1){

        /** SPI1 Clock Disable **/
        __HAL_RCC_SPI1_CLK_DISABLE();


        /** SPI1 GPIO Configuration
         *  PA4 ----------> CS
         *  PA5 ----------> SPI1_SCK 
         *  PA6 ----------> SPI1_MISO 
         *  PA7 ----------> SPI1_MOSI
         */
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);
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
