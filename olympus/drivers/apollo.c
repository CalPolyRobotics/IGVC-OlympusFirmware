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
    return messageSubmodule(APOLLO, APOLLO_STATUS, NULL, 0u, 0u, SPI_TIMEOUT);
}

commsStatus_t setTurnSignal(bool leftSignal, bool rightSignal)
{
    return COMMS_ERR_NOT_IMPL;
}

commsStatus_t setHeadlights(uint8_t speed)
{
    return COMMS_ERR_NOT_IMPL;
}

commsStatus_t setMiscLights(uint16_t lightMask, uint8_t speed)
{
    return COMMS_ERR_NOT_IMPL;
}
