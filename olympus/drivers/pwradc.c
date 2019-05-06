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

extern olympusData_t olympusData;

static enum adc_periph commsOrder[] = {batt_v, batt_i, twlv_v, twlv_i, fv_v, fv_i, thr_v, thr_i};

/* Value index is based off enum set in header */
static uint32_t muls[] = {3821,3270,3024,5956,9200,4953,551,1401};
static uint32_t divs[] = {7800,2393,6025,1447,669,2915,1040,4120};

static uint8_t zeros[ADC_BUFF_SIZE * 2] = {0};

static void adc_start_conversion();
static Timer_Return adc_poll_data();

uint8_t pwradcValues[ADC_BUFF_SIZE * 2] = {0};

void adc_init()
{
    /* pull CS pin low */
    HAL_GPIO_WritePin(PORT_SS_ZADC, PIN_SS_ZADC, GPIO_PIN_RESET);

    /* reset all registers to their default values */
    uint8_t data[1] = {
        //RESET_REG_SEL | RESET_ALL,
        SETUP_REG_SEL | SETUP_CK_10 | SETUP_REF_10
    };

    HAL_SPI_Transmit(&hspi3, data, sizeof(data), 10);

    /* pull CS pin high */
    HAL_GPIO_WritePin(PORT_SS_ZADC, PIN_SS_ZADC, GPIO_PIN_SET);

    adc_start_conversion();
    addCallbackTimer(1000, adc_poll_data, NULL);
}

void commsPwradcCallback(Packet_t* packet)
{
    int i;
    uint16_t convVal;
    for(i = 0; i < ADC_PERIPH_LAST_ENUM; i++)
    {
        convVal = adc_conv(commsOrder[i]);

        olympusData.power.u8[(i*2) + 1] = convVal >> 8;
        olympusData.power.u8[i*2] = convVal & 0xFF;

    }
}


static void adc_start_conversion(){
    /* Single-channel single-ended scan on selected periph */
    HAL_GPIO_WritePin(PORT_SS_ZADC, PIN_SS_ZADC, GPIO_PIN_RESET);

    uint8_t data = CONVERSION_REG_SEL | (7u << CONVERSION_CH);
    HAL_SPI_Transmit(&hspi3, &data, sizeof(uint8_t), 10);

    HAL_GPIO_WritePin(PORT_SS_ZADC, PIN_SS_ZADC, GPIO_PIN_SET);
}


static Timer_Return adc_poll_data() {
    HAL_GPIO_WritePin(PORT_SS_ZADC, PIN_SS_ZADC, GPIO_PIN_RESET);

    HAL_SPI_TransmitReceive(&hspi3, zeros, pwradcValues, ADC_BUFF_SIZE * 2, 10);
    
    /* pull CS pin high */
    HAL_GPIO_WritePin(PORT_SS_ZADC, PIN_SS_ZADC, GPIO_PIN_SET);

    /* start conversion for next read */
    adc_start_conversion();

    return CONTINUE_TIMER;
}

uint16_t adc_conv(enum adc_periph periph){

    uint16_t adcVal = pwradcValues[periph * 2] << 8 |
                      pwradcValues[periph * 2 + 1];

    return (adcVal * muls[periph]) / divs[periph];
}
