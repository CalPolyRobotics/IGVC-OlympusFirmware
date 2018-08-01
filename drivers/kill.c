#include "config.h"
#include "kill.h"

#define RESET_KEY ((uint32_t)0x464C4150)

void resetBoard(uint8_t* data)
{
    if(*(uint32_t*)(data) == RESET_KEY){
        // Clear Reset Flags for Detection on Startup
        RCC -> CSR |= RCC_CSR_RMVF;

        NVIC_SystemReset();
    }
}

void killBoard()
{
    HAL_GPIO_WritePin(GPIO_BOARD_KILL_PORT,
                      GPIO_BOARD_KILL_PIN,
                      GPIO_PIN_SET);
    while(1);
}
