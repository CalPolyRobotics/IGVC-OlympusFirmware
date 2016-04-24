#include "adc.h"

#include "gpio.h"

static uint16_t ADCData[2*8];

ADC_HandleTypeDef hadc1;
ADC_HandleTypeDef hadc2;

/* ADC1 init function */
void MX_ADC1_Init(void)
{
    ADC_ChannelConfTypeDef sConfig;

    /**Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion) 
    */
    hadc1.Instance = ADC1;
    hadc1.Init.ClockPrescaler = ADC_CLOCKPRESCALER_PCLK_DIV2;
    hadc1.Init.Resolution = ADC_RESOLUTION12b;
    hadc1.Init.ScanConvMode = DISABLE;
    hadc1.Init.ContinuousConvMode = ENABLE;
    hadc1.Init.DiscontinuousConvMode = DISABLE;
    hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc1.Init.NbrOfConversion = 1;
    hadc1.Init.DMAContinuousRequests = DISABLE;
    hadc1.Init.EOCSelection = EOC_SINGLE_CONV;
    HAL_ADC_Init(&hadc1);

    /**Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time. 
    */
    sConfig.Channel = ADC_CHANNEL_14;
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
    HAL_ADC_ConfigChannel(&hadc1, &sConfig);

}
/* ADC2 init function */
void MX_ADC2_Init(void)
{
    ADC2->CR1 = ADC_CR1_SCAN;
    ADC2->CR2 = ADC_CR2_DDS | ADC_CR2_DMA | ADC_CR2_CONT | ADC_CR2_ADON;
    ADC2->SMPR2 = (ADC_SMPR2_SMP0_2) | (ADC_SMPR2_SMP1_2);

    ADC2->SQR1 = (ADC_SQR1_L_0);
    ADC2->SQR3 = (10) | (11 << 5);

    ADC->CCR = ADC_CCR_DMA_0 | ADC_CCR_DDS;

    GPIO_InitTypeDef GPIO_InitStruct;

    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    //Stream 3, Channel 1

    DMA2_Stream3->CR = (DMA_SxCR_CHSEL_0) |\
                 (DMA_SxCR_PL) | \
                 (DMA_SxCR_MSIZE_0) |\
                 (DMA_SxCR_PSIZE_0) |\
                 (DMA_SxCR_MINC) |\
                 (DMA_SxCR_CIRC);

    DMA2_Stream3->NDTR = 2*8;
    DMA2_Stream3->PAR = (uint32_t)&(ADC2->DR);
    DMA2_Stream3->M0AR = (uint32_t)ADCData;

    DMA2_Stream3->CR |= (DMA_SxCR_EN);

    ADC2->CR2 |= ADC_CR2_SWSTART;
}

void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    if(hadc->Instance==ADC1)
    {
        /* Peripheral clock enable */
        __ADC1_CLK_ENABLE();

        /**ADC1 GPIO Configuration    
        PC1     ------> ADC1_IN11
        PC2     ------> ADC1_IN12
        PC3     ------> ADC1_IN13
        PA0-WKUP     ------> ADC1_IN0
        PA1     ------> ADC1_IN1
        PC4     ------> ADC1_IN14
        PC5     ------> ADC1_IN15 
        */
        GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4 
                              |GPIO_PIN_5;
        GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
        GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        /* Peripheral interrupt init*/
        NVIC_SetPriority(ADC_IRQn, 4);
        NVIC_EnableIRQ(ADC_IRQn);
    } else if(hadc->Instance==ADC2) {
        /* Peripheral clock enable */
        __ADC2_CLK_ENABLE();

        /**ADC2 GPIO Configuration    
        PC0     ------> ADC2_IN10 
        */
        GPIO_InitStruct.Pin = GPIO_PIN_0;
        GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

        /* Peripheral interrupt init*/
        HAL_NVIC_SetPriority(ADC_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(ADC_IRQn);
    }
}

void HAL_ADC_MspDeInit(ADC_HandleTypeDef* hadc)
{
    if(hadc->Instance==ADC1)
    {
        /* Peripheral clock disable */
        __ADC1_CLK_DISABLE();

        /**ADC1 GPIO Configuration    
        PC1     ------> ADC1_IN11
        PC2     ------> ADC1_IN12
        PC3     ------> ADC1_IN13
        PA0-WKUP     ------> ADC1_IN0
        PA1     ------> ADC1_IN1
        PC4     ------> ADC1_IN14
        PC5     ------> ADC1_IN15 
        */
        HAL_GPIO_DeInit(GPIOC, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|
                               GPIO_PIN_5);

        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_0|GPIO_PIN_1);

    } else if(hadc->Instance==ADC2) {
        /* Peripheral clock disable */
        __ADC2_CLK_DISABLE();

        /**ADC2 GPIO Configuration    
        PC0     ------> ADC2_IN10 
        */
        HAL_GPIO_DeInit(GPIOC, GPIO_PIN_0);
    }
} 

uint32_t getSteeringValue()
{
    uint32_t steeringValue = 0;
    uint32_t i;

    for (i = 0; i < 8; i++)
    {
        steeringValue += ADCData[i * 2];
    }

    return steeringValue / 8;
}

uint32_t getPedalValue()
{
    uint32_t pedalValue = 0;
    uint32_t i;

    for (i = 0; i < 8; i++)
    {
        pedalValue += ADCData[(i * 2) + 1];
    }

    return pedalValue / 8;
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
