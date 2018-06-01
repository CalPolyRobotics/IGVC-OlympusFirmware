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
    return messageSubmodule(HERA, HERA_STATUS, NULL, 0, 0);
}
