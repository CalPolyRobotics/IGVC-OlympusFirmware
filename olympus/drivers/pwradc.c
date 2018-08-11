#include "config.h"
#include "comms.h"
#include "pwradc.h"
#include "i2c.h"
#include "timerCallback.h"

#define ADC_I2C_ADDR (0x33 << 1)

#define ADC_REG 7
#define ADC_SEL 4
#define ADC_RST 1

#define ADC_SCN 5
#define ADC_CS  1
#define ADC_SGL 0

/** TODO - Update to use SPI **/

/* Value index is based off enum set in header */
static uint32_t muls[] = {92306, 5000, 3290, 13716, 92306, 12058, 92306, 92306};
static uint32_t divs[] = {220000, 3631, 2943, 3373, 220000, 3658, 220000, 220000};

static enum adc_periph commsOrder[] = {batt_v, batt_i, twlv_v, twlv_i, fv_v, fv_i, thr_v, thr_i};

uint8_t pwradcValues[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
uint8_t commsPwradcValues[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
static volatile uint32_t currADCPeriph;

static const uint8_t adc_cfg = 
    (1 << ADC_REG) | (0x7 << ADC_SEL) | (1 << ADC_RST); 


Timer_Return adc_poll_data();

void adc_init()
{
    /* Set ADC to internal reference with output */
    /**
    i2cAddTxTransaction(ZEUS_ADC_I2C_ADDR,
                        (uint8_t *)&adc_cfg,
                        sizeof(uint8_t),
                        NULL,
                        NULL);

    addCallbackTimer(30, adc_poll_data, NULL);
    **/
}

void commsPwradcCallback(Packet_t* packet)
{
    int i;
    uint16_t convVal;
    for(i = 0; i < ADC_PERIPH_LAST_ENUM; i++)
    {
        convVal = adc_conv(commsOrder[i]);

        //Convert uint16 to uint8s
        commsPwradcValues[i*2] = convVal >> 8;
        commsPwradcValues[(i*2)+1] = convVal & 0xFF;
    }
   
}

void adc_readDataCallback(void* dummy, uint8_t* data, uint32_t len, I2CStatus status)
{
    /**
    if (status == I2C_ACK)
    {
        // Convert uint16 to two uint8s
        pwradcValues[currADCPeriph * 2] = data[0] & 0xF;
        pwradcValues[(currADCPeriph * 2) + 1] = data[1];

        // Go to the next ADC Periph
        currADCPeriph = (currADCPeriph + 1) % ADC_PERIPH_LAST_ENUM;
    } else {
        printf("ADC Rx NACK\r\n");
    }
    **/
}

void adc_readSetupCallback(void* dummy, I2CStatus status)
{
    /**
    if (status == I2C_ACK)
    {
        
        i2cAddRxTransaction(ZEUS_ADC_I2C_ADDR,
                            sizeof(uint16_t),
                            adc_readDataCallback,
                            NULL);

    } else {
        printf("ADC Tx NACK\r\n");
    }
    **/
}

Timer_Return adc_poll_data() {
    /* Single-channel single-ended scan on selected periph */
    /**
    uint8_t adc_cmd = (3 << ADC_SCN) | (currADCPeriph << ADC_CS) | (1 << ADC_SGL);

    i2cAddTxTransaction(ZEUS_ADC_I2C_ADDR,
                        (uint8_t *)&adc_cmd,
                        sizeof(uint8_t),
                        adc_readSetupCallback,
                        NULL);

    **/
    return CONTINUE_TIMER;
}

uint16_t adc_conv(enum adc_periph periph){

    uint16_t adcVal = pwradcValues[periph * 2] << 8 |
                      pwradcValues[periph * 2 + 1];

    return (adcVal * muls[periph]) / divs[periph];
}