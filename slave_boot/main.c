#include "stm32f0xx.h"
#include "flash.h"
#include "systemClock.h"
#include "spi.h"

int main(void)
{
    wrError_t status;

    /* Configure the system clock to 48 MHz and initialize systick timer*/
    SystemClock_Config();

    /* Enable interrupts */
    __enable_irq();

    /*
     * Start Bootloader after Software Reset
     */
    if((RCC -> CSR & RCC_CSR_SFTRSTF) == 0)
    {
        jumpToApp(APP_START_ADDR);
    }

    /* Clear Reset Flags for Detection on Startup */
    RCC -> CSR |= RCC_CSR_RMVF;

    MX_COMMS_SPI_Init();

    uint8_t data;
    while (1)
    {
        status = readByte(&data);
        if(status == WR_OK){
            runCommsFSM(data);
        }
    }
}
