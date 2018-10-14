/**
 * Hera
 *
 * This module is responsible for all communication with
 * the Hera MCU.
 *
 * Note: all functions expect valid input
 */

#include "hera.h"
#include "spi.h"

commsStatus_t getHeraStatus()
{
    uint8_t data[1];
    messageSubmodule(HERA, HERA_STATUS, data, 0, 1);
    return data[0];
}
