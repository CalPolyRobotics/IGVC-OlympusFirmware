#include <stdlib.h>
#include <stdbool.h>
#include "boot.h"
#include "comms.h"
#include "flash.h"

#define ERASE_FLASH_KEY ((uint32_t)0x666C6170)

typedef struct{
    uint32_t  size;
    uint32_t  key;
}header_t;

static uint8_t errorByte;
static uint32_t size;
static bool flashComplete = false;

static uint8_t *get_status_callback(uint8_t *data);
static uint8_t *write_header(uint8_t *data);
static uint8_t *write_data(uint8_t *data);
static uint8_t *run_checksum(uint8_t *data);
static uint8_t *jump_to_app(uint8_t *data);

/** tx or rx DataLengths can be no longer than 253(rx) 253(tx) **/
msgInfo_t msgResp[NUM_MSGS] = {
    {0,  1, get_status_callback},      /** 0 Get Status  **/
    {8,  1, write_header},             /** 1 Header      **/
    {64, 1, write_data},               /** 2 Data        **/
    {4,  1, run_checksum},             /** 3 Checksum    **/
    {0,  1, jump_to_app},              /** 4 Jump To App **/
    {0,  0, NULL},                     /** 5 Unused      **/
    {0,  0, NULL},                     /** 6 Unused      **/
    {0,  0, NULL},                     /** 7 Unused      **/
    {0,  0, NULL},                     /** 8 Unused      **/
    {0,  0, NULL},                     /** 9 Unused      **/
    {0,  0, NULL},                     /** 10 Unused     **/
};

static uint8_t *get_status_callback(uint8_t *data){
    errorByte = WR_OK;
    return &errorByte;
}

static uint8_t *write_header(uint8_t *data){
    header_t header = *((header_t*)data);

    if(header.key != ERASE_FLASH_KEY){
        errorByte = WR_ERR;
        return &errorByte;
    }

    if(header.size > APP_MAX_SIZE){
        errorByte = WR_ERR;
        return &errorByte;
    }

    size = header.size;

    /* Initialize Flash */
    writeInit(header.size);

    errorByte = WR_OK;
    return &errorByte;
}

static uint8_t *write_data(uint8_t *data){
    static uint32_t count = 0;
    static uint16_t* addr = (uint16_t*)APP_START_ADDR;

    if(size == 0){
        errorByte = WR_ERR;
        return &errorByte;
    }

    /* Write data 16 bits at a time */
    int i;
    for(i = 0; i < 64u; i+=2u){
        uint16_t val = ((uint16_t)data[1 + i] << 8u)  | (uint16_t)data[0 + i];
        writeFlash(addr++, val);
    }
    count += msgResp[2].rxDataLength;

    if(count >= size){
        writeComplete();
        flashComplete = true;
    }

    errorByte = WR_OK;
    return &errorByte;
}

static uint8_t *run_checksum(uint8_t *data){
    if(!flashComplete){
        errorByte = WR_ERR;
        return &errorByte;
    }

    errorByte = WR_OK;
    return &errorByte;
}

static uint8_t *jump_to_app(uint8_t *data){
    errorByte = WR_OK;
    return &errorByte;
}
