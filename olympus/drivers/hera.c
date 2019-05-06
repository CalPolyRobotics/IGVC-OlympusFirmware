/**
 * Hera
 *
 * This module is responsible for all communication with
 * the Hera MCU.
 *
 * Note: all functions expect valid input
 */

#include <string.h>

#include "error.h"
#include "hera.h"
#include "hephaestus.h"
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

    if(status == COMMS_OK){
        memcpy(&heraData.speed, submoduleCommsBuff, SPEED_LENGTH);
    }

    return status;
}

commsStatus_t updateHeraSteer()
{
    commsStatus_t status = messageSubmodule(HERA, HERA_STEER, submoduleCommsBuff, 0u, STEER_LENGTH,
                                            SPI_TIMEOUT);
    if(status == COMMS_OK){
        memcpy(&heraData.steer, submoduleCommsBuff, STEER_LENGTH);
    }

    return status;
}

commsStatus_t updateHeraSonar()
{
    commsStatus_t status = messageSubmodule(HERA, HERA_SONAR, submoduleCommsBuff, 0u, SONAR_LENGTH,
                                            SPI_TIMEOUT);

    if(status == COMMS_OK){
        memcpy(&heraData.sonar, submoduleCommsBuff, SONAR_LENGTH);
    }

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

Timer_Return updateSpeed(void* dummy)
{
    if(updateHeraSpeed() != COMMS_OK)
    {
        ErrorHandler(HERA_SPEED_FAIL, NOTIFY);
    }


    return CONTINUE_TIMER;
}

Timer_Return updateSteerDataLink(void* dummy)
{
    if(updateHeraSteer() != COMMS_OK)
    {
        ErrorHandler(HERA_STEER_FAIL, NOTIFY);
    }

    if(updateHephaestusSteerPot(heraData.steer) != COMMS_OK)
    {
        ErrorHandler(HEPHAESTUS_STEER_FAIL, NOTIFY);
    }

    return CONTINUE_TIMER;
}
