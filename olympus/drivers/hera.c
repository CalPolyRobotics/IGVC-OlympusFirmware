/**
 * Hera
 *
 * This module is responsible for all communication with
 * the Hera MCU.
 *
 * Note: all functions expect valid input
 */

#include <string.h>
#include <stdio.h>

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
    }else{
        ErrorHandler(HERA_SPEED_FAIL, NOTIFY);
    }

    return status;
}

commsStatus_t updateHeraSteer()
{
    commsStatus_t status = messageSubmodule(HERA, HERA_STEER, submoduleCommsBuff, 0u, STEER_LENGTH,
                                            SPI_TIMEOUT);
    if(status == COMMS_OK){
        memcpy(&heraData.steer, submoduleCommsBuff, STEER_LENGTH);
    }else{
        ErrorHandler(HERA_STEER_FAIL, NOTIFY);
    }

    return status;
}

commsStatus_t updateHeraSonar()
{
    commsStatus_t status = messageSubmodule(HERA, HERA_SONAR, submoduleCommsBuff, 0u, SONAR_LENGTH,
                                            SPI_TIMEOUT);

    if(status == COMMS_OK){
        memcpy(&heraData.sonar, submoduleCommsBuff, SONAR_LENGTH);
    }else{
        ErrorHandler(HERA_SONAR_FAIL, NOTIFY);
    }

    return status;
}

void updateHeraData()
{
    updateHeraSpeed();
    updateHeraSteer();
    updateHeraSonar();
}

Timer_Return updateSpeed(void* dummy)
{
    updateHeraSpeed(); 

    return CONTINUE_TIMER;
}

Timer_Return updateSteerDataLink(void* dummy)
{
    updateHeraSteer();
    updateHephaestusSteerPot(heraData.steer);

    return CONTINUE_TIMER;
}
