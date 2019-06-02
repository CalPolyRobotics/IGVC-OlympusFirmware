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
#define HEPHAESTUS_KEY  ((uint32_t)0x48455048)
#define HERMES_KEY      ((uint32_t)0x4845524D)
#define JANUS_KEY       ((uint32_t)0x4A414E55)
#define ZEUS_KEY        ((uint32_t)0x5A455553)

#define NO_ERR             ((uint8_t)0x00)
#define ERR_DATA_WRITE     ((uint8_t)0x01)
#define ERR_CHECKSUM       ((uint8_t)0x02)
#define ERR_INVALID_HEADER ((uint8_t)0x03)
#define ERR_FLASH_ERASE    ((uint8_t)0x04)
#define ERR_REBOOT_FAIL    ((uint8_t)0x05)
#define ERR_REBOOT_START   ((uint8_t)0x06)
#define ERR_REBOOT_TIMEOUT ((uint8_t)0x07)
#define ERR_REBOOT_STATUS  ((uint8_t)0x08)

/** 32 Bit message type **/
typedef enum msgType{
    DATA = 0, CHECKSUM = 1, HEADER = 2, JUMP=3, DMY = 65535
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
uint32_t checksum = 0;

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
                checksum += data;
            }
            else
            {
                memcpy(submoduleCommsBuff + (count % CHUNK_SIZE), &data, sizeof(data));
                count += 4u;

                if((count % CHUNK_SIZE) == 0 || count == header.size)
                {
                    if(count == header.size && count % CHUNK_SIZE != 0u)
                    {
                        /* Zero fill end of buffer if last packet */
                        memset(submoduleCommsBuff + (count % CHUNK_SIZE), 0u, CHUNK_SIZE - (count % CHUNK_SIZE));
                    }

                    if(messageSubmodule(module, 0x02, submoduleCommsBuff, CHUNK_SIZE, 0u, BOOT_SUBMODULE_TIMEOUT) != COMMS_OK)
                    {
                        writeResponse(ERR_DATA_WRITE);
                        msg = DMY;
                        break;
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
            if(module == OLYMPUS)
            {
                if(data == checksum)
                {
                    writeResponse(NO_ERR);
                }
                else
                {
                    writeResponse(ERR_CHECKSUM);
                }
            }
            else
            {
                memcpy(submoduleCommsBuff, &data, sizeof(data));
                if(messageSubmodule(module, 0x03, submoduleCommsBuff, 4u, 0u, BOOT_SUBMODULE_TIMEOUT) != COMMS_OK)
                {
                    writeResponse(ERR_CHECKSUM);
                    msg = DMY;
                    break;
                }

                writeResponse(NO_ERR);
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
                    case HERMES_KEY:
                        module = HERMES;
                        break;
                    case JANUS_KEY:
                        module = JANUS;
                        break;
                    case ZEUS_KEY:
                        module = ZEUS;
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
                    checksum = 0;
                    writeInit(header.size);
                }
                else
                {
                    if(messageSubmodule(module, 0x00, submoduleCommsBuff, 0u, 0u, BOOT_SUBMODULE_TIMEOUT) != COMMS_OK)
                    {
                        writeResponse(ERR_REBOOT_FAIL);
                        msg = DMY;
                        break;
                    }

                    if(messageSubmodule(module, 0x0A, submoduleCommsBuff, 0u, 0u, BOOT_SUBMODULE_TIMEOUT) != COMMS_OK)
                    {
                        writeResponse(ERR_REBOOT_START);
                        msg = DMY;
                        break;
                    }

                    /** Give Time for Restart **/
                    HAL_Delay(200u);

                    if(messageSubmodule(module, 0x00, submoduleCommsBuff, 0u, 0u, BOOT_SUBMODULE_TIMEOUT) != COMMS_OK)
                    {
                        writeResponse(ERR_REBOOT_STATUS);
                        msg = DMY;
                        break;
                    }

                    size_t headerSize = sizeof(header.size) + sizeof(header.fkey);
                    memcpy(submoduleCommsBuff, (uint8_t*)&(header.size), headerSize);

                    if(messageSubmodule(module, 0x01, submoduleCommsBuff, headerSize, 0u, BOOT_FLASH_TIMEOUT) != COMMS_OK)
                    {
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

            if(newMsg == JUMP)
            {
                writeResponse(NO_ERR);

                // Give time to process message
                HAL_Delay(10);

                jumpToApp(USER_APP_BASE_PTR);
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
