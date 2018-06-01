#ifndef __HEPHAESTUS_H__
#define __HEPHAESTUS_H__

#include <stdint.h>
#include "submoduleComms.h"


typedef uint8_t hephaestusMsg_t;
#define HEPHAESTUS_STATUS ((hephaestusMsg_t)0u) 

commsStatus_t getHephaestusStatus();
#endif
