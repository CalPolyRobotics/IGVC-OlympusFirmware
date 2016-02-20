#ifndef ADC_H
#define ADC_H

enum dac_periph {
    batt_i = 0, /* Ain0 */
    fv_v,       /* Ain1 */
    thr_v,      /* Ain2 */
    batt_v,     /* Ain3 */
    twlv_i,     /* Ain4 */
    twlv_v,     /* Ain5 */
    fv_i,       /* Ain6 */
    thr_i       /* Ain7 */
};

void dac_init();
uint16_t dac_read(dac_periph periph);
#endif
