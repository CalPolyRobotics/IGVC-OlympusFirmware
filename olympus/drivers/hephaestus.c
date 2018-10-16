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

commsStatus_t getHephaestusStatus()
{
    return messageSubmodule(HEPHAESTUS, HEPHAESTUS_STATUS, NULL, 0u, 0u, SPI_TIMEOUT);
}

commsStatus_t updateHephaestusSteerPot(uint8_t* potVal)
{
    /**
     * Must create additional buffer to ensure messageSubmodule does not overwrite
     * potVal
     */
    uint8_t data[2];
    memcpy(data, potVal, sizeof(uint16_t));

    return messageSubmodule(HEPHAESTUS, HEPHAESTUS_UPDATE_POT, potVal, sizeof(uint16_t), 0u,
                            SPI_TIMEOUT);
}
