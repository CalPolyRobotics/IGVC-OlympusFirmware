#include <stdint.h>
#include "submoduleComms.h"
#include "comms.h"
#include "gpio.h"
#include "spi.h"

#define SPI_DEFAULT_TIMEOUT 100

static void selectModule(module_t module);
static void deselectModule(module_t module);

uint8_t submoduleCommsBuff[256];

typedef struct pin{
    GPIO_TypeDef* port;
    uint16_t pin_mask;
}pin_t;

static pin_t CS_PINS[NUM_SUBMODULES] = {
    {GPIOA, GPIO_PIN_4}, /* APOLLO CS PIN */
    {GPIOA, GPIO_PIN_4}, /* TODO: HEPHAESTUS CS PIN */
    {GPIOA, GPIO_PIN_4}, /* TODO: HERA CS PIN */
    {GPIOA, GPIO_PIN_4}, /* TODO: JANUS CS PIN */
};

void messageSubmodule(module_t module, uint8_t msg_type, uint8_t* buff, uint8_t tx_size, uint8_t rx_size)
{

    uint8_t start_byte = SUBMODULE_START_BYTE;
    
    selectModule(module);

    /** Send Start Byte **/
    HAL_SPI_Transmit(&hspi3, &start_byte, 1, SPI_DEFAULT_TIMEOUT);

    /** Send Stop Byte **/
    HAL_SPI_Transmit(&hspi3, &msg_type, 1, SPI_DEFAULT_TIMEOUT); 

    /** Transmit Data **/
    /** TODO - Transfer to DMA **/
    HAL_SPI_Transmit(&hspi3, buff, tx_size, 500);

    /** Read Response Data **/
    HAL_SPI_Receive(&hspi3, (uint8_t*)submoduleCommsBuff, rx_size, SPI_DEFAULT_TIMEOUT);

    deselectModule(module);
}

/** Lower the CS bit for the given module **/
static void selectModule(module_t module){
    HAL_GPIO_WritePin(CS_PINS[module].port, CS_PINS[module].pin_mask, GPIO_PIN_RESET);
}

/** Raise the CS bit for the given module **/
static void deselectModule(module_t module){
    HAL_GPIO_WritePin(CS_PINS[module].port, CS_PINS[module].pin_mask, GPIO_PIN_SET);
}

