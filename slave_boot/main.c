#include "stm32f0xx.h"
#include "flash.h"
#include "systemClock.h"
#include "tinySpi.h"

int main(void)
{
    wrError_t status;

    /* Configure the system clock to 48 MHz */
    SystemClock_Config();

    /*
     * Start Bootloader after Software Reset
     */
    if((RCC -> CSR & RCC_CSR_SFTRSTF) == 0)
    {
        jumpToApp(APP_START_ADDR);
    }

    /* Clear Reset Flags for Detection on Startup */
    RCC -> CSR |= RCC_CSR_RMVF;

    MX_SPI1_Init();

    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;

    /** Set PIN3 as output push pull **/
    GPIOB->MODER |= GPIO_MODER_MODER3_0;
    GPIOB->ODR |= GPIO_ODR_3;

    uint8_t data;
    while (1)
    {
        status = readByte(&data);
        if(status == WR_OK){
            runCommsFSM(data);
        }
    }
}
