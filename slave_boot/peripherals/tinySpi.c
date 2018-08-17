#include "stm32f0xx.h"
#include "tinySpi.h"

void MX_SPI1_Init(){
    /* Init the low level hardware : GPIO, CLOCK, NVIC... */
    SPI1_LL_Init();

    /* Set FIFO threshold to 8-bits */
    SPI1->CR2 |= SPI_CR2_FRXTH;

    /* Set 8-bit transfers */
    SPI1->CR2 = SPI_CR2_DS_2 | SPI_CR2_DS_1 | SPI_CR2_DS_0;

    /* Enable SPI as Slave and Hardware NSS*/
    SPI1->CR1 = SPI_CR1_SPE;
}

void SPI1_LL_Init(){
    /** Enable GPIO and SPI Clocks **/
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

    /** SPI1 GPIO Configuration
     *  (Alternate Function, Push Pull, High Speed, No Pull, AF0_SPI1)
     *  PA4 ----------> SPI1_NSS
     *  PA5 ----------> SPI1_SCK
     *  PA6 ----------> SPI1_MISO
     *  PA7 ----------> SPI1_MOSI
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

#define MAX_COUNT 100000u
wrError_t writeResponse(uint8_t *data, uint16_t length){
    uint32_t tickstart = 0u;

    uint8_t* pData = data;
    uint16_t count = length;

    /** Write first piece of data to DR since in slave mode **/
    if (count > 1u)
    {
        /* write on the data register in packing mode */
        SPI1->DR = *((uint16_t *)pData);
        pData += sizeof(uint16_t);
        count -= 2u;
    }
    else
    {
        SPI1->DR = (*pData++);
        count--;
    }

    while (count > 0u)
    {
        /* Wait until TXE flag is set to send data */
        if (SPI1->SR & SPI_SR_TXE)
        {
            if (count > 1u)
            {
                /* write on the data register in packing mode */
                SPI1->DR = *((uint16_t *)pData);
                pData += sizeof(uint16_t);
                count -= 2u;
            }
            else
            {
            SPI1->DR = (*pData++);
            count--;
            }
        }
        else
        {
            /* Timeout management */
            if(tickstart++ == MAX_COUNT){
                return WR_ERR;
            }
        }
    }

    /* Clear overrun flag in 2 Lines communication mode because received is not read */
    __SPI_CLEAR_OVRFLAG(SPI1);

    return WR_OK;
}


wrError_t readResponse(uint8_t *data, uint16_t length){
    uint32_t tickstart = 0u;
    uint16_t count = length;

    /* set fiforxthresold according the reception data length: 8bit */
    SPI1->CR2 |= SPI_CR2_FRXTH;

    /* Check if the SPI is already enabled */
    if ((SPI1->CR1 & SPI_CR1_SPE) != SPI_CR1_SPE)
    {
    /* Enable SPI peripheral */
    SPI1->CR1 |= SPI_CR1_SPE;
    }

    /* Receive data in 8 Bit mode */
    /* Transfer loop */
    while (count > 0U)
    {
        /* Check the RXNE flag */
        if(SPI1->SR & SPI_SR_RXNE)
        {
            /* read the received data */
            *data = SPI1->DR;
            data += sizeof(uint8_t);
            count--;

            tickstart = 0;
        }
        else
        {
            if(tickstart++ == MAX_COUNT){
                return WR_ERR;
            }
        }
    }

  return WR_OK;
}
