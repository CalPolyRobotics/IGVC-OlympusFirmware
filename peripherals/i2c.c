
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
static uint8_t i2cTxNextTransferBuffer[I2C_TRANSACTION_TX_BUF_SIZE];
static uint32_t i2cTxNextTransferIdx;

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

    if (transactionList[nextTransaction].type == I2C_TYPE_TX)
    {
        uint32_t i;
        for (i = 0; i < transactionList[nextTransaction].numBytes; i++)
        {
            i2cTxNextTransferBuffer[i] = buffer8_get(&i2cTxDataFifo);
        }
        i2cTxNextTransferIdx = 0;
    } else {
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

static volatile uint32_t lastIRQState;

void serviceI2C()
{
    uint8_t lastTransaction;
    I2CStatus lastStatus;

    if (shouldServiceI2C)
    {
        if(!(I2C2->SR2 & I2C_SR2_BUSY))
        {
            I2C2->CR1 &= ~(I2C_CR1_STOP);

            shouldServiceI2C = 0;
            lastTransaction = nextTransaction;
            lastStatus = i2cStatus;

            // Transaction has completed

            if (i2cStatus != I2C_ACK)
            {
                // Transaction failed
                if (transactionList[nextTransaction].type == I2C_TYPE_TX)
                {
                }
            } else {
                // Transaction successful
                if (transactionList[nextTransaction].numBytes == 1)
                {
                    i2cRxDataBuffer[i2cRxDataBufferIdx] = I2C2->DR;
                    i2cRxDataBufferIdx++;
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

            if (transactionList[lastTransaction].type == I2C_TYPE_TX)
            {
                if (transactionList[lastTransaction].txCallback)
                {
                    transactionList[lastTransaction].txCallback(
                            transactionList[lastTransaction].cParams,
                            lastStatus);
                }
            } else {

                if (transactionList[lastTransaction].rxCallback)
                {
                    transactionList[lastTransaction].rxCallback(
                            transactionList[lastTransaction].cParams,
                            i2cRxDataBuffer,
                            i2cRxDataBufferIdx,
                            lastStatus);
                }
            }

            transactionList[lastTransaction].ready = 0;
            i2cRunning = 0;

            if (transactionList[nextTransaction].ready == 1)
            {
                startI2CTransfer();
            }
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
            lastIRQState = 1;
        } else {
            // Send address with read
            I2C2->DR = transactionList[nextTransaction].addr << 1 | 1;
            lastIRQState = 2;
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
                I2C2->DR = i2cTxNextTransferBuffer[i2cTxNextTransferIdx++];
                i2cBytesLeft--;
                lastIRQState = 3;
            } else {
                I2C2->CR1 |= I2C_CR1_STOP;
                shouldServiceI2C = 1;

                lastIRQState = 4;
            }
        } else {

            if (i2cBytesLeft == 1)
            {
                i2cBytesLeft--;
                I2C2->CR1 &= ~I2C_CR1_ACK;
                I2C2->CR1 |= I2C_CR1_STOP;
                shouldServiceI2C = 1;


                lastIRQState = 5;
            } else if (i2cBytesLeft == 2)
            {
                I2C2->CR1 &= ~I2C_CR1_ACK;
                I2C2->CR1 |= I2C_CR1_POS;


                lastIRQState = 6;
            } else {
                lastIRQState = 7;
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
                I2C2->DR = i2cTxNextTransferBuffer[i2cTxNextTransferIdx++];
                i2cBytesLeft--;
                lastIRQState = 8;
            } else {
                I2C2->CR1 |= I2C_CR1_STOP;
                shouldServiceI2C = 1;
                lastIRQState = 9;
            }
        } else {
            readI2CByte();
            lastIRQState = 10;
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
        I2C2->SR1 &= ~(I2C_SR1_AF);
        i2cStatus = I2C_NACK;
        lastIRQState = 11;
    }

    // Arbitration Lost
    if (i2cStatusReg & I2C_SR1_ARLO)
    {
        shouldServiceI2C = 1;
        I2C2->SR1 &= ~(I2C_SR1_ARLO);
        i2cStatus = I2C_ERR;
        lastIRQState = 12;
    }

    // Bus Error
    if (i2cStatusReg & I2C_SR1_BERR)
    {
        shouldServiceI2C = 1;
        I2C2->SR1 &= ~(I2C_SR1_BERR);
        i2cStatus = I2C_ERR;
        lastIRQState = 13;
    }

    I2C2->CR1 |= I2C_CR1_STOP;
}

