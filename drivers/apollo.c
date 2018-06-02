/**
 * Apollo
 *
 * This module is responsible for all communication with
 * the Apollo MCU.
 *
 * Note: all functions expect valid input
 */

#include "apollo.h"
#include "spi.h"

commsStatus_t getApolloStatus()
{
    uint8_t data[1];
    messageSubmodule(APOLLO, APOLLO_STATUS, data, 0, 1);
    return data[0];
}

void setTurnSignal(bool leftSignal, bool rightSignal)
{
    return;
}

void setHeadlights(uint8_t speed)
{
    return;
}

void setMiscLights(uint16_t lightMask, uint8_t speed)
{
    return;
}
