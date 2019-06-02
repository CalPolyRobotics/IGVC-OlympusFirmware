/**
 * Zeus
 *
 * This module is responsible for all communication with
 * the Zeus MCU.
 *
 * Note: all functions expect valid input
 */

#include <string.h>
#include <stdio.h>

#include "error.h"
#include "zeus.h"
#include "spi.h"

zeusData_t zeusData;

commsStatus_t getZeusStatus()
{
    return messageSubmodule(ZEUS, ZEUS_STATUS, NULL, 0u, 0u, SPI_TIMEOUT);
}
