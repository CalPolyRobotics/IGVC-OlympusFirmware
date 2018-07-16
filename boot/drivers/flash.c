#include "stm32f205xx.h"
#include "flash.h"

static void eraseSector(uint8_t sector);

void writeInit(uint32_t progsize)
{
    // Unlock Flash for Writing 
    FLASH -> KEYR = KEY_1;
    FLASH -> KEYR = KEY_2;
    
    uint32_t sector;

    /** Calculate the minimum number of sectors to erase **/
    uint32_t numSectors = (progsize / BOOT_SECTOR_SIZE) + (progsize % BOOT_SECTOR_SIZE ? 1 : 0);
    for(sector = 0; sector < numSectors; sector++)
    {
        eraseSector(PROG_START_SECTOR + sector);
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

void jumpToApp(uint32_t* address)
{
    // Ensure priveledge mode
    // Disable Interrupts
    // Disable Interrupting Peripherals
    // Clear pending interrupt requests
    // Disable systick and clear pending exception bit
    // Disable individual fault handlers
    // Active MSP if core is using PSP
    // Load VTOR Address & make sure it matches alignment

    // Set SP to first entry in the vector table
    __set_MSP(address[0]);

    // Set PC to reset value in vector table
    ((void (*)(void))address[1])();
}

static void eraseSector(uint8_t sector){
    while(FLASH -> SR & FLASH_SR_BSY);     // Wait for Flash to be ready

    FLASH -> CR = FLASH_CR_SER;           // Set Sector Erase mode

    // Select the given sector
    FLASH -> CR |= (sector >> 3 & 1) ? FLASH_CR_SNB_3 : 0;
    FLASH -> CR |= (sector >> 2 & 1) ? FLASH_CR_SNB_2 : 0;
    FLASH -> CR |= (sector >> 1 & 1) ? FLASH_CR_SNB_1 : 0;
    FLASH -> CR |= (sector & 1) ? FLASH_CR_SNB_0 : 0;

    FLASH -> CR |= FLASH_CR_STRT;          // Start Erase

    // Wait for Erase completion
    while(FLASH -> SR & FLASH_SR_BSY);
}
