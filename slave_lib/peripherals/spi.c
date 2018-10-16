#include <stdbool.h>
#include "stm32f0xx.h"
#include "spi.h"
#include "systemClock.h"
#include "string.h"

#define COMMS_SPI_IRQn SPI1_IRQn

#define TX_BUFFER_SIZE ((uint32_t)256u)
#define RX_BUFFER_SIZE ((uint32_t)256u)

uint8_t txBuffer[TX_BUFFER_SIZE];
volatile uint8_t txBufferStart = 0;
volatile uint8_t txBufferLen = 0;

uint8_t rxBuffer[RX_BUFFER_SIZE];
volatile uint8_t rxBufferStart = 0;
volatile uint8_t rxBufferLen = 0;

static void COMMS_SPI_Init()
{
    /* Set 8-bit transfers & 8-bit FIFO threshold */
    COMMS_SPI->CR2 = (SPI_CR2_DS_2 | SPI_CR2_DS_1 | SPI_CR2_DS_0) |
                SPI_CR2_FRXTH;

    /* Enable Interrupts */
    COMMS_SPI->CR2 |= SPI_CR2_TXEIE | SPI_CR2_RXNEIE | SPI_CR2_ERRIE;
   
    NVIC_EnableIRQ(COMMS_SPI_IRQn);

    /* Enable SPI as Half-Duplex Slave and Hardware NSS*/
    COMMS_SPI->CR1 = SPI_CR1_BIDIMODE | SPI_CR1_SPE;
}

void MX_COMMS_SPI_Init(){
    /* Init the low level hardware : GPIO, CLOCK */
    COMMS_SPI_LL_Init();

    /* Init the peripheral */
    COMMS_SPI_Init();
}

void COMMS_SPI_LL_Init(){
    /* Enable GPIO and SPI Clocks */
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

    /** COMMS_SPI GPIO Configuration
     *  (Alternate Function, Push Pull, High Speed, No Pull, AF0_COMMS_SPI)
     *  PA4 ----------> COMMS_SPI_NSS
     *  PA5 ----------> COMMS_SPI_SCK
     *  PA6 ----------> COMMS_SPI_MISO
     *
     *  PB0 ----------> COMMS_SPI_INT
     */

    GPIOA->OSPEEDR |=  GPIO_OSPEEDR_OSPEEDR4_Msk |
                       GPIO_OSPEEDR_OSPEEDR5_Msk |
                       GPIO_OSPEEDR_OSPEEDR6_Msk;

    GPIOA->AFR[0] |=  (0x00u << GPIO_AFRL_AFSEL4_Pos) |
                      (0x00u << GPIO_AFRL_AFSEL5_Pos) |
                      (0x00u << GPIO_AFRL_AFSEL6_Pos);

    GPIOA->MODER |= GPIO_MODER_MODER4_1 |
                    GPIO_MODER_MODER5_1 |
                    GPIO_MODER_MODER6_1;

    GPIOB->OSPEEDR |= GPIO_OSPEEDR_OSPEEDR0_Msk;
    GPIOB->MODER |= GPIO_MODER_MODER0_0;
    GPIOB->ODR &= ~GPIO_ODR_0;
}

void clearSPIInt()
{
    GPIOB->ODR &= ~GPIO_ODR_0;
}

void setSPIInt()
{
    GPIOB->ODR |= GPIO_ODR_0;
}

void runResponse()
{
    setSPIInt();
    COMMS_SPI->CR1 |= SPI_CR1_BIDIOE;

    while(!(COMMS_SPI->SR & SPI_SR_BSY));
    while(COMMS_SPI->SR & SPI_SR_BSY);

    COMMS_SPI->CR1 &= ~SPI_CR1_BIDIOE;
    clearSPIInt();
}


wrError_t writeResponse(uint8_t status, uint8_t *data, uint8_t dataLen)
{
    if(txBufferLen + sizeof(status) + dataLen > TX_BUFFER_SIZE)
    {
        return WR_BUFF_FULL;
    }

    /* Disable interrupt to protect shared data */
    COMMS_SPI->CR2 &= ~SPI_CR2_TXEIE;

    uint16_t respLen = sizeof(status) + dataLen;

    uint32_t writeIdx = (txBufferStart + txBufferLen) % TX_BUFFER_SIZE;
    uint16_t firstLen = writeIdx + respLen <= TX_BUFFER_SIZE ? respLen : TX_BUFFER_SIZE - writeIdx;

    /* Write idx to end */
    memcpy(txBuffer + writeIdx, &status, sizeof(status));
    memcpy(txBuffer + writeIdx + sizeof(status), data, firstLen - sizeof(status));

    if(firstLen != respLen)
    {
        /* Write overflow data */
        memcpy(txBuffer, data + firstLen, respLen - firstLen);
    }

    txBufferLen += respLen;

    COMMS_SPI->CR2 |= SPI_CR2_TXEIE;

    runResponse();

    return WR_OK;
}


wrError_t readByte(uint8_t *data)
{
    if(rxBufferLen == 0u)
    {
        return WR_NO_DATA;
    }

    /* Disable interrupt to protect shared data */
    COMMS_SPI->CR2 &= ~SPI_CR2_RXNEIE;

    *data = rxBuffer[rxBufferStart];

    rxBufferLen--;
    rxBufferStart = (rxBufferStart + 1u) % RX_BUFFER_SIZE;

    COMMS_SPI->CR2 |= SPI_CR2_RXNEIE;

    return WR_OK;
}

void SPI1_IRQHandler()
{
    if(SPI1->SR & SPI_SR_RXNE)
    {
        uint32_t idx = (rxBufferStart + rxBufferLen) % RX_BUFFER_SIZE;
        rxBuffer[idx] = (volatile uint8_t)SPI1->DR;
        rxBufferLen++;
    }

    if(SPI1->SR & SPI_SR_TXE)
    {
        if(txBufferLen)
        {
            *((volatile uint8_t *)&SPI1->DR) = txBuffer[txBufferStart];

            txBufferStart = (txBufferStart + 1u) % TX_BUFFER_SIZE;
            txBufferLen--;
        }
        else
        {
            /* Disable interrupt - there is nothing to write */
            COMMS_SPI->CR2 &= ~SPI_CR2_TXEIE;
        }
    }

    if(SPI1->SR & SPI_SR_FRE)
    {
        SPI1->SR &= ~SPI_SR_FRE;
    }

    if(SPI1->SR & SPI_SR_OVR)
    {
        SPI1->SR &= ~SPI_SR_OVR;
    }

    if(SPI1->SR & SPI_SR_MODF)
    {
        SPI1->SR &= ~SPI_SR_MODF;
    }

    if(SPI1->SR & SPI_SR_CRCERR)
    {
        SPI1->SR &= ~SPI_SR_CRCERR;
    }
}
