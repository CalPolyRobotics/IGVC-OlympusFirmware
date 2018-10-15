#include "stm32f0xx_hal.h"
#include "commsLib.h"
#include "systemClock.h"
#include "spi.h"

#define VECTOR_TABLE_LENGTH 48
#define APP_START_ADDR ((uint32_t*)0x8001400)
void remapVectorTable()
{
    volatile uint32_t *VectorTable = (uint32_t *)SRAM_BASE;
    uint32_t tblIdx = 0;

    /* Move vector table from start address to sram */
    for(tblIdx = 0; tblIdx < VECTOR_TABLE_LENGTH; tblIdx++)
    {
        VectorTable[tblIdx] = *(volatile uint32_t*)((uint32_t)APP_START_ADDR + (tblIdx * sizeof(uint32_t)));
    }

    __HAL_RCC_AHB_FORCE_RESET();

    __HAL_RCC_SYSCFG_CLK_ENABLE();

    __HAL_RCC_AHB_RELEASE_RESET();

    /* MAP 0x00000000 to SRAM */
    __HAL_SYSCFG_REMAPMEMORY_SRAM();
}

static wrError_t we;
uint8_t *bootload(uint8_t* data){
    /* Give time for response to be sent */
    SysTick_Delay(50u);

    NVIC_SystemReset();

    /** The code should not execute this return because of the Reset **/
    we = WR_ERR;
    return &we;
}
