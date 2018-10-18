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

static pin_t INT_PINS[NUM_SUBMODULES] = {
    {PORT_INT_APOLLO, PIN_INT_APOLLO},
    {PORT_INT_HEPHAESTUS, PIN_INT_HEPHAESTUS},
    {PORT_INT_HERA, PIN_INT_HERA},
    {PORT_INT_JANUS, PIN_INT_JANUS},
};

void SPI_Transmit(SPI_HandleTypeDef *hspi, uint8_t *pData, uint16_t size, uint32_t timeout)
{
    if ((pData == NULL) || (size == 0U))
    {
        return;
    }

    uint32_t tickend = HAL_GetTick() + timeout;
    __HAL_SPI_1LINE_TX(hspi);

    while (size > 0u && HAL_GetTick() <= tickend)
    {
        /* Wait until TXE flag is set to send data */
        if (__HAL_SPI_GET_FLAG(hspi, SPI_FLAG_TXE))
        {
            *((__IO uint8_t *)&hspi->Instance->DR) = (*pData++);
            size--;
        }
    }
}


void SPI_Receive(SPI_HandleTypeDef *hspi, uint8_t *pData, uint16_t size, uint32_t timeout)
{
    if ((pData == NULL) || (size == 0U))
    {
        return;
    }

    uint32_t tickend = HAL_GetTick() + timeout;

    /* Transfer loop */
    while (size > 0u && HAL_GetTick() <= tickend)
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

    __HAL_SPI_1LINE_TX(hspi);
}

uint8_t writeSubmodule(module_t module, uint8_t msg_type, uint8_t* buff, uint8_t tx_size, uint32_t timeout)
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

    SPI_Receive(&hspi3, buff, 1u, timeout);

    /** Wait for slave device to finish sending data back **/
    timeUp = HAL_GetTick() + timeout;
    while(HAL_GPIO_ReadPin(INT_PINS[module].port, INT_PINS[module].pin_mask) != GPIO_PIN_RESET &&
          HAL_GetTick() < timeUp);


    deselectModule(module);

    return buff[0u];
}

/** Lower the CS bit for the given module **/
static void selectModule(module_t module){
    HAL_GPIO_WritePin(CS_PINS[module].port, CS_PINS[module].pin_mask, GPIO_PIN_RESET);
}

/** Raise the CS bit for the given module **/
static void deselectModule(module_t module){
    HAL_GPIO_WritePin(CS_PINS[module].port, CS_PINS[module].pin_mask, GPIO_PIN_SET);
}
