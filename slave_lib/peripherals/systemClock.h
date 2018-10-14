#ifndef SYSTEMCLOCK_H_
#define SYSTEMCLOCK_H_

#include <stdint.h>
extern volatile uint32_t systickCount;

void SystemClock_Config(void);
void SysTick_Delay(uint32_t ms);

#endif
