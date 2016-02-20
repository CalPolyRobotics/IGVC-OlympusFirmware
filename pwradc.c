#include "pwradc.h"
#include "i2c.h"

#define ADC_I2C_ADDR (0x33 << 1)

#define ADC_REG 7
#define ADC_SEL 4
#define ADC_RST 1

#define ADC_SCN 5
#define ADC_CS  1
#define ADC_SGL 0

static const uint8_t adc_cfg = 
    (1 << ADC_REG) | (0x7 << ADC_SEL) | (1 << ADC_RST);

void adc_init() {
    /* Set ADC to internal reference with output */
    i2cTransmit(ADC_I2C_ADDR, (uint8_t *)&adc_cfg, sizeof(uint8_t));
}

uint16_t adc_read(enum adc_periph periph) {
    /* Single-channel single-ended scan on selected periph */
    uint8_t adc_cmd = (3 << ADC_SCN) | (periph << ADC_CS) | (1 << ADC_SGL);
    uint16_t res;

    i2cTransmit(ADC_I2C_ADDR, (uint8_t *)&adc_cmd, sizeof(uint8_t));
    i2cReceive(ADC_I2C_ADDR, (uint8_t *)&res, sizeof(uint16_t));

    /* Only the bottom 12 bits of the adc response are valid */
    return res & 0x0FFF;
}
