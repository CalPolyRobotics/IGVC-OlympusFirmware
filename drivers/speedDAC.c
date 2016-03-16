
#include <stdio.h>
#include "speedDAC.h"
#include "i2c.h"
#include "comms.h"
#include "timerCallback.h"

#define SPEED_DAC_ADDRESS   0x62

#define SPEED_DAC_PERIOD    201
#define SPEED_DAC_INCR      2

static uint16_t targetSpeed = 0;
static uint16_t currentSpeed = 0;
static uint8_t enableSpeedOutput = 0;

static Timer_Return speedDACCallback(void* dummy);

static void setSpeedDAC(uint8_t value)
{
    i2cTransmit(SPEED_DAC_ADDRESS, &value, 1);
}

void initSpeedDAC()
{
    HAL_GPIO_WritePin(SPEED_DAC_ENABLE_PORT, SPEED_DAC_ENABLE_PIN, GPIO_PIN_SET); 
    setSpeedDAC(0);

    addCallbackTimer(SPEED_DAC_PERIOD, speedDACCallback, NULL);
}

static Timer_Return speedDACCallback(void* dummy)
{
    if (enableSpeedOutput)
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
    }

    return CONTINUE_TIMER;
}

void toggleSpeedDAC(Packet_t* packet)
{
   HAL_GPIO_TogglePin(SPEED_DAC_ENABLE_PORT, SPEED_DAC_ENABLE_PIN);
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

void speedDACHandler(Packet_t* packet)
{
    uint16_t speed;
    if (packet->header.packetLen - sizeof(PacketHeader_t) == 2)
    {
        speed = (packet->data[1] << 2) & 0xFF;

        writeSpeedDAC(speed);
    }
}