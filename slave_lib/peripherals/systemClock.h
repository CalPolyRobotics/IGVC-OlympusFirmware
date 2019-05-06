#ifndef SYSTEMCLOCK_H_
#define SYSTEMCLOCK_H_

#include <stdint.h>

extern uint32_t systickCount;

void SystemClock_Config(void);
void SysTick_Delay(int ms);

#endif
