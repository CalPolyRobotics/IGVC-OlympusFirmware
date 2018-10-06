#include "stm32f2xx_hal.h"
#include "stm32f2xx_hal_conf.h"
#include "usb_otg.h"
#include "gpio.h"
#include "doubleBuffer.h"
#include "usbd_desc.h"
#include "usbd_cdc.h" 

#define USB_RECEIVE_BUFFER_SIZE 2048
#define USB_SEND_BUFFER_SIZE 64
#define USB_SEND_BUFFER_NUM  4

USBD_HandleTypeDef USBD_Device;

static uint8_t usbRecvData[USB_RECEIVE_BUFFER_SIZE];
DoubleBuffer_t usbReceiveBuffer;

static bool CDC_Device_Initialized = false;

static int8_t CDC_Receive(uint8_t* data, uint32_t* len);
static int8_t CDC_Init(void);
static int8_t CDC_Deinit(void);
static int8_t CDC_Control(uint8_t cmd, uint8_t* pbuf, uint16_t len);

static USBD_CDC_ItfTypeDef cdcInterface = {CDC_Init, CDC_Deinit, CDC_Control, CDC_Receive};

static int8_t CDC_Receive(uint8_t* data, uint32_t* len)
{
    if(!doubleBuffer_write(&usbReceiveBuffer, data, *len)){
        return USBD_OK;
    }

    return USBD_CDC_ReceivePacket(&USBD_Device);
}

static int8_t CDC_Init(void)
{
    doubleBuffer_init(&usbReceiveBuffer, usbRecvData, sizeof(usbRecvData));

    return USBD_CDC_SetRxBuffer(&USBD_Device, usbRecvData);
}

static int8_t CDC_Deinit(void)
{
    return USBD_CDC_RegisterInterface(&USBD_Device, &cdcInterface);
}

static int8_t CDC_Control(uint8_t cmd, uint8_t* pbuf, uint16_t len)
{
    return USBD_OK;
}

uint8_t usbWrite(uint8_t* data, uint16_t len){
    // Transmit the current active buffer across USB
    // Actual transmission will take place in a later USB IRQ
    USBD_CDC_SetTxBuffer(&USBD_Device, data, len);
    return USBD_CDC_TransmitPacket(&USBD_Device);
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

    CDC_Device_Initialized = true;
}

void MX_USB_OTG_FS_USB_DeInit(void)
{
    if(CDC_Device_Initialized){
        USBD_DeInit(&USBD_Device);
    }
}
