
#include "usb_otg.h"
#include "gpio.h"
#include "usart.h"
#include "main.h"
#include "comms.h"
#include "doubleBuffer.h"
#include "config.h"

#include <string.h>

USBD_HandleTypeDef  USBD_Device;
static uint8_t usbRecvData[USB_RECEIVE_BUFFER_SIZE];
static uint8_t reinitUSB = 0;
DoubleBuffer_t usbReceiveBuffer;

static int8_t usbReceive(uint8_t* data, uint32_t* len)
{
    doubleBuffer_write(&usbReceiveBuffer, data, *len);

    USBD_CDC_ReceivePacket(&USBD_Device);

    return USBD_OK;
}

static int8_t tunnelInit(void)
{
    doubleBuffer_init(&usbReceiveBuffer, usbRecvData, sizeof(usbRecvData));

    USBD_CDC_SetRxBuffer(&USBD_Device, usbRecvData);
    return USBD_OK;
}

static int8_t dummyDeinit(void)
{
    reinitUSB = 1;

    return USBD_OK;
}

static int8_t dummyControl(uint8_t cmd, uint8_t* pbuf, uint16_t len)
{
    return USBD_OK;
}

static USBD_CDC_ItfTypeDef cdcInterface = {tunnelInit, dummyDeinit, dummyControl, usbReceive};

static uint8_t usbTransmitBuffers[USB_SEND_BUFFER_NUM][USB_SEND_BUFFER_SIZE];
static uint32_t usbTransmitBufferLengths[USB_SEND_BUFFER_NUM];
static volatile uint32_t nextUsbBuffer = 0;
static volatile uint32_t activeUsbBuffer = 0;
static volatile uint32_t usbIsActive = 0;
static volatile uint32_t usbBufferOverrun = 0;
static volatile uint32_t usbTransferHasCompleted = 0;

void usbWrite(uint8_t* data, uint32_t size)
{
    uint32_t bufferSpace;
    uint32_t bytesToWrite;

    if ((usbBufferOverrun == 0) &&
        (usbIsActive == 0 || nextUsbBuffer != activeUsbBuffer))
    {
        printf("Writing %lu bytes\r\n", size);

        while (size > 0)
        {
            // Calculate the number of bytes to write
            bufferSpace = USB_SEND_BUFFER_SIZE - usbTransmitBufferLengths[nextUsbBuffer];
            bytesToWrite = size < bufferSpace ? size : bufferSpace;

            // Copy data into usb buffer
            memcpy(&usbTransmitBuffers[nextUsbBuffer][usbTransmitBufferLengths[nextUsbBuffer]],
                   data, bytesToWrite);

            // Decrease the remaining size
            size -= bytesToWrite;
            data += bytesToWrite;

            // Decrease space in buffer and go to next buffer in necessary
            usbTransmitBufferLengths[nextUsbBuffer] += bytesToWrite;
            if (usbTransmitBufferLengths[nextUsbBuffer] == USB_SEND_BUFFER_SIZE)
            {
                nextUsbBuffer++;
                if (nextUsbBuffer == USB_SEND_BUFFER_NUM)
                {
                    nextUsbBuffer = 0;
                }

                if (nextUsbBuffer == activeUsbBuffer)
                {
                    printf("WARNING: Overran USB Buffers by %lu bytes. Further USB data is corrupted!!!!!!!!!\r\n", size);
                    printf("Increase the size of the input buffers using USB_SEND_BUFFER_NUM and USB_SEND_BUFFER_SIZE\r\n");

                    usbBufferOverrun = 1;
                    size = 0;
                }
            }
        }
    } else {
        printf("Can't write\r\n");
    }
}

void serviceUSBWrite()
{
    if (usbTransferHasCompleted)
    {
        //printf("USB Transfer Completed\r\n");
        // Set USB as no longer active
        usbIsActive = 0;

        usbTransmitBufferLengths[activeUsbBuffer] = 0;

        // Set the active USB buffer to the next buffer
        activeUsbBuffer++;
        if (activeUsbBuffer >= USB_SEND_BUFFER_NUM)
        {
            activeUsbBuffer = 0;
        }

        // Clear the overrun status
        usbBufferOverrun = 0;

        usbTransferHasCompleted = 0;
    }

    // If usb in inactive and
    // the active buffer has data and
    // we can start another transfer
    if (usbIsActive == 0 &&
        usbTransmitBufferLengths[activeUsbBuffer] > 0)
    {
        // Mark USB as active to not overwrite buffer
        usbIsActive = 1;

        // Increase the nextBuffer ptr
        if (nextUsbBuffer == activeUsbBuffer)
        {
            nextUsbBuffer++;
            if (nextUsbBuffer >= USB_SEND_BUFFER_NUM)
            {
                nextUsbBuffer = 0;
            }
        }

        //printf("Writing %lu bytes to USB stack\r\n", usbTransmitBufferLengths[activeUsbBuffer]);

        // Transmit the current active buffer across USB
        // Actual transmission will take place in a later USB IRQ
        USBD_CDC_SetTxBuffer(&USBD_Device,
                             usbTransmitBuffers[activeUsbBuffer],
                             usbTransmitBufferLengths[activeUsbBuffer]);

        USBD_CDC_TransmitPacket(&USBD_Device);
    }

    if (reinitUSB)
    {
        reinitUSB = 0;

        USBD_CDC_RegisterInterface(&USBD_Device, &cdcInterface);
    }
}

// Called once the USB transfer has completed
void USBD_CDC_DataIn_Hook()
{
    usbTransferHasCompleted = 1;
}

void MX_USB_OTG_FS_USB_Init(void)
{
    /* Init Device Library */
    USBD_Init(&USBD_Device, &VCP_Desc, 0);

    /* Add Supported Class */
    USBD_RegisterClass(&USBD_Device, USBD_CDC_CLASS);

    /* Add CDC Interface Class */
    //USBD_CDC_RegisterInterface(&USBD_Device, &USBD_CDC_fops);
    USBD_CDC_RegisterInterface(&USBD_Device, &cdcInterface);

    USB_OTG_FS->GCCFG |= USB_OTG_GCCFG_NOVBUSSENS;

    /* Start Device Process */
    USBD_Start(&USBD_Device);
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
