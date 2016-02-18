
#include "kill.h"

void killBoard()
{
    HAL_GPIO_WritePin(BOARD_KILL_PORT, BOARD_KILL_PIN, GPIO_PIN_RESET);
    while(1);
}