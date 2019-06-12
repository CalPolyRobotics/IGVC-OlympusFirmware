#ifndef __HERMES_H__
#define __HERMES_H__

#include <stdint.h>
#include <stdlib.h>

#include "submoduleComms.h"

typedef uint8_t hermesMsg_t;

#define HERMES_STATUS   ((hermesMsg_t)0u)
#define HERMES_GET_AMAN ((hermesMsg_t)1u)

#define STATUS_LENGTH ((size_t)1u)

typedef struct hermesData{
    uint8_t aman[1];
} __attribute__((packed)) hermesData_t;

extern hermesData_t hermesData;

commsStatus_t getHermesStatus();
commsStatus_t getAutoman();

#endif
