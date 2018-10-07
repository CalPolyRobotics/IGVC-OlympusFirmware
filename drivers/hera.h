#ifndef __HERA_H__
#define __HERA_H__

#include <stdint.h>
#include "submoduleComms.h"

typedef uint8_t heraMsg_t;
#define HERA_STATUS  ((heraMsg_t)0u) 

commsStatus_t getHeraStatus();
#endif
