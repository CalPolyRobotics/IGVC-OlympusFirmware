#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "submoduleComms.h"
#include "comms.h"
#include "gpio.h"
#include "spi.h"
#include "config.h"
#include "sevenSeg.h"
#include "led.h"
#include "apollo.h"
#include "janus.h"
#include "hera.h"
#include "hephaestus.h"

#define SPI_DEFAULT_TIMEOUT 100
#define STATUS_MAX_ATTEMPTS 20

static void selectModule(module_t module);
static void deselectModule(module_t module);
static bool checkSubmoduleStatus(module_t module);

uint8_t submoduleCommsBuff[256];

static pin_t CS_PINS[NUM_SUBMODULES] = {
    {PORT_SS_APOLLO, PIN_SS_APOLLO},
    {PORT_SS_HEPHAESTUS, PIN_SS_HEPHAESTUS},
    {PORT_SS_HERA, PIN_SS_HERA},
    {PORT_SS_JANUS, PIN_SS_JANUS},
};

void messageSubmodule(module_t module, uint8_t msg_type, uint8_t* buff, uint8_t tx_size, uint8_t rx_size)
{

    uint8_t header[2] = {SUBMODULE_START_BYTE, msg_type};

    selectModule(module);

    /** Send Start Byte & Message Type **/
    HAL_SPI_Transmit(&hspi3, header, sizeof(header), SPI_DEFAULT_TIMEOUT);

    /** Delay to wait for Slave Response **/
    int i;
    for(i = 0; i < 200; i++)
    {
        asm("nop");
    }

    /** Transmit Data **/
    HAL_SPI_Transmit(&hspi3, buff, tx_size, SPI_DEFAULT_TIMEOUT);

    /** Receive Data **/
    HAL_SPI_Receive(&hspi3, buff, rx_size, SPI_DEFAULT_TIMEOUT);

    deselectModule(module);
}

void checkAllSubmodules()
{
    if(!checkSubmoduleStatus(APOLLO))
    {
        printf("Status check failed for Apollo\r\n");
        setSevenSeg(APOLLO_STATUS_FAIL);
    }

    if(!checkSubmoduleStatus(HEPHAESTUS))
    {
        printf("Status check failed for Hephaestus\r\n");
        setSevenSeg(HEPHAESTUS_STATUS_FAIL);
    }

    if(!checkSubmoduleStatus(HERA))
    {
        printf("Status check failed for Hera\r\n");
        setSevenSeg(HERA_STATUS_FAIL);
    }

    if(!checkSubmoduleStatus(JANUS))
    {
        printf("Status check failed for Janus\r\n");
        setSevenSeg(JANUS_STATUS_FAIL);
    }
}

/**
 * Returns whether or not the given module responded with a ready status byte
 * @retval: true  - Received valid status
 *          false - Read timed out
 **/
static bool checkSubmoduleStatus(module_t module)
{
    commsStatus_t (*getStatus)(void);

    switch(module)
    {
        case APOLLO:
            getStatus = getApolloStatus;
            break;
        case HEPHAESTUS:
            getStatus = getHephaestusStatus;
            break;
        case HERA:
            getStatus = getHeraStatus;
            break;
        case JANUS:
            getStatus = getJanusStatus;
            break;
        default:
            return false;
    }


    int attempts = 0;
    while (getStatus() != COMMS_OK && attempts < STATUS_MAX_ATTEMPTS)
    {
        attempts++;
    }

    return attempts != STATUS_MAX_ATTEMPTS;
}

/** Lower the CS bit for the given module **/
static void selectModule(module_t module){
    HAL_GPIO_WritePin(CS_PINS[module].port, CS_PINS[module].pin_mask, GPIO_PIN_RESET);
}

/** Raise the CS bit for the given module **/
static void deselectModule(module_t module){
    HAL_GPIO_WritePin(CS_PINS[module].port, CS_PINS[module].pin_mask, GPIO_PIN_SET);
}
