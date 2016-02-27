
#include "gpio.h"
#include "fnr.h"

void setFNR(FNR_t newState)
{
    switch (newState)
    {
        case FNR_FORWARD:
            HAL_GPIO_WritePin(FNR_DIRECTION_PORT, FNR_DIRECTION_PIN, GPIO_PIN_SET);
            HAL_GPIO_WritePin(FNR_ENABLE_PORT, FNR_ENABLE_PIN, GPIO_PIN_SET);
            break;

        case FNR_NEUTRAL:
            HAL_GPIO_WritePin(FNR_ENABLE_PORT, FNR_ENABLE_PIN, GPIO_PIN_RESET);
            break;

        case FNR_REVERSE:
            HAL_GPIO_WritePin(FNR_DIRECTION_PORT, FNR_DIRECTION_PIN, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(FNR_ENABLE_PORT, FNR_ENABLE_PIN, GPIO_PIN_SET);
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