
#include <stdio.h>

#include "config.h"
#include "speedDAC.h"
#include "i2c.h"
#include "comms.h"
#include "timerCallback.h"
#include "adc.h"

static uint16_t targetSpeed = 0;
static uint16_t currentSpeed = 0;
static uint8_t enableSpeedOutput = 0;

static Timer_Return speedDACCallback(void* dummy);

static void setSpeedDAC(uint8_t value)
{
    i2cAddTxTransaction(HERMES_SPEED_DAC_I2C_ADDR, &value, 1, NULL, NULL);
}

void initSpeedDAC()
{
    HAL_GPIO_WritePin(GPIO_AUTO_THROTTLE_ENABLE, GPIO_PIN_SET); 
    setSpeedDAC(0);

    addCallbackTimer(SPEED_DAC_INCR_PERIOD, speedDACCallback, NULL);
}

uint8_t isPedalDown()
{
    return getPedalValue() > 248;
}

static Timer_Return speedDACCallback(void* dummy)
{
    if (enableSpeedOutput)
    {
        if (isPedalDown())
        {
            if (currentSpeed <= targetSpeed)
            {
                currentSpeed += SPEED_DAC_INCR;
                if (currentSpeed > targetSpeed)
                {
                    currentSpeed = targetSpeed;
                }

            } else if (currentSpeed > targetSpeed) {
                currentSpeed = targetSpeed;
            }

            setSpeedDAC((uint8_t)currentSpeed);
        } else {
            currentSpeed = 0;
            setSpeedDAC(0);
        }
    }

    return CONTINUE_TIMER;
}

void toggleSpeedDAC(Packet_t* packet)
{
    HAL_GPIO_TogglePin(GPIO_AUTO_THROTTLE_ENABLE);
}

void enableSpeedDAC()
{
    HAL_GPIO_WritePin(GPIO_AUTO_THROTTLE_ENABLE_PORT,
                      GPIO_AUTO_THROTTLE_ENABLE_PIN,
                      GPIO_PIN_SET);
}

void disableSpeedDAC()
{
    HAL_GPIO_WritePin(GPIO_AUTO_THROTTLE_ENABLE_PORT,
                      GPIO_AUTO_THROTTLE_ENABLE_PIN,
                      GPIO_PIN_RESET);
}

void setSpeedDACOutputEnable(uint8_t enable)
{
    enableSpeedOutput = enable;
    if (!enable)
    {
        setSpeedDAC(0);
    }
}

void writeSpeedDAC(uint8_t value)
{
    targetSpeed = (value & 0xFF);
}

void resetSpeedDAC()
{
    currentSpeed = 0;
    setSpeedDAC(currentSpeed);
}

uint16_t getSpeedDAC()
{
    return targetSpeed;
}

void speedDACHandler(Packet_t* packet)
{
    uint16_t speed;
    if (packet->header.packetLen - sizeof(PacketHeader_t) == 2)
    {
        speed = (packet->data[1] << 2) & 0xFF;

        writeSpeedDAC(speed);
    }
}
