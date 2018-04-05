#include <stdint.h>
#include "moduleComms.h"
#include "apollo.h"
#include "gpio.h"
#include "spi.h"

#define START_BYTE 0xEA
#define SPI_DEFAULT_TIMEOUT 1000

extern SPI_HandleTypeDef hspi1;
static uint8_t return_buff[256];


typedef struct PIN{
    GPIO_TypeDef* port;
    uint16_t pin_mask;
}PIN;

PIN CS_PINS[NUM_MODULES] = {
    {GPIOA, GPIO_PIN_4}, /* APOLLO CS PIN */

    {GPIOA, GPIO_PIN_4}, /* TODO: HEPHAESTUS CS PIN */
    {GPIOA, GPIO_PIN_4}, /* TODO: HERA CS PIN */
    {GPIOA, GPIO_PIN_4}, /* TODO: JANUS CS PIN */
};

uint8_t* send_message(MODULE module, uint8_t msg_type, uint8_t* buff, uint8_t tx_size, uint8_t rx_size){

    uint8_t start_byte = START_BYTE;
    
    pull_cs_low(module);
    HAL_SPI_Transmit(&hspi1, &start_byte, 1, SPI_DEFAULT_TIMEOUT);     // Transmit Start Byte
    HAL_SPI_Transmit(&hspi1, &msg_type, 1, SPI_DEFAULT_TIMEOUT);       // Transmit Message Type

    uint8_t tx_count = tx_size;
    while (tx_count > 0){
        HAL_SPI_Transmit(&hspi1, buff, 1, SPI_DEFAULT_TIMEOUT);                 // Transmit Data
        tx_count--;
    }
    HAL_SPI_Receive(&hspi1, (uint8_t*)return_buff, rx_size, SPI_DEFAULT_TIMEOUT);
    release_cs(module);

    return return_buff;
}

void pull_cs_low(MODULE module){
    GPIO_TypeDef* CS_PORT = CS_PINS[module].port;
    uint16_t CS_PIN = CS_PINS[module].pin_mask;
    HAL_GPIO_WritePin(CS_PORT,
                      CS_PIN,
                      GPIO_PIN_RESET);
}

void release_cs(MODULE module){
    GPIO_TypeDef* CS_PORT = CS_PINS[module].port;
    uint16_t CS_PIN = CS_PINS[module].pin_mask;
    HAL_GPIO_WritePin(CS_PORT,
                      CS_PIN,
                      GPIO_PIN_SET);
}
