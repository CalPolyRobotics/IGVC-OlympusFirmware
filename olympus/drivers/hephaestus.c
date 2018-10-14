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
    uint8_t data[1];
    messageSubmodule(HEPHAESTUS, HEPHAESTUS_STATUS, data, 0, 1);
    return data[0];
}
