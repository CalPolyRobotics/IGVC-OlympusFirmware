#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "stm32f2xx_hal.h"
#include "submoduleComms.h"
#include "gpio.h"
#include "spi.h"

#define SPI_DEFAULT_TIMEOUT 100
#define STATUS_MAX_ATTEMPTS 20

static void selectModule(module_t module);
static void deselectModule(module_t module);

uint8_t submoduleCommsBuff[256];
uint8_t dummyData[256] = {0};

typedef struct pin{
    GPIO_TypeDef* port;
    uint16_t pin_mask;
}pin_t;

static pin_t CS_PINS[NUM_SUBMODULES] = {
    {PORT_SS_APOLLO, PIN_SS_APOLLO},
    {PORT_SS_HEPHAESTUS, PIN_SS_HEPHAESTUS},
    {PORT_SS_HERA, PIN_SS_HERA},
    {PORT_SS_JANUS, PIN_SS_JANUS},
};

static uint8_t statusTimeout(uint32_t timeout)
{
    uint8_t header[2] = {SUBMODULE_START_BYTE, 0x00};

    uint32_t timeUp = HAL_GetTick() + timeout;
    uint8_t resp = 0x00u;


    do{
        HAL_SPI_TransmitReceive(&hspi3, header, dummyData, sizeof(header), SPI_DEFAULT_TIMEOUT);

        /** Clear Possible Write to dummyData **/
        dummyData[0] = 0u;

        /** Delay to wait for slave response **/
        int i;
        for(i = 0; i < 200; i++)
        {
            asm("nop");
        }

        HAL_SPI_TransmitReceive(&hspi3, dummyData, &resp, sizeof(resp), SPI_DEFAULT_TIMEOUT);

    }while(resp != COMMS_OK && HAL_GetTick() < timeUp);

    if(resp != COMMS_OK)
    {
        return COMMS_ERR_TIMEOUT;
    }

    return COMMS_OK;
}

uint8_t writeSubmodule(module_t module, uint8_t msg_type, uint8_t* buff, uint8_t tx_size, uint32_t timeout)
{

    uint8_t header[2] = {SUBMODULE_START_BYTE, msg_type};

    selectModule(module);

    /** Send Start Byte & Message Type **/
    HAL_SPI_TransmitReceive(&hspi3, header, dummyData, sizeof(header), SPI_DEFAULT_TIMEOUT);

    /** Transmit Data **/
    HAL_SPI_TransmitReceive(&hspi3, buff, dummyData, tx_size, SPI_DEFAULT_TIMEOUT);

    /** Delay to wait for slave response **/
    int i;
    for(i = 0; i < 200; i++)
    {
        asm("nop");
    }

    dummyData[0] = 0u;

    uint8_t resp = 0x00u;
    HAL_SPI_TransmitReceive(&hspi3, dummyData, &resp, 1u, SPI_DEFAULT_TIMEOUT);

    if(resp != COMMS_OK)
    {
        resp = statusTimeout(timeout);
    }

    deselectModule(module);

    return resp;
}

/** Lower the CS bit for the given module **/
static void selectModule(module_t module){
    HAL_GPIO_WritePin(CS_PINS[module].port, CS_PINS[module].pin_mask, GPIO_PIN_RESET);
}

/** Raise the CS bit for the given module **/
static void deselectModule(module_t module){
    HAL_GPIO_WritePin(CS_PINS[module].port, CS_PINS[module].pin_mask, GPIO_PIN_SET);
}
