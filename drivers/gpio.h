#ifndef __GPIO_H__
#define __GPIO_H__

#include "main.h"

//The following functions are meant to be used with the defines in config.h
//The port and pinNum arguments should be replaced with one of the GPIO defines
//which defines poth of these parameters in one preprocessor statement.
void initGPIOOutput_PP(GPIO_TypeDef* port, uint8_t pinNum);
void initGPIOOutput_OD(GPIO_TypeDef* port, uint8_t pinNum);
void initGPIOOutput_OD_PU(GPIO_TypeDef* port, uint8_t pinNum);
void initGPIOOutput_OD_PD(GPIO_TypeDef* port, uint8_t pinNum);
void initGPIOInput(GPIO_TypeDef* port, uint8_t pinNum);
void initGPIOAlternate_PP(GPIO_TypeDef* port, uint8_t pinNum, uint32_t alternate);
void initGPIOAlternate_OD(GPIO_TypeDef* port, uint8_t pinNum, uint32_t alternate);
void initGPIOAlternate_OD_PU(GPIO_TypeDef* port, uint8_t pinNum, uint32_t alternate);
void initGPIOAlternate_OD_PD(GPIO_TypeDef* port, uint8_t pinNum, uint32_t alternate);
void initGPIOAnalog(GPIO_TypeDef* port, uint8_t pinNum);

void GPIO_WritePin(GPIO_TypeDef* port, uint8_t pinNum, uint8_t state);
void GPIO_SetPin(GPIO_TypeDef* port, uint8_t pinNum);
void GPIO_ResetPin(GPIO_TypeDef* port, uint8_t pinNum);
void GPIIO_ReadPin(GPIO_TypeDef* port, uint8_t pinNum);

#endif