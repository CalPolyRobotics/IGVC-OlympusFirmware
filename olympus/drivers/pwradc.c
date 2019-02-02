/**
 * This module is designed to work with the MAX11629 ADC chip
 **/

#include <stdio.h>
#include <string.h>

#include "config.h"
#include "comms.h"
#include "pwradc.h"
#include "i2c.h"
#include "submoduleComms.h"
#include "timerCallback.h"

#define ADC_BUFF_SIZE (ADC_PERIPH_LAST_ENUM * sizeof(uint16_t))

//RESET: 0 0 0 0 RESET X X X
#define RESET_REG_SEL 0x10 //select write to reset register

#define RESET_ALL 0x00 //reset all registers and FIFO buffer
#define RESET_FIFO 0x08 //reset just FIFO

//SETUP: 0 1 CKSEL1 CKSEL0 REFSEL1 REFSEL0 X X
#define SETUP_REG_SEL 0x40 //select write to setup register

#define SETUP_CK_00 0x00 //setup clock mode 0
#define SETUP_CK_01 0x10 //setup clock mode 1
#define SETUP_CK_10 0x20 //setup clock mode 2

#define SETUP_REF_00 0x00 //setup reference mode 0
#define SETUP_REF_01 0x04 //setup reference mode 1
#define SETUP_REF_10 0x08 //setup reference mode 2
#define SETUP_REF_11 0x0C //setup reference mode 3

/* CONVERSION: 0 CHSEL3 CHSEL2 CHSEL1 CHSEL0 SCAN1 SCAN0 X */
#define CONVERSION_REG_SEL 0x80 //select write to conversion register

#define CONVERSION_SCAN_00 0x00 //one result per channel
#define CONVERSION_SCAN_01 0x02 //one result per channel
#define CONVERSION_SCAN_10 0x04 //multiple scans averaged to produce one result

#define CONVERSION_CH 3u

#define MAX_ADC_IDX 7

static enum adc_periph commsOrder[] = {batt_v, batt_i, twlv_v, twlv_i, fv_v, fv_i, thr_v, thr_i};

/* Value index is based off enum set in header */
static uint32_t muls[] = {92306, 5000, 3290, 13716, 92306, 12058, 92306, 92306};
static uint32_t divs[] = {220000, 3631, 2943, 3373, 220000, 3658, 220000, 220000};

uint8_t pwradcValues[ADC_BUFF_SIZE] = {0};

Timer_Return adc_poll_data();

extern olympusData_t olympusData;

#include <stdio.h>

void adc_init()
{
    /* pull CS pin low */
    HAL_GPIO_WritePin(PORT_SS_ZADC, PIN_SS_ZADC, GPIO_PIN_RESET);

    /* reset all registers to their default values */
    uint8_t data[2] = {
        RESET_REG_SEL | RESET_ALL,
        SETUP_REG_SEL | SETUP_CK_10 | SETUP_REF_10
    };

    SPI_Transmit(&hspi3, data, sizeof(data), 10);

    /* pull CS pin high */
    HAL_GPIO_WritePin(PORT_SS_ZADC, PIN_SS_ZADC, GPIO_PIN_SET);

    addCallbackTimer(10, adc_poll_data, NULL);
}


/* TODO
 * Go into comms.c and look for olympusData
   * This is used when transmitting the power across USB
   * It uses the u8 attribute of the powerunion_t typedef
 * Read data from ADC over SPI, and then using a changed adc convert function,
   get the 16 bit value from that data and store it in the olympusData union
   as an object and the specific type of data specified by the power_t struct
   * ex. One of the data values is battVolt (36V)
     * Read this value uint8_t[2]
     * Since our device is little endian, swap bytes and store:
       * ex. olympusData.obj.battVolt = (uint16_t)(lowByte << 8 | highByte)
     * Use mols and divs to adjust the voltage and current appropriately (recallibrate) */

void commsPwradcCallback(Packet_t* packet)
{
    int i;
    uint16_t convVal;
    for(i = 0; i < ADC_PERIPH_LAST_ENUM; i++)
    {
        convVal = adc_conv(commsOrder[i]);

        olympusData.power.u8[i*2] = convVal >> 8;
        olympusData.power.u8[(i*2)+1] = convVal & 0xFF;
    }
}

Timer_Return adc_poll_data() {
    /* Single-channel single-ended scan on selected periph
     * pull CS pin low */
    HAL_GPIO_WritePin(PORT_SS_ZADC, PIN_SS_ZADC, GPIO_PIN_RESET);

    uint8_t data = CONVERSION_REG_SEL | (MAX_ADC_IDX << CONVERSION_CH) | CONVERSION_SCAN_00;
    SPI_Transmit(&hspi3, &data, sizeof(uint8_t), 10);

    HAL_GPIO_WritePin(PORT_SS_ZADC, PIN_SS_ZADC, GPIO_PIN_SET);
    HAL_Delay(1);
    HAL_GPIO_WritePin(PORT_SS_ZADC, PIN_SS_ZADC, GPIO_PIN_RESET);

    SPI_Receive(&hspi3, pwradcValues, ADC_BUFF_SIZE, 10);
    
    /* pull CS pin high */
    HAL_GPIO_WritePin(PORT_SS_ZADC, PIN_SS_ZADC, GPIO_PIN_SET);

    return CONTINUE_TIMER;
}

uint16_t adc_conv(enum adc_periph periph){

    uint16_t adcVal = pwradcValues[periph * 2] << 8 |
                      pwradcValues[periph * 2 + 1];

    return (adcVal * muls[periph]) / divs[periph];
}
