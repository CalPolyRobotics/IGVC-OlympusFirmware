#include <stdlib.h>
#include <string.h>

#include "adc.h"
#include "boot.h"
#include "comms.h"

static uint8_t get_status_callback(uint8_t *data);
static uint8_t get_speed_callback(uint8_t *data);
static uint8_t get_steer_callback(uint8_t *data);
static uint8_t get_sonar_callback(uint8_t *data);

#define SPEED_IDX   ((uint32_t)1u)
#define STEER_IDX   ((uint32_t)2u)
#define SONAR_IDX   ((uint32_t)3u)

/** tx or rx DataLengths can be no longer than 253(rx) 253(tx) **/
msgInfo_t msgResp[NUM_MSGS] = {
    {0u, 0u, get_status_callback}, // 0 Get Status
    {0u, 4u, get_speed_callback},  // 1 Get Speed
    {0u, 2u, get_steer_callback},  // 2 Get Steer
    {0u, 8u, get_sonar_callback},  // 3 Get Sonar
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

static uint8_t get_speed_callback(uint8_t *data){
    /* TODO - Implement Speed Data */
    memset(data, 0x55u, msgResp[SPEED_IDX].txDataLength);

    return WR_OK;
}

static uint8_t get_steer_callback(uint8_t *data){
    memcpy(data, &adcChnAvg[STEER_AVG_IDX], msgResp[STEER_IDX].txDataLength);

    return WR_OK;
}

static uint8_t get_sonar_callback(uint8_t *data){
    /* TODO - Implement Sonar Data */
    memset(data, 0x55u, msgResp[SONAR_IDX].txDataLength);

    return WR_OK;
}
