#include <stdint.h>

#include "boot.h"
#include "flash.h"
#include "usb_otg.h"

#define CHUNK_SIZE 64U
#define ERASE_FLASH_KEY ((uint32_t)0x666C6170)

/** 32 Bit message type **/
typedef enum msgType{
    DATA = 0, CHECKSUM = 1, SIZE = 2, ERASE_FLASH = 3, NONE = 65535
}msgType_t;

/** Response Data for USB **/
uint8_t okResponseData = 0x00;
uint8_t failedResponseData = 0x01;

void runBootFSM(uint32_t data){
    static uint32_t* dataAddr = USER_APP_BASE_PTR;

    static msgType_t msg = NONE;
    static uint32_t count = 0;
    static uint32_t size = 0;

    msgType_t newMsg;

    switch(msg)
    {
        case DATA:
            writeFlash(dataAddr, data);
            dataAddr++;

            // Writing one word at a time
            count += 4;

            if(count % CHUNK_SIZE == 0 || count == size){
                usbWrite(&okResponseData, 1U);
            }

            if(count == size){
                completeWrite();
                msg = NONE;
            }

            break;

        case CHECKSUM:
            if(data == 0x0000AAAA)
            {
                usbWrite(&okResponseData, 1U);

                // Give time to application to read data
                HAL_Delay(10);

                jumpToApp(USER_APP_BASE_PTR);
            }
            else
            {
                usbWrite(&failedResponseData, 1U);
            }

            msg = NONE;
            break;

        case SIZE:
            size = data;
            usbWrite(&okResponseData, 1U);

            msg = NONE;
            break;

        case ERASE_FLASH:
            if(data == ERASE_FLASH_KEY && size != 0)
            {
                writeInit(size);
                usbWrite(&okResponseData, 1U);
            }
            else
            {
                usbWrite(&failedResponseData, 1U);
            }

            msg = NONE;
            break;

        case NONE:
            // Data is the message type
            newMsg = (msgType_t)data;

            // Signal to bootloader successful switch to data mode
            if(newMsg == DATA)
            {
                usbWrite(&okResponseData, 1U);
            }

            msg = newMsg;
            break;

        default:
            msg = NONE;
            break;
    }
}

void jumpToApp(uint32_t* address)
{
    // Disable Interrupts
    int i;
    for(i = 0; i < 8; i++){
        NVIC->ICER[i] = 0xFFFFFFFF;
    }

    // Disable Interrupting Peripherals
    MX_USB_OTG_FS_USB_DeInit();

    // Clear pending interrupt requests
    for(i = 0; i < 8; i++){
        NVIC->ICPR[i] = 0xFFFFFFFF;
    }

    // Disable systick and clear pending exception bit
    SysTick->CTRL = 0;
    SCB->ICSR |= SCB_ICSR_PENDSTCLR_Msk;

    // Disable individual fault handlers
    SCB->SHCSR &= ~( SCB_SHCSR_USGFAULTENA_Msk | \
                  SCB_SHCSR_BUSFAULTENA_Msk | \
                  SCB_SHCSR_MEMFAULTENA_Msk ) ;

    // Set SP to first entry in the vector table
    __set_MSP(address[0]);

    // Set PC to reset value in vector table
    ((void (*)(void))address[1])();
}
