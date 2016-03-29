
#include "config.h"
#include "gpio.h"
#include "fnr.h"
#include "speedDAC.h"

void setFNR(FNR_t newState)
{
    resetSpeedDAC();
    switch (newState)
    {
        case FNR_FORWARD:
            setSpeedDACOutputEnable(1);
            HAL_GPIO_WritePin(GPIO_FNR_DIRECTION_PORT,
                              GPIO_FNR_DIRECTION_PIN,
                              GPIO_PIN_SET);
            HAL_GPIO_WritePin(GPIO_FNR_ENABLE_PORT,
                              GPIO_FNR_ENABLE_PIN,
                              GPIO_PIN_SET);
            break;

        case FNR_NEUTRAL:
            setSpeedDACOutputEnable(0);
            HAL_GPIO_WritePin(GPIO_FNR_ENABLE_PORT,
                              GPIO_FNR_ENABLE_PIN,
                              GPIO_PIN_RESET);
            break;

        case FNR_REVERSE:
            setSpeedDACOutputEnable(1);
            HAL_GPIO_WritePin(GPIO_FNR_DIRECTION_PORT,
                              GPIO_FNR_DIRECTION_PIN,
                              GPIO_PIN_RESET);
            HAL_GPIO_WritePin(GPIO_FNR_ENABLE_PORT,
                              GPIO_FNR_ENABLE_PIN,
                              GPIO_PIN_SET);
            break;
    }
}

void FNRCommsHandler(Packet_t* packet)
{
    if (packet->data[0] >= FNR_NEUTRAL && 
        packet->data[0] <= FNR_REVERSE)
    {
        setFNR(packet->data[0]);
    }
}