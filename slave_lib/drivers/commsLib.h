#ifndef COMMSLIB_H
#define COMMSLIB_H

#include <stdint.h>

#define COMMS_START_BYTE ((uint8_t)0xEA)
#define NUM_MSGS 11

/** TODO - This might need to be modified **/
#define SPI_TIMEOUT 100

/** Error Response on Writes **/
typedef uint8_t wrError_t;

#define WR_ERR_DATA_SIZE    ((wrError_t)0x00)
#define WR_ERR_NOT_RDY      ((wrError_t)0x01)
#define WR_ERR_INT          ((wrError_t)0x02)
#define WR_ERR_GEN          ((wrError_t)0x03)
#define WR_ERR_INV_MSG_TYPE ((wrError_t)0x04)
#define WR_ERR              ((wrError_t)0x05)
#define WR_OK               ((wrError_t)0xAA)

/** Message Info Struct **/
typedef struct pktInfo{
    uint8_t rxDataLength;
    uint8_t txDataLength;
    uint8_t* (*callback)(uint8_t* data);
}msgInfo_t;

wrError_t runCommsFSM(uint8_t data);
#endif
