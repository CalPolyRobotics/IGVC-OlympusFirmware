#include "gpio.h"

#define ADC2_SAMPLES 8
#define ADC2_CHANNELS_IN_USE 2

//static uint16_t ADCData[ADC2_SAMPLES * ADC2_CHANNELS_IN_USE];
static uint16_t ADCData[ADC2_SAMPLES * ADC2_CHANNELS_IN_USE];

ADC_HandleTypeDef hadc1;
ADC_HandleTypeDef hadc2;

/* ADC1 init function */
void MX_ADC1_Init(void)
{
}

/* ADC2 init function */
void MX_ADC2_Init(void)
{

    ADC2->CR1 = ADC_CR1_SCAN; //Enable SCAN Mode 

    ADC2->CR2 = ADC_CR2_DDS |  //Enable DMA Mode
                ADC_CR2_DMA |  //Enable DMA Mode
                ADC_CR2_CONT | //Continuous Mode
                ADC_CR2_ADON;  //Turn the ADC on, but don't start it

    ADC2->SMPR2 = (ADC_SMPR2_SMP0_2) | //Sample for 28 cycles
                  (ADC_SMPR2_SMP1_2);  //Sample for 28 cycles

    ADC2->SQR1 = (ADC_SQR1_L_0); //Two conversions in sequence
    ADC2->SQR3 = (10) |     //Channel 10 in the first conversion
                 (11 << 5); //Channel 11 in the second conversion

    ADC->CCR = ADC_CCR_DMA_0; //Disable the DMA for multi-mode

    // Enable GPIO C 0,1 as Analog inputs
    GPIO_InitTypeDef GPIO_InitStruct;

    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    //Stream 3, Channel 1

    //Initialize the DMA to continuously place data into the ADCData array
    DMA2_Stream3->CR = (DMA_SxCR_CHSEL_0) | //Use Channel 0 ()
                 (DMA_SxCR_PL) |            //Very High priority
                 (DMA_SxCR_MSIZE_0) |       //Half-Word Size
                 (DMA_SxCR_PSIZE_0) |       //Half-Word Size
                 (DMA_SxCR_MINC) |          //Memory Increment
                 (DMA_SxCR_CIRC);           //Circular Mode

    DMA2_Stream3->NDTR = ADC2_SAMPLES * ADC2_CHANNELS_IN_USE; //Number of data to transfer

    DMA2_Stream3->PAR = (uint32_t)&(ADC2->DR); //ADC2 Data Regist
    DMA2_Stream3->M0AR = (uint32_t)ADCData;    //ADCData Array is destination

    DMA2_Stream3->CR |= (DMA_SxCR_EN);        //Enable DMA2 Stream 3

    ADC2->CR2 |= ADC_CR2_SWSTART; //Start ADC2
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


