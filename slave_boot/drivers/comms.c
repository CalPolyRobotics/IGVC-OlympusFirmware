#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "boot.h"
#include "comms.h"
#include "flash.h"
#include "spi.h"

#define ERASE_FLASH_KEY ((uint32_t)0x666C6170)

typedef struct{
    uint32_t  size;
    uint32_t  key;
}header_t;

static uint32_t size = 0u;
static uint32_t checksum = 0u;
static bool flashComplete = false;

static uint8_t get_status_callback(uint8_t *data);
static uint8_t write_header(uint8_t *data);
static uint8_t write_data(uint8_t *data);
static uint8_t run_checksum(uint8_t *data);

/** tx or rx DataLengths can be no longer than 253(rx) 253(tx) **/
const msgInfo_t msgResp[NUM_MSGS] = {
    {0,  0, get_status_callback},      /** 0 Get Status  **/
    {8,  0, write_header},             /** 1 Header      **/
    {64, 0, write_data},               /** 2 Data        **/
    {4,  0, run_checksum},             /** 3 Checksum    **/
    {0,  0, NULL},                     /** 4 Unused      **/
    {0,  0, NULL},                     /** 5 Unused      **/
    {0,  0, NULL},                     /** 6 Unused      **/
    {0,  0, NULL},                     /** 7 Unused      **/
    {0,  0, NULL},                     /** 8 Unused      **/
    {0,  0, NULL},                     /** 9 Unused      **/
    {0,  0, NULL},                     /** 10 Unused     **/
};

static uint8_t get_status_callback(uint8_t *data){
    return WR_OK;
}

static uint8_t write_header(uint8_t *data){
    /** memcpy data to avoid 32-bit address alignment issues **/
    header_t header;
    memcpy(&header, data, sizeof(header_t));

    if(header.key != ERASE_FLASH_KEY || header.size > APP_MAX_SIZE){
        return WR_ERR;
    }

    /* Initialize static variables */
    size = header.size;
    checksum = 0;
    flashComplete = false;

    /* Initialize Flash */
    writeInit(header.size);

    return WR_OK;
}

static uint8_t write_data(uint8_t *data){
    static uint32_t count = 0;
    static uint16_t* addr = (uint16_t*)APP_START_ADDR;

    if(size == 0){
        return WR_ERR;
    }

    /* Write data 16 bits at a time */
    int i;
    uint16_t val;
    bool high16bit = false;
    for(i = 0; i < msgResp[2].rxDataLength; i += sizeof(uint16_t)){
        memcpy(&val, data + i, sizeof(uint16_t));

        writeFlash(addr++, val);

        if(high16bit)
        {
            checksum += (uint32_t)val << 16u;
        }
        else
        {
            checksum += val;
        }

        /* Toggle shift to add 32-bit values */
        high16bit = !high16bit;
    }
    count += msgResp[2].rxDataLength;

    if(count >= size){
        writeComplete();
        flashComplete = true;
    }

    return WR_OK;
}

static uint8_t run_checksum(uint8_t *data){
    if(!flashComplete){
        return WR_ERR;
    }

    uint32_t hostChecksum = (uint32_t)data[3] << 24u |
                            (uint32_t)data[2] << 16u |
                            (uint32_t)data[1] << 8u |
                            (uint32_t)data[0];

    if(hostChecksum != checksum){
        return WR_ERR;
    }

    /* Write success since jumpToApp wont return */
    writeResponse(WR_OK, NULL, 0u);

    jumpToApp(APP_START_ADDR);

    /* Application should not get to this point because of jump */
    return WR_ERR;
}
