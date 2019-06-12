#include <stdlib.h>
#include <string.h>

#include "stm32f0xx_hal.h"

#include "boot.h"
#include "comms.h"
#include "gpio.h"
#include "speed.h"

static uint8_t get_status_callback(uint8_t *data);
static uint8_t get_aman_callback(uint8_t *data);
static uint8_t get_speed_callback(uint8_t *data);

#define AMAN_IDX    ((uint32_t)1u)
#define SPEED_IDX   ((uint32_t)2u)

/** tx or rx DataLengths can be no longer than 253(rx) 253(tx) **/
msgInfo_t msgResp[NUM_MSGS] = {
    {0u, 0u, get_status_callback}, // 0 Get Status
    {0u, 1u, get_aman_callback},   // 1 Get Automan
    {0u, 0u, get_speed_callback},  // 2 Get Speed
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

static uint8_t get_aman_callback(uint8_t *data){
    data[0] = get_automan_enabled();

    return WR_OK;
}

static uint8_t get_speed_callback(uint8_t *data){
    int16_t speed = ch1Speed[(ch1SpeedWr + 1u) % PING_PONG_SIZE];

    data[0] = speed & 0xFF;
    data[1] = (speed >> 8u) & 0xFF;

    return WR_OK;
}
