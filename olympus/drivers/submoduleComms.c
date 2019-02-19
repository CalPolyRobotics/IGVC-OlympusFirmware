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
#include "error.h"

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

static pin_t INT_PINS[NUM_SUBMODULES] = {
    {PORT_INT_APOLLO, PIN_INT_APOLLO},
    {PORT_INT_HEPHAESTUS, PIN_INT_HEPHAESTUS},
    {PORT_INT_HERA, PIN_INT_HERA},
    {PORT_INT_JANUS, PIN_INT_JANUS},
};


void SPI_Transmit(SPI_HandleTypeDef *hspi, uint8_t *pData, uint16_t size, uint32_t timeout)
{
    uint32_t tickend = HAL_GetTick() + timeout;

    if ((pData == NULL) || (size == 0U))
    {
        return;
    }

    __HAL_SPI_1LINE_TX(hspi);

    while (size > 0u)
    {
        /* Wait until TXE flag is set to send data */
        if (__HAL_SPI_GET_FLAG(hspi, SPI_FLAG_TXE))
        {
            *((__IO uint8_t *)&hspi->Instance->DR) = (*pData++);
            size--;
        }
        else
        {
            /* Timeout management */
            if (HAL_GetTick() >= tickend)
            {
                return;
            }
        }
    }
}


void SPI_Receive(SPI_HandleTypeDef *hspi, uint8_t *pData, uint16_t size, uint32_t timeout)
{
    if ((pData == NULL) || (size == 0u))
    {
        return;
    }

    uint32_t tickend = HAL_GetTick() + timeout;

    /* Transfer loop */
    __disable_irq();
    while (size > 0u && HAL_GetTick() < tickend)
    {
        __HAL_SPI_1LINE_RX(hspi);

        /* Check the RXNE flag */
        if (__HAL_SPI_GET_FLAG(hspi, SPI_FLAG_RXNE))
        {
            __HAL_SPI_1LINE_TX(hspi);

            /* read the received data */
            (* (uint8_t *)pData) = *(__IO uint8_t *)&hspi->Instance->DR;
            pData += sizeof(uint8_t);
            size--;
        }
    }
    __enable_irq();
}

commsStatus_t messageSubmodule(module_t module, uint8_t msg_type, uint8_t* buff, uint8_t tx_size,
                               uint8_t rx_size, uint32_t timeout)
{
    uint8_t header[2] = {SUBMODULE_START_BYTE, msg_type};

    selectModule(module);

    /** Send Start Byte & Message Type **/
    SPI_Transmit(&hspi3, header, sizeof(header), timeout);

    /** Transmit Data **/
    SPI_Transmit(&hspi3, buff, tx_size, timeout);

    /** Wait for slave device to finish processing data **/
    uint32_t timeUp = HAL_GetTick() + timeout;
    while(HAL_GPIO_ReadPin(INT_PINS[module].port, INT_PINS[module].pin_mask) != GPIO_PIN_SET &&
          HAL_GetTick() < timeUp);

    /** Receive Data **/
    uint8_t status;
    SPI_Receive(&hspi3, &status, STATUS_LEN, timeout);
    SPI_Receive(&hspi3, buff, rx_size, timeout);

    /** Wait for slave device to finish sending data back **/
    timeUp = HAL_GetTick() + timeout;
    while(HAL_GPIO_ReadPin(INT_PINS[module].port, INT_PINS[module].pin_mask) != GPIO_PIN_RESET &&
          HAL_GetTick() < timeUp);

    deselectModule(module);

    return status;
}

void checkAllSubmodules()
{
    if(!checkSubmoduleStatus(APOLLO))
    {
        printf("Status check failed for Apollo\r\n");
        ErrorHandler(APOLLO_STATUS_FAIL, NOTIFY);
    }

    if(!checkSubmoduleStatus(HEPHAESTUS))
    {
        printf("Status check failed for Hephaestus\r\n");
        ErrorHandler(HEPHAESTUS_STATUS_FAIL, NOTIFY);
    }

    if(!checkSubmoduleStatus(HERA))
    {
        printf("Status check failed for Hera\r\n");
        ErrorHandler(HERA_STATUS_FAIL, NOTIFY);
    }

    if(!checkSubmoduleStatus(JANUS))
    {
        printf("Status check failed for Janus\r\n");
        ErrorHandler(JANUS_STATUS_FAIL, NOTIFY);
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

    return getStatus() == COMMS_OK;
}

/** Lower the CS bit for the given module **/
static void selectModule(module_t module){
    HAL_GPIO_WritePin(CS_PINS[module].port, CS_PINS[module].pin_mask, GPIO_PIN_RESET);
}

/** Raise the CS bit for the given module **/
static void deselectModule(module_t module){
    HAL_GPIO_WritePin(CS_PINS[module].port, CS_PINS[module].pin_mask, GPIO_PIN_SET);
}
