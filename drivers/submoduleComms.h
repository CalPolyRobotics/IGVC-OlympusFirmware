#ifndef __SUBMODULECOMMS_H__
#define __SUBMODULECOMMS_H__

#include <stdint.h>

#define SUBMODULE_START_BYTE 0xEA

#define NUM_MODULES    5
#define NUM_SUBMODULES 4

/** Note Submodules must proceed OLYMPUS **/
typedef enum module{
   APOLLO     = 0,
   HEPHAESTUS = 1,
   HERA       = 2,
   JANUS      = 3,
   OLYMPUS    = 4,
   NONE       = 255
}module_t;

typedef enum apolloMsg{
   APOLLO_ECHO             = 0x00,
   APOLLO_SET_TURN_SIGNAL  = 0x01,
   APOLLO_SET_HEADLIGHTS   = 0x02,
   APOLLO_SET_MISC_LIGHTS  = 0x03,
   APOLLO_GET_LIGHT_SENSOR = 0x04
}apolloMsg_t;

typedef enum heraMsg{
   HERA_ECHO = 0
}heraMsg_t;

typedef enum hephaestusMsg{
   HEPHAESTUS_ECHO = 0
}hephaestusMsg_t;

typedef enum janusMsg{
   JANUS_ECHO    = 0,
   JANUS_SET_FNR = 1,
   JANUS_GET_FNR = 2
}janusMsg_t;

extern uint8_t submoduleCommsBuff[256];

void messageSubmodule(module_t module, uint8_t msg_type, uint8_t* buff, uint8_t tx_size, uint8_t rx_size);

module_t getSubmoduleStatus();
void SubmoduleFault_Handler(module_t fault_module);

#endif
