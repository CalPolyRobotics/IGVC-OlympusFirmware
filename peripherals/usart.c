#include "stm32f2xx_hal_dma.h"

#include "main.h"
#include "usart.h"
#include "gpio.h"
#include "dma.h"
#include "utils/buffer8.h"

#include <stdio.h>

#include "config.h"

static uint8_t txBuffers[COMMS_TX_NUM_BUFFERS][COMMS_TX_BUFFER_SIZE];
static uint8_t rxBuffer[COMMS_RX_BUFFER_SIZE];
static volatile uint32_t txBufferLengths[COMMS_TX_NUM_BUFFERS];
static volatile uint32_t nextTxBuffer;
static volatile uint32_t dmaTxBuffer;
static volatile uint8_t txDmaActive;

static uint32_t rxBufferPos;

static volatile uint8_t shouldServiceTxDma;

static void commsUsartDmaInit(void);
static void startTxDma(void);

// Theory of Operation
//
// This file controls the USART used for USB serial communication.
// It does so using 2 Dmas, one for Tx and another for Rx. Also
// note that none of these function can be called within an interrupt.
// 
//
// Usage:
// 1. Setup the DMA buffer sizes in config.h
//  --Note: For simplicity the DMA Stream numbers are hardcoded
//    to 5 & 7 a places in this code. These should not be changed
//
//    Although the baud rate is configurable in config.h, it is
//    hardcoded in this file because of clock issues.
//
//    COMMS_TX_NUM_BUFFERS should be set to a value greater than 1.
//    Otherwise no data can be written to the output buffer while
//    the Dma is active.
//
// 2. Initialize the USART with commsUsartInit().
//
// 3. Call serviceTxDma() in the main while (1) loop. This function
//    call prepares the Tx Dma to send more data and no more data
//    can be sent until this function is called.
//
// 4. To write data use the usartWrite function.
//
// 5. To read data use either usartGet (blocking) or
//    usartRead (non-blocking).
//
//
// Tx Dma:
// There are (COMMS_TX_NUM_BUFFERS) output buffers each with
// size (COMMS_TX_BUFFER_SIZE). Every time the Tx Dma is enabled
// with startTxDma a single buffer is sent across the USART. When
// the DMA is done transferring data the flag (shouldServiceTxDma)
// gets sets. The next time serviceTxDma() is called the buffer
// used for transmission is cleared and a new transmission is
// started on the next buffer if more data is available.
//
// Data is added to the output buffers through the usartWrite
// function. This function takes in a pointer and a data length.
// It copies the data into the next available output buffer. After
// the data is copied in the buffer, if no Tx Dma transmissions
// are occurring, a new transmission is started.
//
//
// Rx Dma:
// There is a single input buffer for the Rx Dma. The Dma is configure
// in circular mode making the input buffer act like a circular buffer.
// Reading a byte increments (rxBufferPos) through the circular buffer.
// The head of the circular buffer is calculated through reading the
// current state of the Rx Dma and calculating its next location.



// Initialize the USART 1 peripheral for serial comms
void commsUsartInit(void)
{
    // For some reason printf starts out as line buffered.
    setbuf(stdout, NULL);

    GPIO_InitTypeDef GPIO_InitStruct;

    GPIO_InitStruct.Pin = GPIO_USART_TX|GPIO_USART_RX;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = GPIO_USART_AF;
    HAL_GPIO_Init(GPIO_USART_PORT, &GPIO_InitStruct);

    COMMS_USART->CR1 = USART_CR1_TE | // Enable the Transmitter
                       USART_CR1_RE;  // Enable the Receiver

    COMMS_USART->CR2 = 0; // Nothing to enable in CR2

    COMMS_USART->CR3 = USART_CR3_DMAT | // Enable the DMA for Transmission
                       USART_CR3_DMAR; // Enable the DMA for Reception

    // This formula should calculate baud rate correctly, but the APB2 CLK
    // is next exactly 30 MHz, leading to an incorrect baud rate. The number
    // 0x209 sets the baud rate to 115200 bits/s and was determined experimentally
    //COMMS_USART->BRR = APB2_CLK_FREQ / (COMMS_BAUDRATE); //Calculate Baudrate
    COMMS_USART->BRR = 0x209;

    // Initialize the DMAs used for USART
    commsUsartDmaInit();

    COMMS_USART->CR1 |= USART_CR1_UE; // Enable the Usart
}

// Initialize the DMAs used for Tx and Rx.
static void commsUsartDmaInit(void)
{
    // Configure Tx DMA

    COMMS_TX_DMA_STREAM->CR = COMMS_TX_DMA_CHANNEL | // The DMA Channel to use
                              DMA_SxCR_MINC |  // Increment the memory address
                              DMA_SxCR_DIR_0 | // Memory -> Peripheral mode
                              DMA_SxCR_TCIE; // Enable the Transfer Complete interrupt

    // Clear all interrupt bits
    COMMS_TX_DMA->HIFCR = DMA_HIFCR_CTCIF7 |
                          DMA_HIFCR_CHTIF7 |
                          DMA_HIFCR_CTEIF7 |
                          DMA_HIFCR_CDMEIF7 |
                          DMA_HIFCR_CFEIF7;


    COMMS_TX_DMA_STREAM->PAR = (uint32_t)&COMMS_USART->DR; // Assign peripheral address to USART Data Register

    NVIC_EnableIRQ(COMMS_TX_DMA_IRQ);

    // Configure Rx DMA
    
    COMMS_RX_DMA_STREAM->CR = COMMS_RX_DMA_CHANNEL | // The DMA Channel to use
                              DMA_SxCR_CIRC | // Enable Circular Mode
                              DMA_SxCR_PL_1 | // High priority level
                              DMA_SxCR_MINC | // Increment the memory address
                              DMA_SxCR_TEIE; // Enable the TRansfer Error interrupt

    COMMS_RX_DMA_STREAM->PAR = (uint32_t)&COMMS_USART->DR; // Assign peripheral address to USART Data Register
    COMMS_RX_DMA_STREAM->M0AR = (uint32_t)rxBuffer; // Start the DMA at rxBuffer
    rxBufferPos = (uint32_t)rxBuffer;     // Used to count values in circular buffer
    COMMS_RX_DMA_STREAM->NDTR = sizeof(rxBuffer); // Size is the size of the array

    COMMS_RX_DMA_STREAM->CR |= DMA_SxCR_EN; // Enable the Rx DMA
}

// Resets the Tx Dma for new transmission. If there is more data to send
// start a new transmission.
void serviceTxDma()
{
    if (shouldServiceTxDma)
    {
        // Clear the the buffer that was just written
        txBufferLengths[dmaTxBuffer] = 0;

        dmaTxBuffer++;
        if (dmaTxBuffer == COMMS_TX_NUM_BUFFERS)
        {
            dmaTxBuffer = 0;
        }

        if (txBufferLengths[dmaTxBuffer] > 0)
        {
            startTxDma();
        } else {
            txDmaActive = 0;
        }

        shouldServiceTxDma = 0;
    }
}

// Enables the Tx Dma and outputs a single buffer over the USART
static void startTxDma(void)
{
    COMMS_TX_DMA_STREAM->M0AR = (uint32_t)txBuffers[dmaTxBuffer];
    COMMS_TX_DMA_STREAM->NDTR = txBufferLengths[dmaTxBuffer];

    COMMS_TX_DMA_STREAM->CR |= DMA_SxCR_EN;
    
    txDmaActive = 1;

    // Increase the next buffer if necessary
    if (nextTxBuffer == dmaTxBuffer)
    {
        nextTxBuffer++;
        if (nextTxBuffer == COMMS_TX_NUM_BUFFERS)
        {
            nextTxBuffer = 0;
        }
    }
}

// Write a single byte into the Tx send buffer
void usartPut(uint8_t data)
{
    usartWrite((char*)&data, 1);
}

// Write size bytes from data into the Tx send buffer
// If the Tx DMA is not sending data, enable the Tx DMA
// and send the buffer
void usartWrite(char* data, uint16_t size)
{
    uint32_t bufferSpace;
    uint32_t bytesToWrite;
    
    if (size > 0)
    {
        while (size > 0)
        {
            // Calculate the number of bytes to write
            bufferSpace = COMMS_TX_BUFFER_SIZE - txBufferLengths[nextTxBuffer];
            bytesToWrite = size < bufferSpace ? size : bufferSpace;

            // Write the bytes to the end of the buffer
            memcpy(&txBuffers[nextTxBuffer][txBufferLengths[nextTxBuffer]], data, bytesToWrite);

            // Decrease the remaining space in the buffer
            txBufferLengths[nextTxBuffer] += bytesToWrite;
            if (txBufferLengths[nextTxBuffer] == COMMS_TX_BUFFER_SIZE)
            {
                // If the buffer is full go to the next buffer
                nextTxBuffer++;
                if (nextTxBuffer == COMMS_TX_NUM_BUFFERS)
                {
                    nextTxBuffer = 0;
                }

                if (nextTxBuffer == dmaTxBuffer)
                {
                    // Tx Buffers Overrun. Drop the rest of the bytes
                    size = 0;
                }
            }

            // Decrease the remaining size
            size -= bytesToWrite;
            data += bytesToWrite;
        }

        // If no tranmission is occuring, send the current buffer
        if (txDmaActive == 0)
        {
            startTxDma();
        }
    }
}

// Gets the DMA's current position in the circular buffer
static uint32_t getRxDmaPosition(void)
{
    uint32_t dmaStart = COMMS_RX_DMA_STREAM->M0AR;
    uint32_t offset = sizeof(rxBuffer) - COMMS_RX_DMA_STREAM->NDTR;

    return dmaStart + offset;
}

// Calculates the number of bytes left in the circular buffer
static uint32_t getRxBytesAvailable(void)
{
    uint32_t dmaPosition = getRxDmaPosition();

    if (rxBufferPos <= dmaPosition)
    {
        return dmaPosition - rxBufferPos;
    } else {
        return sizeof(rxBuffer) - (rxBufferPos - dmaPosition);
    }
}

// Read a single character from the USART
// Blocks until a character is received
char usartGet(void)
{
    char ret;

    while (rxBufferPos == getRxDmaPosition())
    {}

    ret = *(uint8_t*)rxBufferPos;
    rxBufferPos++;
    if ((rxBufferPos - (uint32_t)rxBuffer) == sizeof(rxBuffer))
    {
        rxBufferPos = (uint32_t)rxBuffer;
    }

    return ret;
}

// Reads up to len bytes into data. Returns the number of actual bytes read.
// Does not block
uint32_t usartRead(uint8_t* data, uint32_t len)
{
    uint32_t readLen;
    uint32_t haveBytes = getRxBytesAvailable();

    readLen = (len < haveBytes) ? len : haveBytes;
    
    if (readLen != 0)
    {
        // If the valid data has not wrapped around the end of the buffer
        if (rxBufferPos < COMMS_RX_DMA_STREAM->M0AR)
        {
            memcpy(data, (uint8_t*)rxBufferPos, readLen);
            rxBufferPos += readLen;
        } else {
            uint32_t bytesUntilEnd = ((uint32_t)rxBuffer + sizeof(rxBuffer)) - rxBufferPos;

            // If the number of bytes to grab is still in consecutive memory
            if (bytesUntilEnd > readLen)
            {
                memcpy(data, (uint8_t*)rxBufferPos, readLen);
                rxBufferPos += readLen;
            } else { // The data to grab has wrapped. Use two memcpy's
                // Grab the data until the end of the array
                memcpy(data, (uint8_t*)rxBufferPos, bytesUntilEnd);
                data += bytesUntilEnd;

                // Grab the remaining bytes from the start of the array
                memcpy(data, (uint8_t*)rxBuffer, readLen - bytesUntilEnd);
                rxBufferPos = (uint32_t)rxBuffer + (readLen - bytesUntilEnd);
            }
        }
    }

    return readLen;
}

// When the Tx DMA has completed signal the Tx USART to be serviced
void COMMS_TX_DMA_ISR()
{
    shouldServiceTxDma = 1;

    COMMS_TX_DMA->HIFCR = DMA_HIFCR_CTCIF7;
}
