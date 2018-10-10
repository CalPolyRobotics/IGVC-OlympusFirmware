#ifndef ADC_H
#define ADC_H

#include <stdint.h>

#define STEER_AVG_IDX    ((uint32_t)0u)

extern uint16_t adcChnAvg[];

void MX_ADC1_Init();

#endif
