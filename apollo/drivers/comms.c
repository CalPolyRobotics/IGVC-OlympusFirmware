#include <stdlib.h>
#include "boot.h"
#include "comms.h"
#include "lights.h"

static uint8_t get_status_callback(uint8_t *data);
static uint8_t set_turn_signal_callback(uint8_t *data);
static uint8_t set_headlights_callback(uint8_t *data);
static uint8_t set_misc_lights_callback(uint8_t *data);

/** tx or rx DataLengths can be no longer than 253(rx) 253(tx) **/
msgInfo_t msgResp[NUM_MSGS] = {
    {0u, 0u, get_status_callback},      // 0 Get Status
    {1u, 0u, set_turn_signal_callback}, // 1 Set Turn Signal
    {1u, 0u, set_headlights_callback},  // 2 Set Headlights
    {2u, 0u, set_misc_lights_callback}, // 3 Set Misc Lights
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

static uint8_t set_turn_signal_callback(uint8_t *data){
    set_turn_signal(*data);
    return WR_OK;
}

static uint8_t set_headlights_callback(uint8_t *data){
    set_headlights(*data);
    return WR_OK;
}

static uint8_t set_misc_lights_callback(uint8_t *data){
    set_misc_lights(data[0u], data[1u], data[2u]);
    return WR_OK;
}
