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

hephaestusData_t hephaestusData;

void commsSetSteering(uint8_t* data){
    setHephaestusSteering(*data);
}

commsStatus_t getHephaestusStatus()
{
    return messageSubmodule(HEPHAESTUS, HEPH_STATUS, NULL, 0u, 0u, SPI_TIMEOUT);
}

commsStatus_t setHephaestusSteering(uint8_t angle)
{
    commsStatus_t status;
    status = messageSubmodule(HEPHAESTUS, HEPH_SET_STEERING, &angle, sizeof(uint8_t), 0u,
                              SPI_TIMEOUT);

    if(status != COMMS_OK){
        ErrorHandler(HEPH_SET_STEER_FAIL, NOTIFY);
    }

    return status;
}

commsStatus_t getHephaestusSteerPot(){
    commsStatus_t status = messageSubmodule(HEPHAESTUS, HEPH_GET_STPOT,
                                            submoduleCommsBuff, 0u,
                                            sizeof(uint16_t),
                                            SPI_TIMEOUT);

    if(status == COMMS_OK){
        memcpy(&hephaestusData.steer_pot, submoduleCommsBuff,
               sizeof(uint16_t));
    }else{
        ErrorHandler(HEPH_GET_STR_POT_FAIL, NOTIFY);
    }

    return status;
}

commsStatus_t getHephaestusSteerSpeed(){
    commsStatus_t status = messageSubmodule(HEPHAESTUS, HEPH_GET_STSPD,
                                            submoduleCommsBuff, 0u,
                                            sizeof(uint16_t),
                                            SPI_TIMEOUT);

    if(status == COMMS_OK){
        memcpy(&hephaestusData.steer_speed, submoduleCommsBuff,
               sizeof(uint16_t));
    }else{
        ErrorHandler(HEPH_GET_STR_SPEED_FAIL, NOTIFY);
    }

    return status;
}

// TODO - To be implemented
commsStatus_t setHephaestusBrake(uint8_t *data){
    return COMMS_ERR_NOT_IMPL;
}

commsStatus_t getHephaestusBrakePot(){
    commsStatus_t status = messageSubmodule(HEPHAESTUS, HEPH_GET_BRPOT,
                                            submoduleCommsBuff, 0u,
                                            sizeof(uint16_t),
                                            SPI_TIMEOUT);

    if(status == COMMS_OK){
        memcpy(&hephaestusData.brake_pot, submoduleCommsBuff,
               sizeof(uint16_t));
    }else{
        ErrorHandler(HEPH_GET_BRK_POT_FAIL, NOTIFY);
    }

    return status;
}
