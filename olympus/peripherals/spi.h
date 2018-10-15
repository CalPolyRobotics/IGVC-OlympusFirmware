/**
  ******************************************************************************
  * File Name          : SPI.h
  * Description        : This file provides code for the configuration
  *                      of the SPI instances.
  ******************************************************************************
  *
  * COPYRIGHT(c) 2016 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
#ifndef __spi_H
#define __spi_H
#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32f2xx_hal.h"

#define PORT_SS_ZADC GPIOA
#define PIN_SS_ZADC  GPIO_PIN_8

#define PORT_SS_THDAC GPIOB
#define PIN_SS_THDAC  GPIO_PIN_6

#define PORT_SS_THADC GPIOB
#define PIN_SS_THADC  GPIO_PIN_7

#define PORT_SS_APOLLO GPIOB
#define PIN_SS_APOLLO  GPIO_PIN_9

#define PORT_INT_APOLLO GPIOB
#define PIN_INT_APOLLO GPIO_PIN_10

#define PORT_SS_HERA GPIOC
#define PIN_SS_HERA  GPIO_PIN_3

#define PORT_INT_HERA GPIOB
#define PIN_INT_HERA GPIO_PIN_11

#define PORT_SS_HEPHAESTUS GPIOC
#define PIN_SS_HEPHAESTUS  GPIO_PIN_8

#define PORT_INT_HEPHAESTUS GPIOB
#define PIN_INT_HEPHAESTUS GPIO_PIN_12

#define PORT_SS_IRIS GPIOC
#define PIN_SS_IRIS  GPIO_PIN_9

#define PORT_SS_JANUS GPIOC
#define PIN_SS_JANUS  GPIO_PIN_13

#define PORT_INT_JANUS GPIOB
#define PIN_INT_JANUS  GPIO_PIN_13

extern SPI_HandleTypeDef hspi3;

void MX_SPI3_Init(void);

#ifdef __cplusplus
}
#endif
#endif /*__ spi_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
