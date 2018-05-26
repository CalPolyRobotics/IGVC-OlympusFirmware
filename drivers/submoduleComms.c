#include <stdint.h>
#include "submoduleComms.h"
#include "comms.h"
#include "gpio.h"
#include "spi.h"
#include "config.h"
#include "sevenSeg.h"
#include "led.h"

#define SPI_DEFAULT_TIMEOUT 100

static void selectModule(module_t module);
static void deselectModule(module_t module);

uint8_t submoduleCommsBuff[256];

static pin_t CS_PINS[NUM_SUBMODULES] = {
    {PORT_SS_APOLLO, PIN_SS_APOLLO},
    {PORT_SS_HEPHAESTUS, PIN_SS_HEPHAESTUS},
    {PORT_SS_HERA, PIN_SS_HERA},
    {PORT_SS_JANUS, PIN_SS_JANUS},
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

/** Returns a value corresponding to the first submodule that failed status check
 * @retval: 0 - All Success
 *          1 - Apollo
 *          2 - Hephaestu
 *          3 - Hera
 *          4 - Januss
 **/
module_t getSubmoduleStatus(){
    //module_t modules_list[] = {APOLLO, HEPHAESTUS, HERA, JANUS};
    module_t modules_list[] = {HERA, JANUS};
    uint8_t return_buffer;
    int i;
    for(i = 0; i < NUM_SUBMODULES; i++){
        /* All status messages should have:
         * msg_type: 0
         * tx_size: 0
         * rx_size: 1 */
        messageSubmodule(modules_list[i], 0, &return_buffer, 0, 1);
        if(return_buffer != 0xAA){
            printf("Error code: %d", i);
            return modules_list[i];
        }
    }
    return NONE;
}

/** Uses fault status to display error code.
 * Holds in and infinite loop after outputting error code.
 *
 * @param fault_status - fault code returned form getSubModuleStatus()
 **/
void SubmoduleFault_Handler(module_t fault_module){

    switch(fault_module){
        case APOLLO:
            setSevenSeg(APOLLO_STATUS_FAIL);
            break;
        case HEPHAESTUS:
            setSevenSeg(HEPHAESTUS_STATUS_FAIL);
            break;
        case HERA:
            setSevenSeg(HERA_STATUS_FAIL);
            break;
        case JANUS:
            setSevenSeg(JANUS_STATUS_FAIL);
            break;
        case OLYMPUS:
        case NONE:
            break;
    }

    /*
    if(fault_status != 0){
        //TODO: Remove debug led usage
        writeLED(fault_status, LED_ON);
        while(1);
    }
    */
}
