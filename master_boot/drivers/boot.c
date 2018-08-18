#include <stdint.h>
#include <string.h>

#include "boot.h"
#include "flash.h"
#include "usb_otg.h"
#include "spi.h"
#include "submoduleComms.h"

#define CHUNK_SIZE 64U
#define ERASE_FLASH_KEY ((uint32_t)0x666C6170)

#define OLYMPUS_KEY     ((uint32_t)0x4F4C594D)
#define APOLLO_KEY      ((uint32_t)0x41504F4C)
#define HERA_KEY        ((uint32_t)0x48455241)
#define HEPHAESTUS_KEY  ((uint32_t)0x48455048)
#define HERMES_KEY      ((uint32_t)0x4845524D)
#define JANUS_KEY       ((uint32_t)0x4A414E55)

/** 32 Bit message type **/
typedef enum msgType{
    DATA = 0, CHECKSUM = 1, HEADER = 2, DMY = 65535
}msgType_t;

extern uint8_t submoduleCommsBuff[256];

/**
 * Bootloader Header
 * Device Key, Program Size, and Erase Flash Key
 **/
typedef struct bootHeader{
    uint32_t dkey;
    uint32_t size;
    uint32_t fkey;
}bootHeader_t;

/** Response Data for USB **/
uint8_t okResponseData = 0x00;
uint8_t failedResponseData = 0x01;

void runBootFSM(uint32_t data){
    static uint32_t* dataAddr = USER_APP_BASE_PTR;

    static msgType_t msg = DMY;
    static uint32_t count = 0;
    static module_t module = NONE;
    static bootHeader_t header;

    msgType_t newMsg;

    switch(msg)
    {
        case DATA:
            if(module == OLYMPUS)
            {
                writeFlash(dataAddr, data);
                dataAddr++;
                count += 4u;
            }
            else
            {
                memcpy(submoduleCommsBuff + (count % CHUNK_SIZE), &data, sizeof(data));
                count += 4u;

                if((count % CHUNK_SIZE) == 0 || count == header.size)
                {
                    messageSubmodule(module, 0x02, submoduleCommsBuff, CHUNK_SIZE, 1);

                    if(submoduleCommsBuff[0] != COMMS_OK)
                    {
                        failedResponseData = 0x01;
                        usbWrite(&failedResponseData, 1U);
                        msg = DMY;
                        break;
                    }
                }
            }

            if(count % CHUNK_SIZE == 0 || count == header.size){
                usbWrite(&okResponseData, 1U);
            }


            if(count == header.size){
                if(module == OLYMPUS){
                    completeWrite();
                }

                count = 0;
                msg = DMY;
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

            msg = DMY;
            break;

        case HEADER:
            if(count == 0)
            {
                header.dkey = data;
                count++;
            }
            else if(count == 1)
            {
                header.size = data;
                count++;
            }
            else
            {
                header.fkey = data;
                count = 0;

                switch(header.dkey)
                {
                    case OLYMPUS_KEY:
                        module =  OLYMPUS;
                        break;
                    case APOLLO_KEY:
                        module = APOLLO;
                        break;
                    case HEPHAESTUS_KEY:
                        module = HEPHAESTUS;
                        break;
                    case HERA_KEY:
                        module = HERA;
                        break;
                    case JANUS_KEY:
                        module = JANUS;
                        break;
                    default:
                        module = NONE;
                        break;
                }

                if(header.fkey != ERASE_FLASH_KEY || module == NONE)
                {
                    failedResponseData = 0x02;
                    usbWrite(&failedResponseData, 1U);
                    msg = DMY;
                    break;
                }
                
                if(module == OLYMPUS)
                {
                    // Erase Flash
                    writeInit(header.size);
                }
                else
                {
                    messageSubmodule(module, 0x00, submoduleCommsBuff, 0, 1);
                    if(submoduleCommsBuff[0] != COMMS_OK)
                    {
                        failedResponseData = 0x01;
                        usbWrite(&failedResponseData, 1U);
                        msg = DMY;
                        break;
                    }

                    size_t headerSize = sizeof(header.size) + sizeof(header.fkey);
                    memcpy(submoduleCommsBuff, (uint8_t*)&(header.size), headerSize);
                    messageSubmodule(module, 0x01, submoduleCommsBuff, headerSize, 1);

                    if(submoduleCommsBuff[0] != COMMS_OK)
                    {
                        failedResponseData = 0x01;
                        usbWrite(&failedResponseData, 1U);
                        msg = DMY;
                        break;
                    }
                }

                usbWrite(&okResponseData, 1U);
                msg = DMY;
            }
            break;

        case DMY:
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
            msg = DMY;
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
    HAL_SPI_DeInit(&hspi3);

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


