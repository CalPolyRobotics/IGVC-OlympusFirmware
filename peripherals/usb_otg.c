
#include "usb_otg.h"
#include "gpio.h"
#include "usart.h"
#include "main.h"
#include "comms.h"
#include "config.h"

#include <string.h>

USBD_HandleTypeDef  USBD_Device;
static uint8_t usbRecvData[USB_RECEIVE_BUFFER_SIZE];
buffer8_t usbReceiveBuffer;

static int8_t usbReceive(uint8_t* data, uint32_t* len)
{
    buffer8_write(&usbReceiveBuffer, data, *len);

    //usbWrite("Read bytes\r\n", 12);
    //while ((*len)--)
    //{
        //runCommsFSM(*data++);
    //}

    USBD_CDC_ReceivePacket(&USBD_Device);

    return USBD_OK;
}

static int8_t tunnelInit(void)
{
    buffer8_init(&usbReceiveBuffer, usbRecvData, sizeof(usbRecvData));

    USBD_CDC_SetRxBuffer(&USBD_Device, usbRecvData);
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

//Write data out the USB CDC port
//Return 1 on success and 0 on failure
uint8_t usbWrite(uint8_t* data, uint32_t len)
{
    uint8_t writeStatus;

    USBD_CDC_SetTxBuffer(&USBD_Device, data, len);

    do {
        writeStatus = USBD_CDC_TransmitPacket(&USBD_Device);
    } while (writeStatus == USBD_BUSY);

    return writeStatus == USBD_OK;
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
