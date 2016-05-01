#include "config.h"
#include "comms.h"
#include "pwradc.h"
#include "i2c.h"

#define ADC_I2C_ADDR (0x33 << 1)

#define ADC_REG 7
#define ADC_SEL 4
#define ADC_RST 1

#define ADC_SCN 5
#define ADC_CS  1
#define ADC_SGL 0


/* Value index is based off enum set in header */
static uint32_t mul[] = {92306, 5000, 3290, 13716, 92306, 12058, 92306, 92306};
static uint32_t div[] = {220000, 3631, 2943, 3373, 220000, 3658, 220000, 220000};

const char* periph_name[] =  
{
    "Battery (I)", 
    "Five (V)",
    "Three (V)", 
    "Battery (V)",
    "Twelve (I)",
    "Twelve (V)",
    "Five (I)",
    "Three (I)"
};

const char* periph_unit[] = {"mA", "mV", "mV", "mV", "mA", "mV", "mA", "mA"};

/* Order to traverse periphs for viewing pleasure */
const uint8_t periph_order[] = {3, 0, 5, 4, 1, 6, 2, 7};

uint8_t commsPwradcValues[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

static const uint8_t adc_cfg = 
    (1 << ADC_REG) | (0x7 << ADC_SEL) | (1 << ADC_RST); 

void adc_init() {
    /* Set ADC to internal reference with output */
    i2cTransmit(ZEUS_ADC_I2C_ADDR, (uint8_t *)&adc_cfg, sizeof(uint8_t));
}

void commsPwradcCallback(Packet_t* packet){
    uint16_t adc; 
    int i;

    // Convert uint16 to two uint8s
    for(i = 0; i < ADC_LINES * 2; i+=2){
        adc = adc_conv(periph_order[i/2]);
        commsPwradcValues[i] = adc >> 8;
        commsPwradcValues[i+1] = adc & 0xFF;
    }
    
}

uint16_t adc_read(enum adc_periph periph) {
    /* Single-channel single-ended scan on selected periph */
    uint8_t adc_cmd = (3 << ADC_SCN) | (periph << ADC_CS) | (1 << ADC_SGL);
    uint16_t res;

    i2cTransmit(ZEUS_ADC_I2C_ADDR, (uint8_t *)&adc_cmd, sizeof(uint8_t));
    i2cReceive(ZEUS_ADC_I2C_ADDR, (uint8_t *)&res, sizeof(uint16_t));

    /* Only the bottom 12 bits of the adc response are valid */
    return ((res << 8) | (res >> 8)) & 0xFFF;
}

uint16_t adc_conv(enum adc_periph periph){
    return (adc_read(periph) * mul[periph]) / div[periph];
}
