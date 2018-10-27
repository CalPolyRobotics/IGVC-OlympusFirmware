#include <speed.h>
volatile int count = 0;

void TIM2_IRQHandler() {
     count++; 
}
