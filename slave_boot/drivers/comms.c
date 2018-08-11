#include <stdlib.h>
#include "boot.h"
#include "comms.h"

#define ERASE_FLASH_KEY ((uint32_t)0x666C6170)

typedef struct{
    uint32_t  size;
    uint32_t  key;
}header_t;

static uint8_t errorByte;
static uint32_t size;

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

#include "stm32f0xx_hal.h"
static uint8_t *get_status_callback(uint8_t *data){
    errorByte = WR_OK;
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);
    return &errorByte;
}

static uint8_t *write_header(uint8_t *data){
    header_t header = *((header_t*)data);


    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);

    if(header.key != ERASE_FLASH_KEY){
        errorByte = WR_ERR;
        return &errorByte;
    }

    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);

    size = header.size;
    errorByte = WR_OK;
    return &errorByte;
}

static uint8_t *write_data(uint8_t *data){
    errorByte = WR_OK;
    return &errorByte;
}

static uint8_t *run_checksum(uint8_t *data){
    errorByte = WR_OK;
    return &errorByte;
}

static uint8_t *jump_to_app(uint8_t *data){
    errorByte = WR_OK;
    return &errorByte;
}
