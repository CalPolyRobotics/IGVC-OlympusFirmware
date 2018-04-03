#include <stdint.h>
#include <stdlib.h>
#include "comms.h"
#include "spi.h"

#define SPI_TIMEOUT 100

/**
 * TODO
 * Initialize Interrupts for handling incoming SPI Messages
 * On interrupt, read the message, and call the appropriate handling function
 */

uint8_t* set_turn_signal_callback(uint8_t* data){
    return NULL;
}

uint8_t* set_headlights_callback(uint8_t* data){
    return NULL;
}

uint8_t* set_misc_lights_callback(uint8_t* data){
    return NULL;
}

uint8_t* get_light_sensor_callback(uint8_t* data){
    return NULL;
}

uint8_t*  bootload(uint8_t* data){
    //Software Reset
    return NULL;
}

typedef struct PIN{
    uint32_t pinMask;
    uint32_t port;
}PIN;

typedef enum COMMS_STATE{
    START_BYTE=0, MSG_TYPE, DATA
}COMMS_STATE;

typedef struct msg_info{
    uint8_t txDataLength;
    uint8_t rxDataLength;
    uint8_t* (*callback)(uint8_t* data);
}msg_info;

void handle_message(uint8_t msg_type, uint8_t* buf);
void write_response(uint8_t* data, uint8_t size);

uint8_t buf[256];

msg_info MSG_LENGTH[11] = {
    {1, 0, set_turn_signal_callback}, /** 0 Set Turn Signal **/
    {1, 0, set_headlights_callback}, /** 1 Set Headlights **/
    {2, 0, set_misc_lights_callback}, /** 2 Set Misc Lights **/
    {0, 2, get_light_sensor_callback}, /** 3 Get Light Sensor **/
    {0, 0, NULL}, /** 4 Unused **/
    {0, 0, NULL}, /** 5 Unused **/
    {0, 0, NULL}, /** 6 Unused **/
    {0, 0, NULL}, /** 7 Unused **/
    {0, 0, NULL}, /** 8 Unused **/
    {0, 0, NULL}, /** 9 Unused **/
    {0, 0, bootload}, /** 10 Reset To Bootloader **/
};


void commsFSM(uint8_t data){
    static COMMS_STATE state = START_BYTE;
    static uint8_t dataSize;
    static uint8_t dataIdx;
    static uint8_t msgType;

    switch(state){
        case START_BYTE:
            if(data == COMMS_START_BYTE){
                state = MSG_TYPE;
            }
            break;

        case MSG_TYPE:
            msgType = data;
            dataSize = MSG_LENGTH[msgType].txDataLength;
            if(dataSize == 0){
                write_response(MSG_LENGTH[msgType].callback(NULL), MSG_LENGTH[msgType].rxDataLength);
                state = START_BYTE;
            }else{
                dataIdx = 0;
                state = DATA;
            }
            break;

        case DATA:
            buf[dataIdx] = data;
            if(dataIdx == dataSize - 1){
                write_response(MSG_LENGTH[msgType].callback(buf), MSG_LENGTH[msgType].rxDataLength);
                state = START_BYTE;
            }else{
                dataIdx++;
            }
            break;
    }
}

void write_response(uint8_t* data, uint8_t size){
    if(data == NULL){
        return;
    }

    HAL_SPI_Transmit(&hspi1, data, size, SPI_TIMEOUT);
}
