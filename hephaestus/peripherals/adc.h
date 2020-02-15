#ifndef ADC_H
#define ADC_H

#include <stdint.h>

#define PING_PONG_SIZE 2u

#define NUM_ADC_CHANNELS ((uint32_t)2u)
#define ADC_AVG_NUM      ((size_t)8u)
#define ADC_BUFF_SIZE    (ADC_AVG_NUM * NUM_ADC_CHANNELS)

#define BRAKE_AVG_IDX    ((uint32_t)1u) // TODO: Swapped indexes (was originally 0)
#define STEER_AVG_IDX    ((uint32_t)0u)

extern uint16_t adcChnAvg[PING_PONG_SIZE][NUM_ADC_CHANNELS];
extern int adcChnReadIdx;

void MX_ADC1_Init();

#endif
