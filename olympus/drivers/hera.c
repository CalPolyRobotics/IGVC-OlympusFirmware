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
    messageSubmodule(HERA, HERA_STATUS, submoduleCommsBuff, 0u, STATUS_LENGTH);
    return submoduleCommsBuff[STATUS_IDX];
}

commsStatus_t updateHeraSpeed()
{
    messageSubmodule(HERA, HERA_SPEED, submoduleCommsBuff, 0u, STATUS_LENGTH + SPEED_LENGTH);
    memcpy(&heraData.speed, &submoduleCommsBuff[DATA_IDX], SPEED_LENGTH);
    return submoduleCommsBuff[STATUS_IDX];
}

commsStatus_t updateHeraSteer()
{
    messageSubmodule(HERA, HERA_STEER, submoduleCommsBuff, 0u, STATUS_LENGTH + STEER_LENGTH);
    memcpy(&heraData.steer, &submoduleCommsBuff[DATA_IDX], STEER_LENGTH);
    return submoduleCommsBuff[STATUS_IDX];
}

commsStatus_t updateHeraSonar()
{
    messageSubmodule(HERA, HERA_SONAR, submoduleCommsBuff, 0u, STATUS_LENGTH + SONAR_LENGTH);
    memcpy(&heraData.pedal, &submoduleCommsBuff[DATA_IDX], SONAR_LENGTH);
    return submoduleCommsBuff[STATUS_IDX];
}

commsStatus_t updateHeraData()
{
    commsStatus_t stat;

    if((stat = updateHeraSpeed()) != COMMS_OK)
    {
        return stat;
    }

    if((stat = updateHeraSteer()) != COMMS_OK)
    {
        return stat;
    }

    if((stat = updateHeraSonar()) != COMMS_OK)
    {
        return stat;
    }

    return COMMS_OK;
}
