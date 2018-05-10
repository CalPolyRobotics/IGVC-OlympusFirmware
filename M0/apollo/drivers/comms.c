#include <stdlib.h>
#include "boot.h"
#include "comms.h"
#include "lights.h"


static uint8_t errorByte;

static uint8_t *set_turn_signal_callback(uint8_t *data);
static uint8_t *set_headlights_callback(uint8_t *data);
static uint8_t *set_misc_lights_callback(uint8_t *data);

/** tx or rx DataLengths can be no longer than 253(rx) 253(tx) **/
msgInfo_t msgResp[NUM_MSGS] = {
    {1, 1, set_turn_signal_callback}, /** 0 Set Turn Signal **/
    {1, 1, set_headlights_callback}, /** 1 Set Headlights **/
    {2, 1, set_misc_lights_callback}, /** 2 Set Misc Lights **/
    {0, 0, NULL}, /** 3 Unused **/
    {0, 0, NULL}, /** 4 Unused **/
    {0, 0, NULL}, /** 5 Unused **/
    {0, 0, NULL}, /** 6 Unused **/
    {0, 0, NULL}, /** 7 Unused **/
    {0, 0, NULL}, /** 8 Unused **/
    {0, 0, NULL}, /** 9 Unused **/
    {0, 0, bootload}, /** 10 Reset To Bootloader **/
};

uint8_t* set_turn_signal_callback(uint8_t *data){
    errorByte = WR_OK;
    set_turn_signal(*data);
    return &errorByte;
}

uint8_t* set_headlights_callback(uint8_t *data){
    errorByte = WR_OK;
    set_headlights(*data);
    return &errorByte;
}

uint8_t* set_misc_lights_callback(uint8_t *data){
    errorByte = WR_OK;
    set_misc_lights(data[0], data[1], data[2]);
    return &errorByte;
}
