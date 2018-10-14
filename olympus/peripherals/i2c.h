#ifndef __I2C_H__
#define __I2C_H__

typedef enum {
    I2C_ACK = 0,
    I2C_NACK,
    I2C_ERR
} I2CStatus;

typedef void (*i2cTxCallback)(void*, I2CStatus);
typedef void (*i2cRxCallback)(void*, uint8_t*, uint32_t, I2CStatus);

void i2cInit(void);
uint8_t i2cAddRxTransaction(uint8_t addr, uint32_t numBytes, i2cRxCallback callback, void* parameters);
uint8_t i2cAddTxTransaction(uint8_t addr, uint8_t* txData, uint32_t numBytes, i2cTxCallback callback, void* parameters);
void serviceI2C(void);


#endif
