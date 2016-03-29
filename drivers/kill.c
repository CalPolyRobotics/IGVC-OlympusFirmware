
#include "config.h"
#include "kill.h"

void killBoard()
{
    HAL_GPIO_WritePin(GPIO_BOARD_KILL_PORT,
                      GPIO_BOARD_KILL_PIN,
                      GPIO_PIN_RESET);
    while(1);
}