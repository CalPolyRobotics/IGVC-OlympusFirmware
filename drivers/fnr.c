
#include "config.h"
#include "gpio.h"
#include "fnr.h"
#include "speedDAC.h"
#include "i2c.h"
#include "timerCallback.h"
#include "sevenSeg.h"
#include "steering.h"


#include <stdio.h>

volatile uint8_t FNRState = 0;

static Timer_Return updateSevenSegCallback(void* dummy);
static Timer_Return pollFnrStateCallback(void* dummy);

void initAutomanInt()
{
    //EXTI->IMR |= EXTI_IMR_MR15;
    //EXTI->RTSR |= EXTI_RTSR_TR15;
    //EXTI->FTSR |= EXTI_FTSR_TR15;

    //SYSCFG->EXTICR[3] = SYSCFG_EXTICR4_EXTI15_PC;

    //NVIC_SetPriority(EXTI15_10_IRQn, 5);
    //NVIC_EnableIRQ(EXTI15_10_IRQn);

    addCallbackTimer(POLL_FNR_UPDATE_PERIOD, pollFnrStateCallback, NULL);
}

void serviceAutoman()
{
    if (HAL_GPIO_ReadPin(GPIO_COMPUTER_DRIVE_PORT, GPIO_COMPUTER_DRIVE_PIN))
    {
        //printf("Enabled DAC\r\n");
        enableSpeedDAC();
        // Enabled Steering
        enableSteering(1);
    }
    else
    {
        //printf("Disabled DAC\r\n");
        disableSpeedDAC();
        // Enabled Steering
        enableSteering(0);
    }
}

// Function to set FNR
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
        case FNR_STATE_UNKNOWN:
        default:
            setSpeedDACOutputEnable(0);
            HAL_GPIO_WritePin(GPIO_FNR_ENABLE_PORT,
                              GPIO_FNR_ENABLE_PIN,
                              GPIO_PIN_RESET);
    }
}

uint8_t getFNR(){
    return FNRState;
}

void pollFnrI2CCallback(void* dummy, uint8_t* newState, uint32_t len, I2CStatus status)
{
    if (status == I2C_ACK)
    {
        //if (*newState <= (FNR_REVERSE + 1))
        {
            FNRState = *newState;
            setSevenSeg(FNRState);
        //} else {
            //setSevenSeg('r');
        }
    } else {
        FNRState = FNR_STATE_UNKNOWN;
        setSevenSeg('n');
    }
}

Timer_Return pollFnrStateCallback(void* dummy)
{
    i2cAddRxTransaction(JANUS_ATTINY_I2C_ADDR, 1, pollFnrI2CCallback, NULL);
    return CONTINUE_TIMER;
}

void commsGetFNRCallback(){
    FNRState = getFNR();
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
}
