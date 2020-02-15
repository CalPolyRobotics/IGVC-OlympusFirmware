#include <string.h>

#include "stm32f0xx_hal.h"
#include "adc.h"

static volatile uint16_t adcBuffer[ADC_BUFF_SIZE];

uint16_t adcChnAvg[2u][NUM_ADC_CHANNELS];
int adcChnWriteIdx = 0u;
int adcChnReadIdx = 1u;

static void ADC1_LL_Init();

void MX_ADC1_Init()
{
    ADC1_LL_Init();

    /** ADC Calibration */
    if((ADC1->CR & ADC_CR_ADEN) != 0u)
    {
        ADC1->CR |= ADC_CR_ADDIS;
    }

    while((ADC1->CR & ADC_CR_ADEN) != 0u);

    ADC1->CFGR1 &= ~ADC_CFGR1_DMAEN;
    ADC1->CR |= ADC_CR_ADCAL;

    while((ADC1->CR & ADC_CR_ADCAL) != 0u);

    /** ADC Setup
     * Continuous Mode, Overwrite Data on Overrun, Enable Circular DMA
     * Channel 2 & Channel 3
     **/
    ADC1->CFGR1 |= ADC_CFGR1_CONT | ADC_CFGR1_OVRMOD | ADC_CFGR1_DMAEN |
                   ADC_CFGR1_DMACFG;
    ADC1->CHSELR = ADC_CHSELR_CHSEL2 | ADC_CHSELR_CHSEL3;

    // Make sure SYSCFG is set to use DMA channel 1 for ADC
    SYSCFG->CFGR1 &= ~SYSCFG_CFGR1_DMA_RMP;

    /** DMA Configuration **/
    DMA1_Channel1->CPAR = (uint32_t)(&(ADC1->DR));
    DMA1_Channel1->CMAR = (uint32_t)adcBuffer;
    DMA1_Channel1->CNDTR = ADC_BUFF_SIZE;

    /**
     * Memory Increment Mode, 16-bit Mem Size, 16-bit Peripheral Size,
     * Transfer Error and Transfer Complete Interrupt Enable,
     * Circular Mode
     */
    DMA1_Channel1->CCR |= DMA_CCR_MINC | DMA_CCR_MSIZE_0 | DMA_CCR_PSIZE_0 |
                          DMA_CCR_TEIE | DMA_CCR_TCIE | DMA_CCR_CIRC; 
    DMA1_Channel1->CCR |= DMA_CCR_EN;

    NVIC_EnableIRQ(DMA1_Channel1_IRQn);

    /** ADC Enable **/
    if((ADC1 -> ISR & ADC_ISR_ADRDY) != 0u)
    {
        ADC1->ISR |= ADC_ISR_ADRDY;
    }

    ADC1->CR |= ADC_CR_ADEN;
    while((ADC1->ISR & ADC_ISR_ADRDY) == 0u);

    ADC1->CR |= ADC_CR_ADSTART;
}

static void ADC1_LL_Init()
{
    // PA2 -> BRK_POT -> ADC_IN2
    // PA3 -> STR_POT -> ADC_IN3

    /** Enable Clock **/
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
    RCC->CR2 |= RCC_CR2_HSI14ON;
    while((RCC->CR2 & RCC_CR2_HSI14RDY) == 0u);

    RCC->AHBENR |= RCC_AHBENR_DMA1EN;
    
    /** Remap ADC DMA to Channel 1 **/
    SYSCFG->CFGR1 &= ~SYSCFG_CFGR1_ADC_DMA_RMP;

    /** Enable GPIO **/
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Pin = GPIO_PIN_2 | GPIO_PIN_3;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}


void DMA1_Channel1_IRQHandler()
{
    /** Transfer Complete **/
    if(DMA1->ISR & DMA_ISR_TCIF1)
    {
        // Pause Transfers
        DMA1_Channel1->CCR &= ~DMA_CCR_EN;
        DMA1->IFCR |= DMA_IFCR_CTCIF1;
        //Put ADC in idle state
        ADC1->CR |= ADC_CR_ADSTP;

        // Clear Data & Calculate Average
        memset(adcChnAvg[adcChnWriteIdx], 0u, NUM_ADC_CHANNELS * sizeof(adcChnAvg[0][0]));

        int idx;
        for(idx = 0; idx < ADC_BUFF_SIZE; idx++)
        {
            adcChnAvg[adcChnWriteIdx][idx % NUM_ADC_CHANNELS] += adcBuffer[idx];
        }
        
        int chn;
        for(chn = 0; chn < NUM_ADC_CHANNELS; chn++)
        {
            adcChnAvg[adcChnWriteIdx][chn] /= ADC_AVG_NUM;
        }

        adcChnWriteIdx = (adcChnWriteIdx + 1u) % PING_PONG_SIZE;
        adcChnReadIdx = (adcChnReadIdx + 1u) % PING_PONG_SIZE;

        // Contiue Transfers
        DMA1_Channel1->CCR |= DMA_CCR_EN;
        // Put ADC out of idle state
        ADC1->CR |= ADC_CR_ADSTART;
    }

    /** Transfer Error **/
    if(DMA1->ISR & DMA_ISR_TEIF1)
    {
        DMA1->IFCR |= DMA_IFCR_CTEIF1;
    }
}
