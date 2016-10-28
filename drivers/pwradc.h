#ifndef ADC_H
#define ADC_H

#include <stdint.h>

#define ADC_LINES 8

enum adc_periph {
    batt_i = 0, /* Ain0 */
    fv_v,       /* Ain1 */
    thr_v,      /* Ain2 */
    batt_v,     /* Ain3 */
    twlv_i,     /* Ain4 */
    twlv_v,     /* Ain5 */
    fv_i,       /* Ain6 */
    thr_i,      /* Ain7 */
    ADC_PERIPH_LAST_ENUM
};

extern const char* periph_name[];
extern const char* periph_unit[];
extern const uint8_t periph_order[]; 

void adc_init();
void commsPwradcCallback();
uint16_t adc_read(enum adc_periph periph);
uint16_t adc_conv(enum adc_periph periph);
#endif
