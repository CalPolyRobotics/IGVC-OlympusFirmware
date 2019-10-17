#ifndef __HEPH_H__
#define __HEPH_H__

#include <stdint.h>
#include "submoduleComms.h"

typedef struct hephaestusData{
   uint8_t steer_pot[2];
   uint8_t steer_speed[2];
   uint8_t brake_pot[2];
} __attribute__((packed)) hephaestusData_t;

typedef uint8_t hephaestusMsg_t;
#define HEPH_STATUS       ((hephaestusMsg_t)0u)
#define HEPH_SET_STEERING ((hephaestusMsg_t)1u)
#define HEPH_GET_STPOT    ((hephaestusMsg_t)2u)
#define HEPH_GET_STSPD    ((hephaestusMsg_t)3u)
#define HEPH_SET_BRAKE    ((hephaestusMsg_t)4u)
#define HEPH_GET_BRPOT    ((hephaestusMsg_t)5u)

void commsSetSteering(uint8_t *data);
commsStatus_t getHephaestusStatus();
commsStatus_t setHephaestusSteering(uint8_t angle);
commsStatus_t getHephaestusSteerPot();
commsStatus_t getHephaestusSteerSpeed();
commsStatus_t setHephaestusBrake(uint8_t *data);
commsStatus_t getHephaestusBrakePot();

#endif
