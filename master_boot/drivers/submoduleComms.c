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

void messageSubmodule(module_t module, uint8_t msg_type, uint8_t* buff, uint8_t tx_size, uint8_t rx_size)
{

    uint8_t header[2] = {SUBMODULE_START_BYTE, msg_type};

    selectModule(module);

    /** Send Start Byte & Message Type **/
    HAL_SPI_Transmit(&hspi3, header, sizeof(header), SPI_DEFAULT_TIMEOUT);

    /** Transmit Data **/
    HAL_SPI_Transmit(&hspi3, buff, tx_size, SPI_DEFAULT_TIMEOUT);

    /** Delay to wait for Slave Response **/
    int i;
    for(i = 0; i < 200; i++)
    {
        asm("nop");
    }

    /* Clear out data in buffer to prevent receiving bad data */
    for(i = 0; i < rx_size; i++)
    {
        buff[i] = 0;
    }

    /** Receive Data **/
    HAL_SPI_Receive(&hspi3, buff, rx_size, SPI_DEFAULT_TIMEOUT);

    deselectModule(module);
}

/* Continuously request data from the module, checking against COMMS_OK, the
 * message is sent numTimes with delay in between each check 
 */
bool checkStatus(module_t module, int numTimes, int delay){
    int count = numTimes;
    while(count > 0){
        messageSubmodule(module, 0x00, submoduleCommsBuff, 0, 1);
        if(submoduleCommsBuff[0] == COMMS_OK)
        {
            return true;
        }
        HAL_Delay(delay);
        count--;
    }

    return false;
}

/** Lower the CS bit for the given module **/
static void selectModule(module_t module){
    HAL_GPIO_WritePin(CS_PINS[module].port, CS_PINS[module].pin_mask, GPIO_PIN_RESET);
}

/** Raise the CS bit for the given module **/
static void deselectModule(module_t module){
    HAL_GPIO_WritePin(CS_PINS[module].port, CS_PINS[module].pin_mask, GPIO_PIN_SET);
}
