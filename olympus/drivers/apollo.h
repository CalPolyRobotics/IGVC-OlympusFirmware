#ifndef __APOLLO_H__
#define __APOLLO_H__

#include <stdint.h>
#include <stdbool.h>
#include "submoduleComms.h"

typedef uint8_t apolloMsg_t;
#define APOLLO_STATUS          ((apolloMsg_t)0u) 
#define APOLLO_SET_TURN_SIGNAL ((apolloMsg_t)1u) 
#define APOLLO_SET_HEADLIGHTS  ((apolloMsg_t)2u) 
#define APOLLO_SET_MISC_LIGHTS ((apolloMsg_t)3u) 

commsStatus_t getApolloStatus();
commsStatus_t setTurnSignal(bool leftSignal, bool rightSignal);
commsStatus_t setHeadlights(uint8_t speed);
commsStatus_t setMiscLights(uint16_t lightMask, uint8_t speed);

#endif
