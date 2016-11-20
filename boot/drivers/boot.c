#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdint.h>

#include "main.h"
#include "usart.h"
#include "characterMapping.h"

#define FLASH_PROG_START   (uint32_t*)0x080A0000
#define PROG_COMPLETE      0x1FF2EF8B //Reversed Since Data is reversed for write
#define KEY_1              0x45670123
#define KEY_2              0xCDEF89AB


static void writeFlash(uint32_t*, uint32_t);
static void writeInit();
static void writeComplete();
static void eraseSector(uint8_t);
static void jumpToApp();

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

static void writeFlash(uint32_t* loc, uint32_t data){
    //usartWrite((char *)&data, 4); 

    *loc = data;
    while(FLASH -> SR & FLASH_SR_BSY);
}


static void writeInit(){
    // Unlock Flash for Writing 
    FLASH -> KEYR = KEY_1;
    FLASH -> KEYR = KEY_2;
   
    eraseSector(9);
    eraseSector(10);
    eraseSector(11);

    // Set parallelism to 32 bit
    FLASH -> CR |= FLASH_CR_PSIZE_1;

    // Set Flash to write mode
    FLASH -> CR |= FLASH_CR_PG;
}

static void writeComplete(){
    FLASH -> CR &= ~FLASH_CR_PG;

    char *msg = "Goodbye\n";
    usartWrite(msg, 8); 
}

static void jumpToApp(){
    // +4ul is to skip SP at start of bin and + 1ul is to put in thumb mode
    void (* const jumpFunction)(void) = (void (*)(void))(FLASH_PROG_START + 4ul + 1ul);
    //void (* const jumpFunction)(void) = *(FLASH_PROG_START + 4ul) + 1ul;

    // Set the Main stack pointer to the first 4 bytes at FLASH_PROG_START
    __set_MSP((uint32_t)*FLASH_PROG_START);

    jumpFunction();
}

static void eraseSector(uint8_t sector){

    while(FLASH -> SR & FLASH_SR_BSY);     // Wait for Flash to be ready

    FLASH -> CR |= FLASH_CR_SER;           // Set Sector Erase mode

    // Select the given sector
    FLASH -> CR |= (sector >> 3 & 1) ? FLASH_CR_SNB_3 : 0;
    FLASH -> CR |= (sector >> 2 & 1) ? FLASH_CR_SNB_2 : 0;
    FLASH -> CR |= (sector >> 1 & 1) ? FLASH_CR_SNB_1 : 0;
    FLASH -> CR |= (sector >> 0 & 1) ? FLASH_CR_SNB_0 : 0;

    FLASH -> CR |= FLASH_CR_STRT;          // Start Erase

    // Wait for Erase completion
    while(FLASH -> SR & FLASH_SR_BSY);
    
}
