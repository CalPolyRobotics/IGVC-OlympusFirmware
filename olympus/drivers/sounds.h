#ifndef __sound_H
#define __sound_H
#ifdef __cplusplus

extern "C"{
#endif

#include "stm32f2xx_hal.h"

extern TIM_HandleTypeDef htim1;

void Tim_Init(void);

#ifdef __cplusplus
}
#endif
#endif
