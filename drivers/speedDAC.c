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

#define UPO_SET   ((uint16_t)0xE800)
#define UPO_RESET ((uint16_t)0xE000)

#define DATA_CTRL_MASK ((uint16_t)0x4000)
#define DATA_MASK      ((uint16_t)0x3FFC)
#define DATA_OFFSET    2u

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
static void writeDataMAX517(uint16_t dacData){
    uint8_t spiData[2] = {(dacData >> 8u) & 0xFF, dacData & 0xFF};

    /** TODO - Move SPI to DMA **/
    HAL_GPIO_WritePin(PORT_SS_THDAC, PIN_SS_THDAC, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi3, spiData, 2, 500);
    HAL_GPIO_WritePin(PORT_SS_THDAC, PIN_SS_THDAC, GPIO_PIN_SET);
}

static void setSpeedDAC(uint16_t value)
{
    writeDataMAX517(DATA_CTRL_MASK | ((value << DATA_OFFSET) & DATA_MASK));
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
    return 1;
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
                setSpeedDAC((uint16_t)currentSpeed);

            } else if (currentSpeed > targetSpeed) {
                currentSpeed = targetSpeed;
                setSpeedDAC((uint16_t)currentSpeed);
            }

        } else {
            currentSpeed = 0;
            setSpeedDAC(0);
        }
    }

    return CONTINUE_TIMER;
}

void enableSpeedDAC()
{
    writeDataMAX517(UPO_SET);
    enableSpeedOutput = 1u;
}

void disableSpeedDAC()
{
    writeDataMAX517(UPO_RESET);
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
    targetSpeed = value;
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
