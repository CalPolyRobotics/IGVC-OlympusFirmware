#include <stdint.h>

#include "usart.h"

void bootloaderFSM(){

    //if(RCC -> CSR & SFTRSTF){
        // Boot loader

        uint32_t* writeLoc = FLASH_PROG_START;
        uint32_t wrData = 0; 
        uint8_t cnt = 0;
        uint8_t data;

        writeInit();

        while(1){
            serviceTxDma();

            while((usartRead(&data, 1))){
                wrData |= 0xFFFFFFFF & data;
                cnt ++;

                if(cnt == 4){
                    // When Data Finished Reading
                    if(wrData == PROG_COMPLETE){
                        writeComplete();

                        // run checksum and Jump
                        //printf("Running Checksum\r\n");
                        jumpToApp();
                    }else{
                        writeFlash(writeLoc, wrData);
                        while(FLASH -> SR & FLASH_SR_BSY);

                        writeLoc ++;

                        cnt = 0;
                    }
                }

                wrData <<= 8;
            }
        }
    //}else{
        // Run Checksum
        // Jump to application
    //}
}

