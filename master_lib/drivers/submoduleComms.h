#ifndef __SUBMODULECOMMS_H__
#define __SUBMODULECOMMS_H__

#include <stdint.h>

#define SUBMODULE_START_BYTE ((uint8_t)0xEAu)

#define NUM_MODULES    5u
#define NUM_SUBMODULES 4u

#define STATUS_IDX 0u
#define STATUS_LEN 1u
#define DATA_IDX   1u

#define BOOT_SUBMODULE_TIMEOUT ((uint16_t)100u)
#define BOOT_FLASH_TIMEOUT     ((uint16_t)5000u)

#define SPI_TIMEOUT 5u

/** Error Responses **/
typedef uint8_t commsStatus_t;

#define COMMS_ERR_DATA_SIZE    ((commsStatus_t)0x00)
#define COMMS_ERR_NOT_RDY      ((commsStatus_t)0x01)
#define COMMS_ERR_INT          ((commsStatus_t)0x02)
#define COMMS_ERR_GEN          ((commsStatus_t)0x03)
#define COMMS_ERR_INV_MSG_TYPE ((commsStatus_t)0x04)
#define COMMS_ERR_TIMEOUT      ((commsStatus_t)0x05)

#define COMMS_OK               ((commsStatus_t)0xAA)

#define COMMS_ERR_NOT_IMPL     ((commsStatus_t)0xAB)

/** Note Submodules must proceed OLYMPUS **/
typedef enum module{
   APOLLO     = 0,
   HEPHAESTUS = 1,
   HERA       = 2,
   JANUS      = 3,
   OLYMPUS    = 4,
   NONE       = 255
}module_t;


extern uint8_t submoduleCommsBuff[256u];

commsStatus_t messageSubmodule(module_t module, uint8_t msg_type, uint8_t* buff, uint8_t tx_size,
                               uint8_t rx_size, uint32_t timeout);

#endif
