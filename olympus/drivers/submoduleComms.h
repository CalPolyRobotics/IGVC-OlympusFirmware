#ifndef __SUBMODULECOMMS_H__
#define __SUBMODULECOMMS_H__

#include <stdint.h>

#define SUBMODULE_START_BYTE 0xEA

#define NUM_MODULES    5
#define NUM_SUBMODULES 4

#define STATUS_IDX 0
#define DATA_IDX 1

/** Error Responses **/
typedef uint8_t commsStatus_t;

#define COMMS_ERR_DATA_SIZE    ((commsStatus_t)0x00)
#define COMMS_ERR_NOT_RDY      ((commsStatus_t)0x01)
#define COMMS_ERR_INT          ((commsStatus_t)0x02)
#define COMMS_ERR_GEN          ((commsStatus_t)0x03)
#define COMMS_ERR_INV_MSG_TYPE ((commsStatus_t)0x04)
#define COMMS_OK               ((commsStatus_t)0xAA)

/** Note Submodules must proceed OLYMPUS **/
typedef enum module{
   APOLLO     = 0,
   HEPHAESTUS = 1,
   HERA       = 2,
   JANUS      = 3,
   OLYMPUS    = 4,
   NONE       = 255
}module_t;


extern uint8_t submoduleCommsBuff[256];

void messageSubmodule(module_t module, uint8_t msg_type, uint8_t* buff, uint8_t tx_size, uint8_t rx_size);

void checkAllSubmodules();

#endif
