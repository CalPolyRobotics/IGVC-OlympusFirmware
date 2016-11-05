
#include <stdio.h>
#include <stdbool.h>

#include "config.h"
#include "i2c.h"
#include "gpio.h"
#include "buffer8.h"
#include "usart.h"

typedef enum {
    I2C_TYPE_TX,
    I2C_TYPE_RX
} I2C_TransferType;

// A single I2C transaction request
typedef struct {
    uint8_t ready;
    uint8_t numBytes;
    uint8_t addr;
    I2C_TransferType type;
    union {
        i2cTxCallback txCallback;
        i2cRxCallback rxCallback;
    };
    void* cParams;
} I2CTransaction;

// List of future transactions
static I2CTransaction transactionList[I2C_MAX_NUM_TRANSACTIONS];

// Buffer to hold tx data for future transactions. Treated as a fifo
static uint8_t i2cTxDataBuffer[I2C_TRANSACTION_TX_BUF_SIZE];

// Buffer to hold rx data for the current transaction
static uint8_t i2cRxDataBuffer[I2C_TRANSACTION_RX_BUF_SIZE];

// Index into i2cTxDataBuffer
static uint32_t i2cRxDataBufferIdx;

// Fifo struct used to treat i2cTxDataBuffer as a fifo
static buffer8_t i2cTxDataFifo;

// Bufffer to hold tx data for the current transaction.
// Data from i2cTxDataBuffer is copied into this buffer
// before the transaction starts
static uint8_t i2cTxNextTransferBuffer[I2C_TRANSACTION_TX_BUF_SIZE];

// Index into i2cTxNextTransferBuffer
static uint32_t i2cTxNextTransferIdx;

// The next free transaction number
static uint32_t freeTransaction;

// The current transaction, or the next transaction that will occur
static volatile uint32_t nextTransaction;

// Flag to indicate whether the i2c is active or not
static volatile uint8_t i2cRunning;

// Flag to indicate if i2c should be serviced
static volatile uint8_t shouldServiceI2C;

// Status of the current transaction
static volatile uint8_t i2cStatus;

// Number of bytes left in the current transaction
static volatile uint8_t i2cBytesLeft;

// Theory of Operation
//
// This file control the I2C peripheral which talks to
// several slave devices on the control board. For a
// list of all the slave devices, see the control board
// documentation.
//
// Usage:
// 1. Setup the buffer sizes in config.h
// 
// 2. Initialize the I2C peripheral with i2cInit()
//
// 3. Call serviceI2C() in the main while (1) loop.
//    This function must be called between I2C transactions.
//
// 4. To write data to an I2C slave use the i2cAddTxTransaction(...)
//    function.
//
// 5. To read data from an I2C slave use the i2cAddRxTransaction(...)
//    function.
//
// Callbacks:
// After a transaction has been completed a user provided callback
// will be called. This function is called whether or not the transaction
// was successful. Note that it is possible to start a new I2C transaction
// from inside another transaction's callback.
//
// Tx Data Callback:
//  (*i2cTxCallback)(void* parameters, I2CStatus status)
//
// The (parameters) parameter is a void* passed direction from the
// i2cAddTxTransaction function call. This allows the user to use a
// single callback for multiple purposes.
// The (status) parameter indicates the status of the transaction.
// There are three possible status: I2C_ACK, I2C_NACK, I2C_ERR.
// Only I2C_ACK indicates that the transaction was successful. An I2C_NACK
// status means that no slave responded to the address and the I2C_ERR
// status means that some sort of bus error occured during the transaction.
//
// Rx Data Callback:
//  (*i2cRxCallback)(void* parameters,
//                   uint8_t* rxData,
//                   uint32_t numBytes,
//                   I2CStatus status)
//
// See "Tx Data Callback" for information on (parameters) and (status).
// The (rxData) parameter is a pointer to the recieved data. The data
// is only valid during this callback so any data that needs to be kept
// must be copied at this time.
// The (numBytes) parameter contains the number of bytes actually recieved
// by the slave.


// Initialize the I2C peripheral and the SDA/SCL pins
void i2cInit()
{
    GPIO_InitTypeDef GPIO_InitStruct;

    GPIO_InitStruct.Pin = GPIO_I2C_SCL | GPIO_I2C_SDA;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = GPIO_I2C_AF;
    HAL_GPIO_Init(GPIO_I2C_PORT, &GPIO_InitStruct);

    I2C2->CR1 = 0;

    I2C2->CR2 = I2C_CR2_ITEVTEN | // Enable event interrupts
                I2C_CR2_ITERREN | // Enable error event interrupts
                (0x1E);           // APB Clk speed is 30 MHz (max speed)

    I2C2->CCR = 150;    // Slow speed and high and low period times
    I2C2->TRISE = 0x1F; // Rist time for slow I2C. This needs to change for fast mode

    (void) I2C2->SR1; // Clear some flags
    (void) I2C2->SR2; // Clear some flags

    I2C2->CR1 |= I2C_CR1_PE; // Enable the peripheral

    NVIC_EnableIRQ(I2C2_ER_IRQn);
    NVIC_EnableIRQ(I2C2_EV_IRQn);

    buffer8_init(&i2cTxDataFifo, i2cTxDataBuffer, I2C_TRANSACTION_TX_BUF_SIZE);
}

// Starts the next I2C transatcion
static void startI2CTransfer()
{
    // Setup static globals
    i2cRunning = 1;
    i2cBytesLeft = transactionList[nextTransaction].numBytes;

    if (transactionList[nextTransaction].type == I2C_TYPE_TX)
    {
        // Copy tx data into i2cTxNextTransferBuffer from the txDataFifo
        uint32_t i;
        for (i = 0; i < transactionList[nextTransaction].numBytes; i++)
        {
            i2cTxNextTransferBuffer[i] = buffer8_get(&i2cTxDataFifo);
        }
        i2cTxNextTransferIdx = 0;
    } else {
        // On an Rx transaction, always ACK
        I2C2->CR1 |= I2C_CR1_ACK;
    }

    // Start the I2C transaction
    I2C2->CR1 |= I2C_CR1_START;
}

// Add an Rx transaction to the end of the transaction list
//
// addr: The 7-bit slave address (excludes R/W bit)
// numBytes: The number of bytes to read
// callback: The function to call when the transaction has completed or NULL
//           Note: The only way to copy the data from a transaction is
//                 through the use of an rx callback. Therefore, if no
//                 callback is supplied the recieve data is thrown away
// parameters: A void* to pass to the callback
//
// return: true if the transaction was added
//         false if the transaction could not be added
uint8_t i2cAddRxTransaction(uint8_t addr, uint32_t numBytes, i2cRxCallback callback, void* parameters)
{
    if (transactionList[freeTransaction].ready)
    {
        return false;
    }

    // Setup the transaction struct
    transactionList[freeTransaction].ready = 1;
    transactionList[freeTransaction].numBytes = numBytes;
    transactionList[freeTransaction].addr = addr;
    transactionList[freeTransaction].type = I2C_TYPE_RX;
    transactionList[freeTransaction].rxCallback = callback;
    transactionList[freeTransaction].cParams = parameters;

    // Move the freeTransaction idx
    freeTransaction++;
    if (freeTransaction == I2C_MAX_NUM_TRANSACTIONS)
    {
        freeTransaction = 0;
    }

    // Start the transaction if I2C is not currently running
    if (!i2cRunning)
    {
        startI2CTransfer();
    }

    return true;
}

// Add a Tx transaction to the end of the transaction list
//
// addr: The 7-bit slave address (excludes R/W bit)
// txData: A pointer to the data to transfer. Data is copied from this buffer during this function call
// numBytes: The number of bytes to transfer
// callback: The function to call when the transaction has completed or NULL
// parameters: A void* to pass to the callback
//
// return: true if the transaction was added
//         false if the transaction could not be added
//
uint8_t i2cAddTxTransaction(uint8_t addr, uint8_t* txData, uint32_t numBytes, i2cTxCallback callback, void* parameters)
{
    if (transactionList[freeTransaction].ready)
    {
        return false;
    }

    // Setupt the transaction struct
    transactionList[freeTransaction].ready = 1;
    transactionList[freeTransaction].numBytes = numBytes;
    transactionList[freeTransaction].addr = addr;
    transactionList[freeTransaction].type = I2C_TYPE_TX;
    transactionList[freeTransaction].txCallback = callback;
    transactionList[freeTransaction].cParams = parameters;

    // Copy the txData to the fifo
    // TODO: Only copy data if we are not going to start the transaction immediatly
    buffer8_write(&i2cTxDataFifo, txData, numBytes);

    // Move the freeTransaction idx
    freeTransaction++;
    if (freeTransaction == I2C_MAX_NUM_TRANSACTIONS)
    {
        freeTransaction = 0;
    }

    // Start the transaction if I2C is not currently running
    if (!i2cRunning)
    {
        startI2CTransfer();
    }

    return true;
}

// Service the I2C peripheral
// Should be called in the main loop
// Must be called after an I2C transaction has finished
void serviceI2C()
{
    uint8_t lastTransaction;
    I2CStatus lastStatus;

    if (shouldServiceI2C)
    {
        // Wait until the I2C is not busy (Stop bit has been sent)
        // before finishing the transaction
        // TODO: The callbacks can be called before this if statement
        //       is true
        if(!(I2C2->SR2 & I2C_SR2_BUSY))
        {
            // Clear the STOP bit
            I2C2->CR1 &= ~(I2C_CR1_STOP);
            I2C2->CR1 &= ~(I2C_CR1_POS);

            shouldServiceI2C = 0;
            lastTransaction = nextTransaction;
            lastStatus = i2cStatus;


            if (i2cStatus == I2C_ACK)
            {
                // Transaction successful
                if (transactionList[nextTransaction].numBytes == 1)
                {
                    // This should only occur if a single byte was read
                    // See the reference manual for more information
                    i2cRxDataBuffer[i2cRxDataBufferIdx] = I2C2->DR;
                    i2cRxDataBufferIdx++;
                }
            }

            // Reset I2C for the next transaction
            i2cRxDataBufferIdx = 0;
            i2cStatus = I2C_ACK;

            // Increase the nextTransaction index
            nextTransaction++;
            if (nextTransaction == I2C_MAX_NUM_TRANSACTIONS)
            {
                nextTransaction = 0;
            }

            if (transactionList[lastTransaction].type == I2C_TYPE_TX)
            {
                // Call the Tx callback if it exists
                if (transactionList[lastTransaction].txCallback)
                {
                    transactionList[lastTransaction].txCallback(
                            transactionList[lastTransaction].cParams,
                            lastStatus);
                }
            } else {
                // Call the Rx callback if it exists
                if (transactionList[lastTransaction].rxCallback)
                {
                    transactionList[lastTransaction].rxCallback(
                            transactionList[lastTransaction].cParams,
                            i2cRxDataBuffer,
                            i2cRxDataBufferIdx,
                            lastStatus);
                }
            }

            // Cleanup the transaction struct
            transactionList[lastTransaction].ready = 0;

            // Free to start a new transaction
            i2cRunning = 0;

            // Check to see if a new transaction is ready
            if (transactionList[nextTransaction].ready == 1)
            {
                startI2CTransfer();
            }
        }
    }
}

// Read a single Rx byte
// A byte is read differently depending 
// on the number of bytes remaining
// See the reference manual for more information
static void readI2CByte()
{
    if (i2cBytesLeft == 3)
    {
        // Turn off ACK with 3 bytes remaining
        I2C2->CR1 &= ~I2C_CR1_ACK;
    } else if (i2cBytesLeft == 2) {
        // Set the STOP bit with two bytes remaining
        I2C2->CR1 |= I2C_CR1_STOP;
        shouldServiceI2C = 1;

        // Read in the last two bytes at the same time
        i2cRxDataBuffer[i2cRxDataBufferIdx] = I2C2->DR;
        i2cRxDataBufferIdx++;
        i2cBytesLeft--;
    }

    // Read in one byte from the peripheral
    i2cRxDataBuffer[i2cRxDataBufferIdx] = I2C2->DR;
    i2cRxDataBufferIdx++;
    i2cBytesLeft--;

}

// Interrupt handler for events
//  Start bit sent (SB)
//  Address sent and ACK received (ADDR)
//  Byte transfer finsihed (BTF)
void I2C2_EV_IRQHandler()
{
    uint32_t i2cStatusReg = I2C2->SR1;

    // Start Bit Sent
    if (i2cStatusReg & I2C_SR1_SB)
    {
        // Send the address (with the R/W bite) after the start bit has been sent
        if (transactionList[nextTransaction].type == I2C_TYPE_TX)
        {
            // Send address with write
            I2C2->DR = transactionList[nextTransaction].addr << 1 | 0;
        } else {
            // Send address with read
            I2C2->DR = transactionList[nextTransaction].addr << 1 | 1;
        }
    }

    // Address Sent
    if (i2cStatusReg & I2C_SR1_ADDR)
    {
        uint32_t i2cStatusReg2 = I2C2->SR2;
        (void) i2cStatusReg2;

        if (transactionList[nextTransaction].type == I2C_TYPE_TX)
        {
            // Used to cover the case where 0 bytes are sent.
            // May be useful for an address scanner
            if (i2cBytesLeft > 0)
            {
                I2C2->DR = i2cTxNextTransferBuffer[i2cTxNextTransferIdx++];
                i2cBytesLeft--;
            } else {
                I2C2->CR1 |= I2C_CR1_STOP;
                shouldServiceI2C = 1;
            }
        } else {

            // Reading I2C data has multiple cases depending on the number of bytes read.
            // Most of this code was taken from the reference manual or the HAL library.
            // The following code handles edge cases for reading only 1 or 2 bytes
            if (i2cBytesLeft == 1)
            {
                i2cBytesLeft--;
                I2C2->CR1 &= ~I2C_CR1_ACK;
                I2C2->CR1 |= I2C_CR1_STOP;
                shouldServiceI2C = 1;
            } else if (i2cBytesLeft == 2)
            {
                I2C2->CR1 &= ~I2C_CR1_ACK;
                I2C2->CR1 |= I2C_CR1_POS;
            }
        }
    }

    // Byte Transfer Finished
    if (i2cStatusReg & I2C_SR1_BTF)
    {

        if (transactionList[nextTransaction].type == I2C_TYPE_TX)
        {
            if (i2cBytesLeft > 0)
            {
                // While there's data left to transfer, transfer it
                I2C2->DR = i2cTxNextTransferBuffer[i2cTxNextTransferIdx++];
                i2cBytesLeft--;
            } else {
                // After the last byte has been sent, stop the transfer
                I2C2->CR1 |= I2C_CR1_STOP;
                shouldServiceI2C = 1;
            }
        } else {
            // For an Rx transaction, see readI2CByte
            readI2CByte();
        }
    }
}

// Interrupt handler for errors
//  Address NACK (AF)
//  Arbitration Lost (ARLO)
//  Bus Error (BERR)
void I2C2_ER_IRQHandler()
{
    // For all errors, set the status to error
    // and end the I2C transaction. This error
    // is then reported to the callback function
    // if available. The only exception is NACK,
    // which reports NACK instead of an error.

    uint32_t i2cStatusReg = I2C2->SR1;

    // NACK
    if (i2cStatusReg & I2C_SR1_AF)
    {
        shouldServiceI2C = 1;
        I2C2->SR1 &= ~(I2C_SR1_AF);
        i2cStatus = I2C_NACK;
    }

    // Arbitration Lost
    if (i2cStatusReg & I2C_SR1_ARLO)
    {
        shouldServiceI2C = 1;
        I2C2->SR1 &= ~(I2C_SR1_ARLO);
        i2cStatus = I2C_ERR;
    }

    // Bus Error
    if (i2cStatusReg & I2C_SR1_BERR)
    {
        shouldServiceI2C = 1;
        I2C2->SR1 &= ~(I2C_SR1_BERR);
        i2cStatus = I2C_ERR;
    }

    // Send a STOP bit
    I2C2->CR1 |= I2C_CR1_STOP;
}

