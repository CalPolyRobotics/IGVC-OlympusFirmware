#ifndef ADC_H
#define ADC_H

#include <stdint.h>

enum adc_periph {
    batt_i = 0, /* Ain0 */
    fv_v,       /* Ain1 */
    thr_v,      /* Ain2 */
    batt_v,     /* Ain3 */
    twlv_i,     /* Ain4 */
    twlv_v,     /* Ain5 */
    fv_i,       /* Ain6 */
    thr_i       /* Ain7 */
};

void adc_init();
uint16_t adc_read(enum adc_periph periph);
#endif
