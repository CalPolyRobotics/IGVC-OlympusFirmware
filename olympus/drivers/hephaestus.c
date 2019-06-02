/**
 * Hephaestus
 *
 * This module is responsible for all communication with
 * the Hephaestus MCU.
 *
 * Note: all functions expect valid input
 */

#include "error.h"
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
    commsStatus_t status;
    status = messageSubmodule(HEPHAESTUS, HEPHAESTUS_SET_STEERING, &angle, sizeof(uint8_t), 0u,
                              SPI_TIMEOUT);

    if(status != COMMS_OK){
        ErrorHandler(HEPH_SET_STEER_FAIL, NOTIFY);
    }

    return status;
}

commsStatus_t updateHephaestusSteerPot(uint8_t* potVal)
{
    commsStatus_t status;
    status = messageSubmodule(HEPHAESTUS, HEPHAESTUS_UPDATE_POT, potVal, sizeof(uint16_t), 0u,
                              SPI_TIMEOUT);
    if(status != COMMS_OK){
        ErrorHandler(HEPH_STEER_POT_FAIL, NOTIFY);
    }

    return status;
}
