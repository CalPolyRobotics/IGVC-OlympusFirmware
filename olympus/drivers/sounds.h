#ifndef __sound_H
#define __sound_H
#ifdef __cplusplus

extern "C"{
#endif

#include "stm32f2xx_hal.h"

extern TIM_HandleTypeDef htim1;

typedef struct{
   int freq;
   int numnotes;
} note;

#define BUZZER_PIN GPIO_PIN_8

void Tim_Init(void);
void playNote(uint16_t fre);
void Song(void);
void Error(void);

#ifdef __cplusplus
}
#endif
#endif
