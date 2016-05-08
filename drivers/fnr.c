
#include "config.h"
#include "gpio.h"
#include "fnr.h"
#include "speedDAC.h"

#include <stdio.h>

void initAutomanInt()
{
    EXTI->IMR |= EXTI_IMR_MR15;
    EXTI->RTSR |= EXTI_RTSR_TR15;
    EXTI->FTSR |= EXTI_FTSR_TR15;

    SYSCFG->EXTICR[3] = SYSCFG_EXTICR4_EXTI15_PC;

    NVIC_SetPriority(EXTI15_10_IRQn, 5);
    NVIC_EnableIRQ(EXTI15_10_IRQn);
}

void setFNR(FNR_t newState)
{
    switch (newState)
    {
        case FNR_FORWARD:
            setSpeedDACOutputEnable(1);
            HAL_GPIO_WritePin(GPIO_FNR_DIRECTION_PORT,
                              GPIO_FNR_DIRECTION_PIN,
                              GPIO_PIN_SET);
            HAL_GPIO_WritePin(GPIO_FNR_ENABLE_PORT,
                              GPIO_FNR_ENABLE_PIN,
                              GPIO_PIN_SET);
            break;

        case FNR_NEUTRAL:
            setSpeedDACOutputEnable(0);
            HAL_GPIO_WritePin(GPIO_FNR_ENABLE_PORT,
                              GPIO_FNR_ENABLE_PIN,
                              GPIO_PIN_RESET);
            break;

        case FNR_REVERSE:
            setSpeedDACOutputEnable(1);
            HAL_GPIO_WritePin(GPIO_FNR_DIRECTION_PORT,
                              GPIO_FNR_DIRECTION_PIN,
                              GPIO_PIN_RESET);
            HAL_GPIO_WritePin(GPIO_FNR_ENABLE_PORT,
                              GPIO_FNR_ENABLE_PIN,
                              GPIO_PIN_SET);
            break;
    }
}

void FNRCommsHandler(Packet_t* packet)
{
    if (packet->data[0] >= FNR_NEUTRAL && 
        packet->data[0] <= FNR_REVERSE)
    {
        setFNR(packet->data[0]);
    }
}

void EXTI15_10_IRQHandler()
{
    EXTI->PR |= EXTI_PR_PR15;
    printf("Pin Changed\r\n");
}
