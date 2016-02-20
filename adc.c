#include "adc.h"
#include "i2c.h"

#define ADC_I2C_ADDR (0x33 << 1)

static const uint8_t adc_cfg = 
    (1 << ADC_REG) | (0x7 << ADC_SEL) | (1 << ADC_RST);

void dac_init() {
    /* Set ADC to internal reference with output */
    i2cTransmit(ADC_I2C_ADDR, &adc_cfg, sizeof(uint8_t));
}

uint16_t dac_read(dac_periph periph) {
    /* Single-channel single-ended scan on selected periph */
    uint8_t adc_cmd = (3 << ADC_SCN) | (periph << ADC_CS) | (1 << ADC_SIN_SEL);
    uint16_t res;

    i2cTransmit(ADC_I2C_ADDR, &adc_cmd, sizeof(uint8_t));
    i2cReceive(ADC_I2C_ADDR, &res, sizeof(uint16_t));

    /* Only the bottom 12 bits of the adc response are valid */
    return res & 0x0FFF;
}
