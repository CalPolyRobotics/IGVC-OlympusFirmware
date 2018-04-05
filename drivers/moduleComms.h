#ifndef __MODULE_COMMS_H__
#define __MODULE_COMMS_H__

#include <stdint.h>

#define NUM_MODULES 4

typedef enum MODULE {
    APOLLO=0, HEPHAESTUS, HERA, JANUS
} MODULE;

uint8_t* send_message(MODULE module, uint8_t msg_type, uint8_t* buff, uint8_t tx_size, uint8_t rx_size);
#endif

void pull_cs_low(MODULE module);

void release_cs(MODULE module);
