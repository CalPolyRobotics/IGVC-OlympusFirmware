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

commsStatus_t getHermesStatus()
{
    return messageSubmodule(HERMES, HERMES_STATUS, NULL, 0u, 0u, SPI_TIMEOUT);
}
