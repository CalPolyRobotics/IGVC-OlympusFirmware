#include <stdlib.h>
#include "boot.h"
#include "comms.h"
#include "string.h"
#include "adc.h"
#include "steer.h"
#include "steer_speed.h"

static uint8_t get_status_callback(uint8_t *data);
static uint8_t set_steer_callback(uint8_t *data);
static uint8_t get_steer_pot_callback(uint8_t *data);
static uint8_t get_steer_speed_callback(uint8_t *data);
static uint8_t set_brake_callback(uint8_t *data);
static uint8_t get_brake_pot_callback(uint8_t *data);

#define S_STEER_IDX     ((uint32_t)1u)
#define G_STEER_POT_IDX ((uint32_t)2u)
#define G_STEER_SPD_IDX ((uint32_t)3u)
#define S_BRAKE_IDX     ((uint32_t)4u)
#define G_BRAKE_POT_IDX ((uint32_t)5u)

/** tx or rx DataLengths can be no longer than 253(rx) 253(tx) **/
msgInfo_t msgResp[NUM_MSGS] = {
    {0u, 0u, get_status_callback},      // 0 Get Status
    {1u, 0u, set_steer_callback},       // 1 Set Steering Angle
    {0u, 2u, get_steer_pot_callback},   // 2 Get Steer Pot
    {0u, 2u, get_steer_speed_callback}, // 3 Get Steer Speed
    {2u, 0u, set_brake_callback},       // 4 Set Brake 
    {0u, 2u, get_brake_pot_callback},   // 5 Get Brake Pot
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


static uint8_t get_steer_pot_callback(uint8_t *data)
{
    memcpy(data, &adcChnAvg[adcChnReadIdx][STEER_AVG_IDX],
           msgResp[G_STEER_POT_IDX].txDataLength);

    return WR_OK;
}

static uint8_t get_steer_speed_callback(uint8_t *data)
{ 
    memcpy(data, &ch3Speed[(ch3SpeedWr + 1u) % PING_PONG_SIZE],
           msgResp[G_STEER_SPD_IDX].txDataLength);

    return WR_OK;
}

static uint8_t set_brake_callback(uint8_t *data)
{
    return WR_ERR;
}

static uint8_t get_brake_pot_callback(uint8_t *data)
{
    memcpy(data, &adcChnAvg[adcChnReadIdx][BRAKE_AVG_IDX],
           msgResp[G_BRAKE_POT_IDX].txDataLength);

    return WR_OK;
}
