/**
 * Hephaestus
 *
 * This module is responsible for all communication with
 * the Hephaestus MCU.
 *
 * Note: all functions expect valid input
 */

#include "hephaestus.h"
#include "spi.h"
#include "string.h"

void commsSetSteering(uint8_t* data){
    setHephaestusSteering(*data);
}

commsStatus_t getHephaestusStatus()
{
    return messageSubmodule(HEPHAESTUS, HEPHAESTUS_STATUS, NULL, 0u, 0u, SPI_TIMEOUT);
}

commsStatus_t setHephaestusSteering(uint8_t angle)
{
    return messageSubmodule(HEPHAESTUS, HEPHAESTUS_SET_STEERING, &angle, sizeof(uint8_t), 0u,
                            SPI_TIMEOUT);
}

commsStatus_t updateHephaestusSteerPot(uint8_t* potVal)
{
    return messageSubmodule(HEPHAESTUS, HEPHAESTUS_UPDATE_POT, potVal, sizeof(uint16_t), 0u,
                            SPI_TIMEOUT);
}
