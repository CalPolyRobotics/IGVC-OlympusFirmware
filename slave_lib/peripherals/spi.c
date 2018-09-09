#include <stdbool.h>
#include "stm32f0xx.h"
#include "spi.h"
#include "systemClock.h"

void MX_COMMS_SPI_Init(){
    /* Init the low level hardware : GPIO, CLOCK */
    COMMS_SPI_LL_Init();

    /* Set 8-bit transfers & 8-bit FIFO threshold */
    COMMS_SPI->CR2 = (SPI_CR2_DS_2 | SPI_CR2_DS_1 | SPI_CR2_DS_0) |
                SPI_CR2_FRXTH;

    /* Enable SPI as Slave and Hardware NSS*/
    COMMS_SPI->CR1 = SPI_CR1_SPE;
}

void COMMS_SPI_LL_Init(){
    /* Enable GPIO and SPI Clocks */
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

    /** COMMS_SPI GPIO Configuration
     *  (Alternate Function, Push Pull, High Speed, No Pull, AF0_COMMS_SPI)
     *  PA4 ----------> COMMS_SPI_NSS
     *  PA5 ----------> COMMS_SPI_SCK
     *  PA6 ----------> COMMS_SPI_MISO
     *  PA7 ----------> COMMS_SPI_MOSI
     */

    GPIOA->OSPEEDR |=  GPIO_OSPEEDR_OSPEEDR4_Msk |
                       GPIO_OSPEEDR_OSPEEDR5_Msk |
                       GPIO_OSPEEDR_OSPEEDR6_Msk |
                       GPIO_OSPEEDR_OSPEEDR7_Msk;

    GPIOA->AFR[0] |=  (0x00u << GPIO_AFRL_AFSEL4_Pos) |
                      (0x00u << GPIO_AFRL_AFSEL5_Pos) |
                      (0x00u << GPIO_AFRL_AFSEL6_Pos) |
                      (0x00u << GPIO_AFRL_AFSEL7_Pos);

    GPIOA->MODER |= GPIO_MODER_MODER4_1 |
                    GPIO_MODER_MODER5_1 |
                    GPIO_MODER_MODER6_1 |
                    GPIO_MODER_MODER7_1;
}

wrError_t writeResponse(uint8_t *data, uint16_t length, uint32_t timeout){
    uint8_t* pData = data;
    uint16_t count = length;

    /* Write first piece of data to DR since in slave mode */
    if (count > 1u)
    {
        /* write on the data register in packing mode */
        COMMS_SPI->DR = *((uint16_t *)pData);
        pData += sizeof(uint16_t);
        count -= 2u;
    }
    else
    {
        COMMS_SPI->DR = (*pData++);
        count--;
    }

    uint32_t finish = systickCount + timeout;
    while (count > 0u)
    {
        /* Timeout */
        if(systickCount > finish)
        {
            return WR_ERR;
        }

        /* Wait until TXE flag is set to send data */
        if (COMMS_SPI->SR & SPI_SR_TXE)
        {
            if (count > 1u)
            {
                /* write on the data register in packing mode */
                COMMS_SPI->DR = *((uint16_t *)pData);
                pData += sizeof(uint16_t);
                count -= 2u;
            }
            else
            {
                COMMS_SPI->DR = (*pData++);
                count--;
            }
        }
    }


    /* Clear overrun flag in 2 Lines communication mode because received is not read */
    __SPI_CLEAR_OVRFLAG(COMMS_SPI);

    return WR_OK;
}


wrError_t readByte(uint8_t *data, uint32_t timeout){
    bool readData = true;

    /* Write a 0 while reading byte */
    if(COMMS_SPI->SR & SPI_SR_TXE){
        COMMS_SPI->DR = (uint8_t)0x00u;
    }

    /* Receive data in 8 Bit mode */
    uint32_t finish = systickCount + timeout;
    while (readData)
    {
        /* Timeout */
        if(systickCount > finish)
        {
            return WR_ERR;
        }

        /* Check the RXNE flag */
        if(COMMS_SPI->SR & SPI_SR_RXNE)
        {
            /* read the received data */
            *data = COMMS_SPI->DR;
            data += sizeof(uint8_t);
            readData = false;
        }
    }

  return WR_OK;
}
