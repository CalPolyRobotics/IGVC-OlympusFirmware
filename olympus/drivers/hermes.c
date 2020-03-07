/**
 * Hermes
 *
 * This module is responsible for all communication with
 * the Hermes MCU.
 *
 * Note: all functions expect valid input
 */

#include <string.h>
#include <stdio.h>

#include "error.h"
#include "hermes.h"
#include "spi.h"

hermesData_t hermesData;

void commsSetSpeed(uint8_t* data){
    uint16_t targetSpeed = (((uint16_t) data[1]) << 8u) | data[0];
    updateHermesSetSpeedTarget(targetSpeed);
}

commsStatus_t getHermesStatus()
{
    return messageSubmodule(HERMES, HERMES_STATUS, NULL, 0u, 0u, SPI_TIMEOUT);
}


/**
 * state[0,1]
 */
commsStatus_t updateHermesAutoman()
{
    commsStatus_t status = messageSubmodule(HERMES, HERMES_GET_AMAN,
                                            submoduleCommsBuff, 0u,
                                            sizeof(uint8_t), SPI_TIMEOUT);

    uint8_t message = submoduleCommsBuff[0];
    if (status == COMMS_OK && message <= 1)
    {
        *hermesData.aman = message;
    }

    return status;
}

commsStatus_t updateHermesSpeed()
{
    commsStatus_t status = messageSubmodule(HERMES, HERMES_GET_SPEED,
                                            submoduleCommsBuff, 0u,
                                            SPEED_LENGTH, SPI_TIMEOUT);

    if(status == COMMS_OK){
        memcpy(&hermesData.speed, submoduleCommsBuff, SPEED_LENGTH);
    }else{
        ErrorHandler(HERMES_GET_SPEED_FAIL, NOTIFY);
    }

    return status;
}

commsStatus_t updateHermesSetSpeedTarget(uint16_t speedTargetMMs)
{
    uint8_t speed[2] = {(speedTargetMMs >> 8u), speedTargetMMs};

    commsStatus_t status = messageSubmodule(HERMES, HERMES_SET_SPEED_TARGET,
                                            speed, SPEED_TARGET_LENGTH,
                                            0u, SPI_TIMEOUT);
    if(status != COMMS_OK){
        ErrorHandler(HERMES_SET_SPD_TAR_FAIL, NOTIFY);
    }

    return status;
}
