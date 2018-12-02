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

#define ADC_REG_SEL 7 //start of register select

//RESET: 0 0 0 0 RESET X X X
#define RESET_REG_SEL 0x00 //select write to reset register

#define RESET_ALL 0x01 //reset all registers and FIFO buffer
#define RESET_FIFO 0x00 //reset just FIFO

#define RESET_RES 3 //RESET bit

//SETUP: 0 1 CKSEL1 CKSEL0 REFSEL1 REFSEL0 X X
#define SETUP_REG_SEL 0x01 //select write to setup register

#define SETUP_CK_00 0x00 //setup clock mode 0
#define SETUP_CK_01 0x01 //setup clock mode 1
#define SETUP_CK_10 0x02 //setup clock mode 2

#define SETUP_REF_00 0x00 //setup reference mode 0
#define SETUP_REF_01 0x01 //setup reference mode 1
#define SETUP_REF_10 0x02 //setup reference mode 2
#define SETUP_REF_11 0x03 //setup reference mode 3

#define SETUP_CK 5
#define SETUP_REF 3

//CONVERSION: 0 CHSEL3 CHSEL2 CHSEL1 CHSEL0 SCAN1 SCAN0 X
#define CONVERSION_REG_SEL 0x00 //select write to conversion register

// static enum adc_periph commsOrder[] = {batt_v, batt_i, twlv_v, twlv_i, fv_v, fv_i, thr_v, thr_i};

#define CONVERSION_SCAN_00 0x00 //one result per channel
#define CONVERSION_SCAN_01 0x01 //one result per channel
#define CONVERSION_SCAN_10 0x10 //multiple scans averaged to produce one result

#define CONVERSION_CH 6
#define CONVERSION_SCAN 2

/** TODO - Update to use SPI **/

/* Value index is based off enum set in header */
static uint32_t muls[] = {92306, 5000, 3290, 13716, 92306, 12058, 92306, 92306};
static uint32_t divs[] = {220000, 3631, 2943, 3373, 220000, 3658, 220000, 220000};

uint8_t pwradcValues[16] = {0};
uint8_t commsPwradcValues[16] = {0};
static volatile uint32_t currADCPeriph;

Timer_Return adc_poll_data();

void adc_init()
{
    //pull CS pin low
    HAL_GPIO_WritePin(PORT_SS_ZADC, PIN_SS_ZADC, GPIO_PIN_RESET);

    /* reset all registers to their default values */
    uint8_t data = (RESET_REG_SEL << ADC_REG_SEL) | (RESET_ALL << RESET_RES);
    SPI_Transmit(&hspi3, &data, sizeof(data), 10);

    /* configure ADC to use clock mode 2 and the internal reference with no wait time */
    data = (SETUP_REG_SEL << ADC_REG_SEL) | (SETUP_CK_10 << SETUP_CK) | (SETUP_REF_10 << SETUP_REF);
    SPI_Transmit(&hspi3, &data, sizeof(data), 10);

    //pull CS pin high
    HAL_GPIO_WritePin(PORT_SS_ZADC, PIN_SS_ZADC, GPIO_PIN_SET);

    addCallbackTimer(1000, adc_poll_data, NULL);
}

void commsPwradcCallback(Packet_t* packet)
{
/**
    int i;
    uint16_t convVal;
    for(i = 0; i < ADC_PERIPH_LAST_ENUM; i++)
    {
        convVal = adc_conv(commsOrder[i]);

        //Convert uint16 to uint8s
        commsPwradcValues[i*2] = convVal >> 8;
        commsPwradcValues[(i*2)+1] = convVal & 0xFF;
    }
**/
}

/** TODO
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
     * Use mols and divs to adjust the voltage and current appropriately (recallibrate)
**/

Timer_Return adc_poll_data() {
    /* Single-channel single-ended scan on selected periph */
    //pull CS pin low
    HAL_GPIO_WritePin(PORT_SS_ZADC, PIN_SS_ZADC, GPIO_PIN_RESET);

    uint8_t data = (CONVERSION_REG_SEL << ADC_REG_SEL) | (currADCPeriph << CONVERSION_CH) | (CONVERSION_SCAN_00 << CONVERSION_SCAN);
    SPI_Transmit(&hspi3, &data, sizeof(data), 10);

    //pull CS pin high
    HAL_GPIO_WritePin(PORT_SS_ZADC, PIN_SS_ZADC, GPIO_PIN_SET);

    uint8_t adcData[sizeof(uint16_t)];
    SPI_Receive(&hspi3, (uint8_t*)adcData, sizeof(adcData), 10);

    memcpy(&pwradcValues[currADCPeriph], adcData, sizeof(adcData));

    return CONTINUE_TIMER;
}

uint16_t adc_conv(enum adc_periph periph){

    uint16_t adcVal = pwradcValues[periph * 2] << 8 |
                      pwradcValues[periph * 2 + 1];

    return (adcVal * muls[periph]) / divs[periph];
}
