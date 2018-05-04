#include <stdio.h>

#include "config.h"
#include "speedDAC.h"
#include "comms.h"
#include "timerCallback.h"
#include "adc.h"
#include "spi.h"

volatile uint8_t commsPedalAdc[2] = {0};

static uint16_t targetSpeed = 0;
static uint16_t currentSpeed = 0;
static uint8_t enableSpeedOutput = 0;

static Timer_Return speedDACCallback(void* dummy);

#define BIT_MASK_LOW_12 ((uint16_t)0x0FFF)

#define UPO_MASK      ((uint16_t)0x4000)
#define UPO_DATA_MASK ((uint16_t)0x03FF)
#define UPO_OFFSET    2u

#define DATA_MASK      ((uint16_t)0xE000)
#define DATA_DATA_MASK ((uint16_t)0x0001)
#define DATA_OFFSET    11u

#define SPI_WORD(mask, data_mask, offset, data) (((data & data_mask) << offset) | mask)

/**
 * Using MAX517X 12-Bit DAC
 *
 * To write and update the DAC, write
 *
 * Pull CS LOW
 * 0b01[12-bit DAC data]xx
 * Release CS
 *
 * To set/reset UPO (Auto/Man Enable)
 *
 * Pull CS LOW
 * 0b1110[high/low]xxxxxxxxxxx
 * Release CS
 */
static void writeDataMAX517(uint16_t value){
    /** TODO - Move SPI to DMA **/
    HAL_GPIO_WritePin(PORT_SS_THDAC, PIN_SS_THADC, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi3, (uint8_t*)&value, sizeof(value), 500);
    HAL_GPIO_WritePin(PORT_SS_THDAC, PIN_SS_THADC, GPIO_PIN_SET);
}

static void setSpeedDAC(uint16_t value)
{
    writeDataMAX517(SPI_WORD(DATA_MASK, DATA_DATA_MASK, DATA_OFFSET, value));
}

void initSpeedDAC()
{
    setSpeedDAC(0);
    enableSpeedDAC();
    addCallbackTimer(SPEED_DAC_INCR_PERIOD, speedDACCallback, NULL);
}

uint8_t isPedalDown()
{
    /** TODO - Request Pin from HERA **/
    return 0;
}

static Timer_Return speedDACCallback(void* dummy)
{
    if (enableSpeedOutput)
    {
        if (isPedalDown())
        {
            if (currentSpeed < targetSpeed)
            {
                currentSpeed += SPEED_DAC_INCR;
                if (currentSpeed > targetSpeed)
                {
                    currentSpeed = targetSpeed;
                }

            } else if (currentSpeed > targetSpeed) {
                currentSpeed = targetSpeed;
            }

            setSpeedDAC((uint16_t)currentSpeed);
        } else {
            currentSpeed = 0;
            setSpeedDAC(0);
        }
    }

    return CONTINUE_TIMER;
}

void enableSpeedDAC()
{
    writeDataMAX517(SPI_WORD(UPO_MASK, UPO_DATA_MASK, UPO_OFFSET, 1u));
    enableSpeedOutput = 1u;
}

void disableSpeedDAC()
{
    writeDataMAX517(SPI_WORD(UPO_MASK, UPO_DATA_MASK, UPO_OFFSET, 0u));
    enableSpeedOutput = 0u;
    resetSpeedDAC();
}

void resetSpeedDAC()
{
    targetSpeed = 0u;
    currentSpeed = 0u;
    setSpeedDAC(currentSpeed);
}

void writeSpeedDAC(uint16_t value)
{
    targetSpeed = (value & BIT_MASK_LOW_12);
}

uint16_t getSpeedDAC()
{
    return targetSpeed;
}

void commsSetThrottleCallback(Packet_t* packet)
{
    uint16_t speed = ((uint16_t)packet->data[0] << 8 | (packet->data[1] & 0xFF)); 
    writeSpeedDAC(speed);
}

void commsSetThrottleEnableCallback(Packet_t* packet){
    if(packet->data[0]){
        enableSpeedDAC();
    }else{
        disableSpeedDAC();
    }
}
