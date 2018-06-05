#include "stm32f205xx.h"
#include "flash.h"

static void eraseSector(uint8_t sector);

void writeInit()
{

    // Unlock Flash for Writing 
    FLASH -> KEYR = KEY_1;
    FLASH -> KEYR = KEY_2;
    
    uint32_t sector;
    for(sector = PROG_START_SECTOR; sector < PROG_NUM_SECTORS; sector++)
    {
        eraseSector(sector);
    }

    // Parallelism to 32 bit
    FLASH -> CR |= FLASH_CR_PSIZE_1;

    // Set Flash to write mode
    FLASH -> CR |= FLASH_CR_PG;
}

void writeFlash(uint32_t* loc, uint32_t data)
{

    *loc = data;
    while(FLASH -> SR & FLASH_SR_BSY);
}

void completeWrite()
{
    FLASH -> CR &= ~FLASH_CR_PG;
}

void jumpToApp(){

    // +4ul is to skip SP at start of bin and + 1ul is to put in thumb mode
    void (* const jumpFunction)(void) = (void (*)(void))(PROG_START + 4ul + 1ul);
    //void (* const jumpFunction)(void) = *(FLASH_PROG_START + 4ul) + 1ul;

    // Set the Main stack pointer to the first 4 bytes at FLASH_PROG_START
    __set_MSP((uint32_t)*PROG_START);

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
