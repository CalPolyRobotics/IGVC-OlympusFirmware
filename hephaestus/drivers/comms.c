#include <stdlib.h>
#include "boot.h"
#include "comms.h"
#include "string.h"
#include "steer.h"

static uint8_t get_status_callback(uint8_t *data);
static uint8_t get_status_callback(uint8_t *data);
static uint8_t set_steer_callback(uint8_t *data);
static uint8_t update_steerpot_callback(uint8_t *data);

/** tx or rx DataLengths can be no longer than 253(rx) 253(tx) **/
msgInfo_t msgResp[NUM_MSGS] = {
    {0u, 0u, get_status_callback},      // 0 Get Status
    {1u, 0u, set_steer_callback},       // 1 Set Steering Angle
    {2u, 0u, update_steerpot_callback}, // 2 Update Steerpot Value
    {0u, 0u, NULL},                     // 3 Unused
    {0u, 0u, NULL},                     // 4 Unused
    {0u, 0u, NULL},                     // 5 Unused
    {0u, 0u, NULL},                     // 6 Unused
    {0u, 0u, NULL},                     // 7 Unused
    {0u, 0u, NULL},                     // 8 Unused
    {0u, 0u, NULL},                     // 9 Unused
    {0u, 0u, bootload},                 // 10 Bootloader
};

static uint8_t get_status_callback(uint8_t *data){
    return WR_OK;
}

static uint8_t set_steer_callback(uint8_t *data)
{
    if(*data > MAX_RIGHT_DEG || *data < MAX_LEFT_DEG)
    {
        return WR_ERR_INV_ARG;
    }

    set_steer_target(*data);
    return WR_OK;
}

static uint8_t update_steerpot_callback(uint8_t *data)
{
    uint16_t tmp;
    memcpy(&tmp, data, sizeof(uint16_t));

    if(tmp > MAX_RIGHT_POT_CHECK || tmp < MAX_LEFT_POT_CHECK){
        return WR_ERR_INV_ARG;
    }

    steeringPot = tmp;
    return WR_OK;
}
