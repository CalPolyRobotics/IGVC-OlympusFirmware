/**
 * Hephaestus
 *
 * This module is responsible for all communication with
 * the Hephaestus MCU.
 *
 * Note: all functions expect valid input
 */

#include "hephaestus.h"
#include "spi.h"

commsStatus_t getHephaestusStatus()
{
    return messageSubmodule(HEPHAESTUS, HEPHAESTUS_STATUS, NULL, 0, 0);
}
