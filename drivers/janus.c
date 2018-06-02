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

commsStatus_t getJanusStatus()
{
    uint8_t data[1];
    messageSubmodule(JANUS, JANUS_STATUS, data, 0, 1);
    return data[0];
}

/**
 * \return state[0,1,2,3]
 */
fnr_t getFNR()
{
    uint8_t message = FNR_INVALID;
    messageSubmodule(JANUS, JANUS_GET_FNR, &message, 0, 1);

    if (message <= 2)
    {
        return message;
    }

    return FNR_INVALID;
}

/**
 * \param state[0,1,2]
 */
void setFNR(fnr_t state)
{
    uint8_t message = (uint8_t)state;
    messageSubmodule(JANUS, JANUS_SET_FNR, &message, 1, 0);
}
