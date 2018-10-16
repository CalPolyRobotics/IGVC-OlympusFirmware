#include <stdlib.h>
#include "boot.h"
#include "comms.h"

static uint8_t get_status_callback(uint8_t *data);

/** tx or rx DataLengths can be no longer than 253(rx) 253(tx) **/
msgInfo_t msgResp[NUM_MSGS] = {
    {0u, 0u, get_status_callback}, // 0 Get Status
    {0u, 0u, NULL},                // 1 Unused
    {0u, 0u, NULL},                // 2 Unused
    {0u, 0u, NULL},                // 3 Unused
    {0u, 0u, NULL},                // 4 Unused
    {0u, 0u, NULL},                // 5 Unused
    {0u, 0u, NULL},                // 6 Unused
    {0u, 0u, NULL},                // 7 Unused
    {0u, 0u, NULL},                // 8 Unused
    {0u, 0u, NULL},                // 9 Unused
    {0u, 0u, bootload},            // 10 Bootloader
};

static uint8_t get_status_callback(uint8_t *data){
    return WR_OK;
}
