#include <stdlib.h>
#include "boot.h"
#include "comms.h"
#include "fnr.h"

static uint8_t errorByte;

static uint8_t dataBuf[256];

static uint8_t *get_status_callback(uint8_t *data);
static uint8_t *get_fnr_callback(uint8_t *data);
static uint8_t *set_fnr_callback(uint8_t *data);

/** tx or rx DataLengths can be no longer than 253(rx) 253(tx) **/
msgInfo_t msgResp[NUM_MSGS] = {
    {0, 1, get_status_callback}, /** 0 Get Status **/
    {0, 1, get_fnr_callback},    /** 1 Get FNR    **/
    {1, 0, set_fnr_callback},    /** 2 Set FNR    **/
    {0, 0, NULL}, /** 3 Unused **/
    {0, 0, NULL}, /** 4 Unused **/
    {0, 0, NULL}, /** 5 Unused **/
    {0, 0, NULL}, /** 6 Unused **/
    {0, 0, NULL}, /** 7 Unused **/
    {0, 0, NULL}, /** 8 Unused **/
    {0, 0, NULL}, /** 9 Unused **/
    {0, 1, bootload}, /** 10 Reset To Bootloader **/
};

static uint8_t *get_status_callback(uint8_t *data){
    errorByte = WR_OK;
    return &errorByte;
}

static uint8_t *set_fnr_callback(uint8_t *data){
    setFNR(data[0]);
    return NULL;
}

static uint8_t *get_fnr_callback(uint8_t *data){
    dataBuf[0] = getFNR();
    return dataBuf;
}
