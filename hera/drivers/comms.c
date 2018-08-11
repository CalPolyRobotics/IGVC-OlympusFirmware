#include <stdlib.h>
#include "boot.h"
#include "comms.h"

static uint8_t errorByte;

static uint8_t *get_status_callback(uint8_t *data);

/** tx or rx DataLengths can be no longer than 253(rx) 253(tx) **/
msgInfo_t msgResp[NUM_MSGS] = {
    {0, 1, get_status_callback}, /** 0 Get Status **/
    {0, 0, NULL}, /** 1 Unused **/
    {0, 0, NULL}, /** 2 Unused **/
    {0, 0, NULL}, /** 3 Unused **/
    {0, 0, NULL}, /** 4 Unused **/
    {0, 0, NULL}, /** 5 Unused **/
    {0, 0, NULL}, /** 6 Unused **/
    {0, 0, NULL}, /** 7 Unused **/
    {0, 0, NULL}, /** 8 Unused **/
    {0, 0, NULL}, /** 9 Unused **/
    {0, 0, bootload}, /** 10 Reset To Bootloader **/
};

static uint8_t *get_status_callback(uint8_t *data){
    errorByte = WR_OK;
    return &errorByte;
}
