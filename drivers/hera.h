#ifndef __HERA_H__
#define __HERA_H__

#include <stdint.h>
#include "submoduleComms.h"

typedef struct heraData{
   uint8_t speed[4];
   uint8_t pedal[2];
   uint8_t sonar[8];
} __attribute__((packed)) heraData_t;

typedef uint8_t heraMsg_t;
#define HERA_STATUS  ((heraMsg_t)0u) 

commsStatus_t getHeraStatus();
#endif
