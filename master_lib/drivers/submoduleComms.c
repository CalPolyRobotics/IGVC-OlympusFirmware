#include <stdint.h>
#include <stdio.h>

#include "submoduleComms.h"
#include "gpio.h"
#include "spi.h"

#define SPI_DEFAULT_TIMEOUT 100
#define STATUS_MAX_ATTEMPTS 20

static void selectModule(module_t module);
static void deselectModule(module_t module);

uint8_t submoduleCommsBuff[256];
uint8_t dmy[256];

static pin_t CS_PINS[] = {
    {APOL_SS_PRT, APOL_SS_PIN},
    {HEPH_SS_PRT, HEPH_SS_PIN},
    {HERM_SS_PRT, HERM_SS_PIN},
    {JANU_SS_PRT, JANU_SS_PIN},
    {ZEUS_SS_PRT, ZEUS_SS_PIN},
};

static pin_t INT_PINS[] = {
    {APOL_IT_PRT, APOL_IT_PIN},
    {HEPH_IT_PRT, HEPH_IT_PIN},
    {HERM_IT_PRT, HERM_IT_PIN},
    {JANU_IT_PRT, JANU_IT_PIN},
    {ZEUS_IT_PRT, ZEUS_IT_PIN},
};


commsStatus_t messageSubmodule(module_t module, uint8_t msg_type, uint8_t* buff, uint8_t tx_size,
                               uint8_t rx_size, uint32_t timeout)
{
    uint8_t header[2] = {SUBMODULE_START_BYTE, msg_type};

    selectModule(module);

    /** Send Start Byte & Message Type **/
    HAL_SPI_TransmitReceive(&hspi3, header, dmy, sizeof(header), timeout);

    /** Transmit Data **/
    if(tx_size){
        HAL_SPI_TransmitReceive(&hspi3, buff, dmy, tx_size, timeout);
    }

    /** Wait for slave device to finish processing data **/
    uint32_t timeUp = HAL_GetTick() + timeout;
    while(HAL_GPIO_ReadPin(INT_PINS[module].port, INT_PINS[module].pin_mask) != GPIO_PIN_SET &&
          HAL_GetTick() < timeUp);

    if(HAL_GetTick() >= timeUp){
        deselectModule(module);
        return COMMS_ERR_TIMEOUT;
    }

    /** Receive Data **/
    uint8_t status;
    HAL_SPI_Receive(&hspi3, &status, STATUS_LEN, timeout);

    if(rx_size){
        HAL_SPI_Receive(&hspi3, buff, rx_size, timeout);
    }

    /** Wait for slave device to finish sending data back **/
    timeUp = HAL_GetTick() + timeout;
    while(HAL_GPIO_ReadPin(INT_PINS[module].port, INT_PINS[module].pin_mask) != GPIO_PIN_RESET &&
          HAL_GetTick() < timeUp);

    deselectModule(module);

    return status;
}

/** Lower the CS bit for the given module **/
static void selectModule(module_t module){
    HAL_GPIO_WritePin(CS_PINS[module].port, CS_PINS[module].pin_mask, GPIO_PIN_RESET);
}

/** Raise the CS bit for the given module **/
static void deselectModule(module_t module){
    HAL_GPIO_WritePin(CS_PINS[module].port, CS_PINS[module].pin_mask, GPIO_PIN_SET);
}
