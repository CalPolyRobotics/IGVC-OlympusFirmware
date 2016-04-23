/**
  ******************************************************************************
  * File Name          : USB_OTG.c
  * Description        : This file provides code for the configuration
  *                      of the USB_OTG instances.
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

#include "usb_otg.h"
#include "gpio.h"
#include "usart.h"
#include "main.h"
#include "comms.h"
#include <string.h>

USBD_HandleTypeDef  USBD_Device;
uint8_t myBuffer[128];
buffer8_t usbReceiveBuffer;

static int8_t usbReceive(uint8_t* data, uint32_t* len)
{
    buffer8_write(&usbReceiveBuffer, data, *len);

    USBD_CDC_ReceivePacket(&USBD_Device);

    return USBD_OK;
}

static int8_t tunnelInit(void)
{
    memset(myBuffer, 0, 128);

    buffer8_init(&usbReceiveBuffer, myBuffer, 128);

    USBD_CDC_SetRxBuffer(&USBD_Device, myBuffer);
    return USBD_OK;
}

static int8_t dummyDeinit(void)
{   
    return USBD_OK;
}

static int8_t dummyControl(uint8_t cmd, uint8_t* pbuf, uint16_t len)
{
    return USBD_OK;
}

USBD_CDC_ItfTypeDef itfTest = {tunnelInit, dummyDeinit, dummyControl, usbReceive};

void usbWrite(uint8_t* data, uint32_t len)
{
    USBD_CDC_SetTxBuffer(&USBD_Device, data, len);
    USBD_CDC_TransmitPacket(&USBD_Device);
}

void MX_USB_OTG_FS_USB_Init(void)
{
    /* Init Device Library */
    USBD_Init(&USBD_Device, &VCP_Desc, 0);

    /* Add Supported Class */
    USBD_RegisterClass(&USBD_Device, USBD_CDC_CLASS);

    /* Add CDC Interface Class */
    //USBD_CDC_RegisterInterface(&USBD_Device, &USBD_CDC_fops);
    USBD_CDC_RegisterInterface(&USBD_Device, &itfTest);

    USB_OTG_FS->GCCFG |= USB_OTG_GCCFG_NOVBUSSENS;

    /* Start Device Process */
    USBD_Start(&USBD_Device);
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
