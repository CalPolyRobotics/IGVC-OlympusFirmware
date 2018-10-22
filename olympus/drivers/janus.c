/**
 * Janus
 *
 * This module is responsible for all communication with
 * the Janus MCU.
 *
 * Note: all functions expect valid input, for example
 * setFNR expects only values of 0, 1, or 2. Anything
 * other than expected values will produce undefined
 * behavior
 */

#include "janus.h"
#include "spi.h"

janusData_t janusData;

void commsSetFNR(uint8_t* data)
{
    setFNR(data[0]);
}

commsStatus_t getJanusStatus()
{
    return messageSubmodule(JANUS, JANUS_STATUS, NULL, 0u, 0u, SPI_TIMEOUT);
}

/**
 * \param state[0,1,2]
 */
commsStatus_t setFNR(fnr_t state)
{
    uint8_t message = (uint8_t)state;

    return messageSubmodule(JANUS, JANUS_SET_FNR, &message, sizeof(fnr_t), 0u, SPI_TIMEOUT);
}

/**
 * \return state[0,1,2,3]
 */
commsStatus_t updateFNR()
{
    commsStatus_t status = messageSubmodule(JANUS, JANUS_GET_FNR, submoduleCommsBuff, 0u,
                                            sizeof(fnr_t), SPI_TIMEOUT);

    uint8_t message = submoduleCommsBuff[0];
    if (status != COMMS_OK && message <= 2)
    {
        *janusData.fnr = message;
    }
    else
    {
        *janusData.fnr = FNR_INVALID;     
    }

    return status;
}
