
#include "main.h"
#include "gpio.h"

void initGPIOOutput_PP(GPIO_TypeDef* port, uint8_t pinNum)
{
    GPIO_InitTypeDef gpio;

    gpio.Pin = (1 << pinNum);
    gpio.Mode = GPIO_MODE_OUTPUT_PP;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_HIGH;

    HAL_GPIO_Init(port, &gpio);
}

void initGPIOOutput_OD(GPIO_TypeDef* port, uint8_t pinNum)
{
    GPIO_InitTypeDef gpio;

    gpio.Pin = (1 << pinNum);
    gpio.Mode = GPIO_MODE_OUTPUT_OD;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_HIGH;

    HAL_GPIO_Init(port, &gpio);
}

void initGPIOOutput_OD_PU(GPIO_TypeDef* port, uint8_t pinNum)
{
    GPIO_InitTypeDef gpio;

    gpio.Pin = (1 << pinNum);
    gpio.Mode = GPIO_MODE_OUTPUT_OD;
    gpio.Pull = GPIO_PULLUP;
    gpio.Speed = GPIO_SPEED_FREQ_HIGH;

    HAL_GPIO_Init(port, &gpio);
}

void initGPIOOutput_OD_PD(GPIO_TypeDef* port, uint8_t pinNum)
{
    GPIO_InitTypeDef gpio;

    gpio.Pin = (1 << pinNum);
    gpio.Mode = GPIO_MODE_OUTPUT_OD;
    gpio.Pull = GPIO_PULLDOWN;
    gpio.Speed = GPIO_SPEED_FREQ_HIGH;

    HAL_GPIO_Init(port, &gpio);
}

void initGPIOInput(GPIO_TypeDef* port, uint8_t pinNum)
{
    GPIO_InitTypeDef gpio;

    gpio.Pin = (1 << pinNum);
    gpio.Mode = GPIO_MODE_INPUT;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_HIGH;

    HAL_GPIO_Init(port, &gpio);
}

void initGPIOAlternate_PP(GPIO_TypeDef* port, uint8_t pinNum, uint32_t alternate)
{
    GPIO_InitTypeDef gpio;

    gpio.Pin = (1 << pinNum);
    gpio.Mode = GPIO_MODE_AF_PP;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_HIGH;
    gpio.Alternate = alternate;

    HAL_GPIO_Init(port, &gpio);
}

void initGPIOAlternate_OD(GPIO_TypeDef* port, uint8_t pinNum, uint32_t alternate)
{
    GPIO_InitTypeDef gpio;

    gpio.Pin = (1 << pinNum);
    gpio.Mode = GPIO_MODE_AF_OD;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_HIGH;
    gpio.Alternate = alternate;

    HAL_GPIO_Init(port, &gpio);
}

void initGPIOAlternate_OD_PU(GPIO_TypeDef* port, uint8_t pinNum, uint32_t alternate)
{
    GPIO_InitTypeDef gpio;

    gpio.Pin = (1 << pinNum);
    gpio.Mode = GPIO_MODE_AF_OD;
    gpio.Pull = GPIO_PULLUP;
    gpio.Speed = GPIO_SPEED_FREQ_HIGH;
    gpio.Alternate = alternate;

    HAL_GPIO_Init(port, &gpio);
}

void initGPIOAlternate_OD_PD(GPIO_TypeDef* port, uint8_t pinNum, uint32_t alternate)
{
    GPIO_InitTypeDef gpio;

    gpio.Pin = (1 << pinNum);
    gpio.Mode = GPIO_MODE_AF_OD;
    gpio.Pull = GPIO_PULLDOWN;
    gpio.Speed = GPIO_SPEED_FREQ_HIGH;
    gpio.Alternate = alternate;

    HAL_GPIO_Init(port, &gpio);
}

void initGPIOAnalog(GPIO_TypeDef* port, uint8_t pinNum)
{
    GPIO_InitTypeDef gpio;

    gpio.Pin = (1 << pinNum);
    gpio.Mode = GPIO_MODE_ANALOG;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_HIGH;

    HAL_GPIO_Init(port, &gpio);   
}

void GPIO_WritePin(GPIO_TypeDef* port, uint8_t pinNum, uint8_t state)
{
    HAL_GPIO_WritePin(port, (1 << pinNum), state);
}

void GPIO_SetPin(GPIO_TypeDef* port, uint8_t pinNum)
{
    GPIO_WritePin(port, pinNum, 1);
}

void GPIO_ResetPin(GPIO_TypeDef* port, uint8_t pinNum)
{
    GPIO_WritePin(port, pinNum, 0);
}

uint8_t GPIO_ReadPin(GPIO_TypeDef* port, uint8_t pinNum)
{
    return HAL_GPIO_ReadPin(port, (1 << pinNum)) == GPIO_PIN_SET;
}
