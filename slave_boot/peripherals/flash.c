#include "stm32f0xx.h"
#include "flash.h"

static void erasePage(uint8_t page);
static void checkEOP();

void writeInit(uint32_t progsize)
{
    /* Unlock FLASH registers */
    FLASH->KEYR = FLASH_KEY1;
    FLASH->KEYR = FLASH_KEY2;

    uint32_t page;

    /* Calculate the minimum number of pages to erase */
    uint32_t numPages = (progsize / PAGE_SIZE) + (progsize % PAGE_SIZE ? 1 : 0);
    for(page = 0; page < numPages; page++)
    {
        erasePage(APP_START_PAGE + page);
    }

}

void writeFlash(uint16_t* addr, uint16_t data)
{
    /* Set Flash to write mode */
    FLASH->CR |= FLASH_CR_PG;

    /* Flash must be written 16 bits at a time */
    *addr = data;
    while(FLASH->SR & FLASH_SR_BSY);

    checkEOP();

    FLASH->CR &= ~FLASH_CR_PG;
}

void writeComplete()
{
    /* Disable flash programming */
    FLASH->CR &= ~FLASH_CR_PG;
}

void jumpToApp(uint32_t* address)
{
    // Disable Interrupts
    NVIC->ICER[0] = 0xFFFFFFFF;

    // Clear pending interrupt requests
    NVIC->ICPR[0] = 0xFFFFFFFF;

    // Disable systick and clear pending exception bit
    SysTick->CTRL = 0;
    SCB->ICSR |= SCB_ICSR_PENDSTCLR_Msk;

    // Set SP to first entry in the vector table
    __set_MSP(address[0]);

    // Set PC to reset value in vector table
    ((void (*)(void))address[1])();
}

static void erasePage(uint8_t page){
    while(FLASH -> SR & FLASH_SR_BSY);

    /* Enable page erase */
    FLASH->CR = FLASH_CR_PER;

    /* Select page and start erase */
    FLASH->AR = FLASH_BASE + (page * PAGE_SIZE);
    FLASH->CR |= FLASH_CR_STRT;

    while(FLASH -> SR & FLASH_SR_BSY);
    checkEOP();

    /* Disable page erase */
    FLASH->CR &= ~FLASH_CR_PER;
}

static void checkEOP(){
    if (FLASH->SR & FLASH_SR_EOP) /* (5) */
    {
        /* Check and clear EOP - TODO handle else case */
        FLASH->SR = FLASH_SR_EOP; /* (6)*/
    }
}
