#include "main.h"

/**
      (#) Declare a SPI_HandleTypeDef handle structure, for example:
          SPI_HandleTypeDef  hspi;

      (#)Initialize the SPI low level resources by implementing the HAL_SPI_MspInit() API:
          (##) Enable the SPIx interface clock
          (##) SPI pins configuration
              (+++) Enable the clock for the SPI GPIOs
              (+++) Configure these SPI pins as alternate function push-pull
          (##) NVIC configuration if you need to use interrupt process
              (+++) Configure the SPIx interrupt priority
              (+++) Enable the NVIC SPI IRQ handle
          (##) DMA Configuration if you need to use DMA process
              (+++) Declare a DMA_HandleTypeDef handle structure for the transmit or receive Stream/Channel
              (+++) Enable the DMAx clock
              (+++) Configure the DMA handle parameters
              (+++) Configure the DMA Tx or Rx Stream/Channel
              (+++) Associate the initialized hdma_tx handle to the hspi DMA Tx or Rx handle
              (+++) Configure the priority and enable the NVIC for the transfer complete interrupt on the DMA Tx or Rx Stream/Channel
              **/

void init_spi(){
    __HAL_RCC_SPI1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    //          (+++) Configure these SPI pins as alternate function push-pull

    GPIO_InitTypeDef gpioInit;

    gpioInit.Pin = GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
    gpioInit.Pull = GPIO_NOPULL;
    gpioInit.Mode = GPIO_MODE_OUTPUT_PP;
    gpioInit.Speed = GPIO_SPEED_FREQ_MEDIUM;
    gpioInit.Alternate = GPIO_AF0_SPI1;

    HAL_GPIO_Init(GPIOA, &gpioInit);

    SPI_InitTypeDef ispi;
    ispi.Mode = SPI_MODE_MASTER;
    ispi.Direction = SPI_DIRECTION_2LINES;
    ispi.DataSize = SPI_DATASIZE_16BIT;
    ispi.CLKPolarity = SPI_POLARITY_LOW;
    ispi.CLKPhase = SPI_PHASE_1EDGE;
    ispi.NSS = SPI_NSS_HARD_OUTPUT;
    ispi.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
    ispi.FirstBit = SPI_FIRSTBIT_MSB;
    ispi.TIMode = SPI_TIMODE_DISABLE;
    ispi.NSSPMode =  SPI_NSS_PULSE_DISABLE;


    SPI_HandleTypeDef hspi;
    hspi.Instance = SPI1;
    hspi.Init = ispi;

    HAL_SPI_Init(&hspi);

    uint8_t data[] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k'};
    while(1){
        HAL_SPI_Transmit(&hspi, data, 12, 1000);
        HAL_Delay(100);
        GPIOB -> ODR ^= GPIO_PIN_3;
    }
}
