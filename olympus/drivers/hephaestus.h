#ifndef __HEPHAESTUS_H__
#define __HEPHAESTUS_H__

#include <stdint.h>
#include "submoduleComms.h"

typedef struct hephaestusData{
   uint8_t steering[2];
} __attribute__((packed)) hephaestusData_t;

typedef uint8_t hephaestusMsg_t;
#define HEPHAESTUS_STATUS       ((hephaestusMsg_t)0u)
#define HEPHAESTUS_SET_STEERING ((hephaestusMsg_t)1u)
#define HEPHAESTUS_UPDATE_POT   ((hephaestusMsg_t)2u)

void commsSetSteering(uint8_t *data);
commsStatus_t getHephaestusStatus();
commsStatus_t setHephaestusSteering(uint8_t angle);
commsStatus_t updateHephaestusSteerPot(uint8_t *data);

#endif
