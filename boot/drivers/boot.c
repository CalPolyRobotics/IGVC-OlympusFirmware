#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdint.h>

#include "main.h"
#include "usart.h"
#include "characterMapping.h"

#define FLASH_PROG_START   (uint32_t*)0x080E0000
#define FLASH_PROG_END     0x454f4651
#define KEY_1              0x45670123
#define KEY_2              0xCDEF89AB


static void writeFlash(uint32_t);
static void writeInit();
static void writeComplete();

void bootloaderFSM(){

    //if(RCC -> CSR & SFTRSTF){
        // Boot loader

        uint32_t* writeLoc = FLASH_PROG_START;
        uint32_t wrData = 0; 
        uint8_t cnt = 0;
        uint8_t data;

        while(1){
            serviceTxDma();

            while((usartRead(&data, 1))){
                if(data == 0x01){ //Data Finished Reading

                    // Shift data to left align
                    data <<= ((3 - cnt) * 8);

                    writeFlash(writeLoc, data);
                    writeLoc += sizeof(uint32_t);

                    // run checksum and Jump
                    printf("Running Checksum");
                }

                wrData |= 0xFFFFFFFF & data;
                cnt ++;

                if(cnt == 4){
                    writeFlash(wrData);
                    writeLoc += sizeof(uint32_t);

                    cnt = 0;
                }

                wrData <<= 8;
            }

        }
    //}else{
        // Run Checksum
        // Jump to application
    //}

}

static void writeFlash(uint32_t* loc, uint32_t data){
    printf("%lx\r\n", data);

    *loc = data;
    while(FLASH -> SR & FLASH_SR_BSY);
}

static void writeInit(){
    // Unlock Flash for Writing 
    FLASH -> KEYR = KEY_1;
    FLASH -> KEYR = KEY_2;
   
    // Erase Sector 11
    while(FLASH -> SR & FLASH_SR_BSY);     // Wait for Flash to be ready
    FLASH -> CR |= FLASH_CR_SER;           // Set Sector Erase mode
    FLASH -> CR |= FLASH_CR_SNB_3 | FLASH_CR_SNB_2 | FLASH_CR_SNB_0; // Sector 11
    FLASH -> CR |= FLASH_CR_STRT;          // Start Erase

    // Wait for Erase completion
    while(FLASH -> SR & FLASH_SR_BSY);

    // Set parallelism to 32 bit
    FLASH -> CR |= FLASH_CR_PSIZE_1;

    // Set Flash to write mode
    FLASH -> CR |= FLASH_CR_PG;
}

static void writeComplete(){
   FLASH -> CR &= ~FLASH_CR_PG;
}
