/**
 * Hera
 *
 * This module is responsible for all communication with
 * the Hera MCU.
 *
 * Note: all functions expect valid input
 */

#include <string.h>

#include "hera.h"
#include "spi.h"

heraData_t heraData;

commsStatus_t getHeraStatus()
{
    return messageSubmodule(HERA, HERA_STATUS, NULL, 0u, 0u, SPI_TIMEOUT);
}

commsStatus_t updateHeraSpeed()
{
    commsStatus_t status = messageSubmodule(HERA, HERA_SPEED, submoduleCommsBuff, 0u, SPEED_LENGTH,
                                            SPI_TIMEOUT);

    memcpy(&heraData.speed, submoduleCommsBuff, SPEED_LENGTH);
    return status;
}

commsStatus_t updateHeraSteer()
{
    commsStatus_t status = messageSubmodule(HERA, HERA_STEER, submoduleCommsBuff, 0u, STEER_LENGTH,
                                            SPI_TIMEOUT);

    memcpy(&heraData.steer, submoduleCommsBuff, STEER_LENGTH);
    return status;
}

commsStatus_t updateHeraSonar()
{
    commsStatus_t status = messageSubmodule(HERA, HERA_SONAR, submoduleCommsBuff, 0u, SONAR_LENGTH,
                                            SPI_TIMEOUT);

    memcpy(&heraData.sonar, submoduleCommsBuff, SONAR_LENGTH);
    return status;
}

commsStatus_t updateHeraData()
{
    commsStatus_t status;

    if((status = updateHeraSpeed()) != COMMS_OK)
    {
        return status;
    }

    if((status = updateHeraSteer()) != COMMS_OK)
    {
        return status;
    }

    if((status = updateHeraSonar()) != COMMS_OK)
    {
        return status;
    }

    return status;
}
