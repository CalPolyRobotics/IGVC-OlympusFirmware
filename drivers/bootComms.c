#include "stm32f2xx_hal.h"

void bootloadBoard(){
    // Clear Reset Flags for Detection on Startup
    RCC -> CSR |= RCC_CSR_RMVF;

    NVIC_SystemReset();
}
