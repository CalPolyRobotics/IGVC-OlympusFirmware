#ifndef ADC_H
#define ADC_H

#include <stdint.h>

#define ADC_LINES 8

enum adc_periph {
    fv_i = 0, /* Ain0 */
    thr_v,    /* Ain1 */
    twlv_i,   /* Ain2 */
    twlv_v,   /* Ain3 */
    batt_v,   /* Ain4 */
    fv_v,     /* Ain5 */
    thr_i,    /* Ain6 */
    batt_i,   /* Ain7 */
    ADC_PERIPH_LAST_ENUM
};

void adc_init();
uint16_t adc_conv(enum adc_periph periph);
void commsPwradcCallback();
#endif
