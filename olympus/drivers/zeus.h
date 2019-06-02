#ifndef __ZEUS_H__
#define __ZEUS_H__

#include <stdint.h>
#include <stdlib.h>

#include "submoduleComms.h"

typedef uint8_t zeusMsg_t;

#define ZEUS_STATUS  ((zeusMsg_t)0u)

#define STATUS_LENGTH ((size_t)1u)

typedef struct zeusData{
} __attribute__((packed)) zeusData_t;

extern zeusData_t zeusData;

commsStatus_t getZeusStatus();

#endif
