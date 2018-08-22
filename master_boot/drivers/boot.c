#include <stdint.h>
#include <string.h>
#include <stdbool.h>

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

#define NO_ERR             ((uint8_t)0x00)
#define ERR_DATA_WRITE     ((uint8_t)0x01)
#define ERR_CHECKSUM       ((uint8_t)0x02)
#define ERR_INVALID_HEADER ((uint8_t)0x03)
#define ERR_FLASH_ERASE    ((uint8_t)0x04)
#define ERR_REBOOT_START   ((uint8_t)0x05)
#define ERR_REBOOT_TIMEOUT ((uint8_t)0x06)

/** 32 Bit message type **/
typedef enum msgType{
    DATA = 0, CHECKSUM = 1, HEADER = 2, DMY = 65535
}msgType_t;

extern uint8_t submoduleCommsBuff[256];

static void writeResponse(uint8_t response);

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
uint8_t responseData = NO_ERR;

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
                        if(!checkStatus(module, 50, 5)){
                            writeResponse(ERR_DATA_WRITE);
                            msg = DMY;
                            break;
                        }
                    }
                }
            }

            if(count % CHUNK_SIZE == 0 || count == header.size){
                writeResponse(NO_ERR);
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
                writeResponse(NO_ERR);

                // Give time to application to read data
                HAL_Delay(10);

                jumpToApp(USER_APP_BASE_PTR);
            }
            else
            {
                writeResponse(ERR_CHECKSUM);
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
                    writeResponse(ERR_INVALID_HEADER);
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

                    messageSubmodule(module, 0x0A, submoduleCommsBuff, 0, 1);
                    if(submoduleCommsBuff[0] != COMMS_OK){
                        writeResponse(ERR_REBOOT_START);
                        msg = DMY;
                        break;
                    }

                    if(!checkStatus(module, 50, 100))
                    {
                        writeResponse(ERR_REBOOT_TIMEOUT);
                        msg = DMY;
                        break;
                    }

                    size_t headerSize = sizeof(header.size) + sizeof(header.fkey);
                    memcpy(submoduleCommsBuff, (uint8_t*)&(header.size), headerSize);
                    messageSubmodule(module, 0x01, submoduleCommsBuff, headerSize, 1);

                    if(!checkStatus(module, 50, 100))
                    {
                        /* writeHeader can take longer than expected because of the flash erase */
                        writeResponse(ERR_FLASH_ERASE);
                        msg = DMY;
                        break;
                    }
                }

                writeResponse(NO_ERR);
                msg = DMY;
            }
            break;

        case DMY:
            // Data is the message type
            newMsg = (msgType_t)data;

            // Signal to bootloader successful switch to data mode
            if(newMsg == DATA)
            {
                writeResponse(NO_ERR);
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

static void writeResponse(uint8_t response)
{
    responseData = response;
    usbWrite(&responseData, sizeof(responseData));
}
