#ifndef __HERA_H__
#define __HERA_H__

#include <stdint.h>
#include <stdlib.h>

#include "config.h"

#include "submoduleComms.h"
#include "timerCallback.h"

typedef uint8_t heraMsg_t;

#define HERA_STATUS  ((heraMsg_t)0u)
#define HERA_SPEED   ((heraMsg_t)1u)
#define HERA_STEER   ((heraMsg_t)2u)
#define HERA_SONAR   ((heraMsg_t)3u)

#define STATUS_LENGTH ((size_t)1u)
#define SPEED_LENGTH  ((size_t)2u)
#define STEER_LENGTH  ((size_t)2u)
#define SONAR_LENGTH  ((size_t)8u)

typedef struct heraData{
   uint8_t speed[2];
   uint8_t steer[2];
   uint8_t sonar[8];
} __attribute__((packed)) heraData_t;

extern heraData_t heraData;

commsStatus_t getHeraStatus();
commsStatus_t updateHeraSpeed();
commsStatus_t updateHeraSteer();
commsStatus_t updateHeraSonar();
void updateHeraData();

Timer_Return updateSpeed(void* dummy);
Timer_Return updateSteerDataLink(void* dummy);

#endif
