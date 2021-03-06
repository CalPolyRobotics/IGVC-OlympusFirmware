#ifndef COMMSLIB_H
#define COMMSLIB_H

#include <stdint.h>

#define COMMS_START_BYTE ((uint8_t)0xEAu)
#define NUM_MSGS 11u

#define BOOT_MSG   ((uint8_t)0x0Au)

#define STATUS_IDX ((uint8_t)0x00u)
#define STATUS_LEN ((uint8_t)0x01u)

/** Error Response on Writes **/
typedef uint8_t wrError_t;

#define WR_ERR_DATA_SIZE    ((wrError_t)0x00)
#define WR_ERR_NOT_RDY      ((wrError_t)0x01)
#define WR_ERR_INT          ((wrError_t)0x02)
#define WR_ERR_GEN          ((wrError_t)0x03)
#define WR_ERR_INV_MSG_TYPE ((wrError_t)0x04)
#define WR_ERR              ((wrError_t)0x05)
#define WR_NO_DATA          ((wrError_t)0x06)
#define WR_BUFF_FULL        ((wrError_t)0x07)
#define WR_ERR_INV_ARG      ((wrError_t)0x08)
#define WR_OK               ((wrError_t)0xAA)

/** Message Info Struct **/
typedef struct pktInfo{
    uint8_t rxDataLength;
    uint8_t txDataLength;
    uint8_t (*callback)(uint8_t* data);
}msgInfo_t;

wrError_t runCommsFSM(uint8_t data);
#endif
