#ifndef __HEPHAESTUS_H__
#define __HEPHAESTUS_H__

#include <stdint.h>
#include "submoduleComms.h"

typedef struct hephaestusData{
   uint8_t steering[2];
} __attribute__((packed)) hephaestusData_t;

typedef uint8_t hephaestusMsg_t;
#define HEPHAESTUS_STATUS ((hephaestusMsg_t)0u) 

commsStatus_t getHephaestusStatus();
#endif
