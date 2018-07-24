#include <stdint.h>

#include "boot.h"
#include "flash.h"
#include "usb_otg.h"

typedef enum bootState{
    HEADER, DATA3, DATA2, DATA1, DATA0, COMPLETE
}bootState_t;

typedef union uint32{
    uint32_t u32;
    uint16_t u16[2];
    uint8_t  u8[4];
}uint32_u;

typedef struct header{
    uint32_u device;
    uint32_u progSize;
} header_t;


void runBootFSM(uint8_t data){
    static bootState_t state = HEADER;
    static uint32_t* dataAddr = USER_APP_BASE_PTR;
    static uint32_t count = 0;

    static header_t header;
    static uint32_u word;

    switch(state)
    {
        case HEADER:
            // Make sure to start on the first character of the
            // header (Avoid incoming AT commands)
            if(count == 0 && data != 'O')
            {
                break;
            }

            if(count < sizeof(header.device))
            {
                header.device.u8[count] = data;
            }
            else if(count >= sizeof(header.device))
            {
                header.progSize.u8[count - sizeof(header.device)] = data;
            }

            if(++count == sizeof(header)){
                // Our program counts 32-bit words instead of bytes
                header.progSize.u32 /= 4u;
                state = DATA0;
                count = 0;
            }
            break;

        case DATA3:
            word.u8[3] = data;
            writeFlash(dataAddr, word.u32);
            dataAddr++;

            if(count++ == header.progSize.u32){
                count = 0;
                completeWrite();
                GPIOB->BSRRL |= (uint32_t)GPIO_PIN_7;

                state = COMPLETE;
            }else{
                state = DATA0;
            }
            break;

        case DATA2:
            word.u8[2] = data;
            state = DATA3;
            break;

        case DATA1:
            word.u8[1] = data;
            state = DATA2;
            break;

        case DATA0:
            word.u8[0] = data;
            state = DATA1;
            break;

        case COMPLETE:
            //completeWrite();
            jumpToApp(USER_APP_BASE_PTR);
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
