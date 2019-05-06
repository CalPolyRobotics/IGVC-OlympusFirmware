#ifndef __sound_H
#define __sound_H

#include "stm32f2xx_hal.h"

extern TIM_HandleTypeDef htim1;

typedef struct{
   int freq;
   int numnotes;
} note;

#define BUZZER_PIN GPIO_PIN_8

void Tim_Init(void);
void playNote(uint16_t fre);
void runStartupSong(void);
void runErrorSong(void);

#endif
