#include "stm32f0xx.h"
#include "tinySpi.h"
#include "systemClock.h"

int main(void)
{
    wrError_t status;

    /* Configure the system clock to 48 MHz */
    SystemClock_Config();

    MX_SPI1_Init();

    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;

    /** Set PIN3 as output push pull **/
    GPIOB->MODER |= GPIO_MODER_MODER3_0;
    GPIOB->ODR |= GPIO_ODR_3;

    uint8_t data;
    while (1)
    {
        status = readResponse(&data, 1);
        if(status == WR_OK){
            runCommsFSM(data);
        }
    }
}
