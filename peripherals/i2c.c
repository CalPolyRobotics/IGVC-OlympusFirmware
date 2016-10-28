
#include <stdio.h>
#include <stdbool.h>

#include "config.h"
#include "i2c.h"
#include "gpio.h"
#include "buffer8.h"

typedef enum {
    I2C_TYPE_TX,
    I2C_TYPE_RX
} I2C_TransferType;

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

static I2CTransaction transactionList[I2C_MAX_NUM_TRANSACTIONS];
static uint8_t i2cTxDataBuffer[I2C_TRANSACTION_TX_BUF_SIZE];
static uint8_t i2cRxDataBuffer[I2C_TRANSACTION_RX_BUF_SIZE];
static uint32_t i2cRxDataBufferIdx;
static buffer8_t i2cTxDataFifo;

static uint32_t freeTransaction;
static volatile uint32_t nextTransaction;

static volatile uint8_t i2cRunning;
static volatile uint8_t shouldServiceI2C;
static volatile uint8_t i2cStatus;
static volatile uint8_t i2cBytesLeft;

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

static void startI2CTransfer()
{
    i2cRunning = 1;

    i2cBytesLeft = transactionList[nextTransaction].numBytes;

    if (transactionList[nextTransaction].type == I2C_TYPE_RX)
    {
        I2C2->CR1 |= I2C_CR1_ACK;
    }
    
    I2C2->CR1 |= I2C_CR1_START;
}

uint8_t i2cAddRxTransaction(uint8_t addr, uint32_t numBytes, i2cRxCallback callback, void* parameters)
{
    if (transactionList[freeTransaction].ready)
    {
        return false;
    }

    transactionList[freeTransaction].ready = 1;
    transactionList[freeTransaction].numBytes = numBytes;
    transactionList[freeTransaction].addr = addr;
    transactionList[freeTransaction].type = I2C_TYPE_RX;
    transactionList[freeTransaction].rxCallback = callback;
    transactionList[freeTransaction].cParams = parameters;

    freeTransaction++;
    if (freeTransaction == I2C_MAX_NUM_TRANSACTIONS)
    {
        freeTransaction = 0;
    }

    if (!i2cRunning)
    {
        startI2CTransfer();
    }

    return true;
}

uint8_t i2cAddTxTransaction(uint8_t addr, uint8_t* txData, uint32_t numBytes, i2cTxCallback callback, void* parameters)
{
    if (transactionList[freeTransaction].ready)
    {
        return false;
    }

    transactionList[freeTransaction].ready = 1;
    transactionList[freeTransaction].numBytes = numBytes;
    transactionList[freeTransaction].addr = addr;
    transactionList[freeTransaction].type = I2C_TYPE_TX;
    transactionList[freeTransaction].txCallback = callback;
    transactionList[freeTransaction].cParams = parameters;

    buffer8_write(&i2cTxDataFifo, txData, numBytes);

    freeTransaction++;
    if (freeTransaction == I2C_MAX_NUM_TRANSACTIONS)
    {
        freeTransaction = 0;
    }

    if (!i2cRunning)
    {
        startI2CTransfer();
    }

    return true;
}

void serviceI2C()
{
    if (shouldServiceI2C)
    {
        shouldServiceI2C = 0;
        i2cRunning = 0;
        transactionList[nextTransaction].ready = 0;

        while (I2C2->SR2 & I2C_SR2_BUSY);
        I2C2->CR1 &= ~(I2C_CR1_STOP);

        // Transaction has completed

        if (i2cStatus != I2C_ACK)
        {
            // Transaction failed
            if (transactionList[nextTransaction].type == I2C_TYPE_TX)
            {
                // Clear the remaining bytes in the fifo
                while (i2cBytesLeft > 0)
                {
                    buffer8_get(&i2cTxDataFifo);
                    i2cBytesLeft--;
                }

                if (transactionList[nextTransaction].txCallback)
                {
                    transactionList[nextTransaction].txCallback(
                            transactionList[nextTransaction].cParams,
                            i2cStatus);
                }
            } else {
                if (transactionList[nextTransaction].rxCallback)
                {
                    transactionList[nextTransaction].rxCallback(
                            transactionList[nextTransaction].cParams,
                            NULL,
                            0,
                            i2cStatus);
                }
            }
        } else {
            // Transaction successful
           
            if (transactionList[nextTransaction].type == I2C_TYPE_TX)
            {
                if (transactionList[nextTransaction].txCallback)
                {
                    transactionList[nextTransaction].txCallback(
                            transactionList[nextTransaction].cParams,
                            i2cStatus);
                }
            } else {

                if (transactionList[nextTransaction].numBytes == 1)
                {
                    i2cRxDataBuffer[i2cRxDataBufferIdx] = I2C2->DR;
                    i2cRxDataBufferIdx++;
                }

                if (transactionList[nextTransaction].rxCallback)
                {
                    transactionList[nextTransaction].rxCallback(
                            transactionList[nextTransaction].cParams,
                            i2cRxDataBuffer,
                            i2cRxDataBufferIdx,
                            i2cStatus);
                }
            }
        }

        // Reset I2C for the next transaction
        i2cRxDataBufferIdx = 0;
        i2cStatus = I2C_ACK;

        nextTransaction++;
        if (nextTransaction == I2C_MAX_NUM_TRANSACTIONS)
        {
            nextTransaction = 0;
        }

        if (transactionList[nextTransaction].ready == 1)
        {
            startI2CTransfer();
        }
    }
}

static void readI2CByte()
{
    if (i2cBytesLeft == 3)
    {
        I2C2->CR1 &= ~I2C_CR1_ACK;
    } else if (i2cBytesLeft == 2) {
        I2C2->CR1 |= I2C_CR1_STOP;
        shouldServiceI2C = 1;

        i2cRxDataBuffer[i2cRxDataBufferIdx] = I2C2->DR;
        i2cRxDataBufferIdx++;
        i2cBytesLeft--;
    }

    i2cRxDataBuffer[i2cRxDataBufferIdx] = I2C2->DR;
    i2cRxDataBufferIdx++;
    i2cBytesLeft--;

}

void I2C2_EV_IRQHandler()
{
    uint32_t i2cStatusReg = I2C2->SR1;

    // Start Bit Sent
    if (i2cStatusReg & I2C_SR1_SB)
    {
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
            if (i2cBytesLeft > 0)
            {
                I2C2->DR = buffer8_get(&i2cTxDataFifo);
                i2cBytesLeft--;
            } else {
                I2C2->CR1 |= I2C_CR1_STOP;
                shouldServiceI2C = 1;
            }
        } else {

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
                I2C2->DR = buffer8_get(&i2cTxDataFifo);
                i2cBytesLeft--;
            } else {
                I2C2->CR1 |= I2C_CR1_STOP;
                shouldServiceI2C = 1;
            }
        } else {
            readI2CByte();
        }
    }
}

void I2C2_ER_IRQHandler()
{
    uint32_t i2cStatusReg = I2C2->SR1;

    // NACK
    if (i2cStatusReg & I2C_SR1_AF)
    {
        shouldServiceI2C = 1;
        i2cStatus = I2C_NACK;
    }

    // Arbitration Lost
    if (i2cStatusReg & I2C_SR1_ARLO)
    {
        shouldServiceI2C = 1;
        i2cStatus = I2C_ERR;
    }

    // Bus Error
    if (i2cStatusReg & I2C_SR1_BERR)
    {
        shouldServiceI2C = 1;
        i2cStatus = I2C_ERR;
    }

    I2C2->CR1 |= I2C_CR1_STOP;
}

