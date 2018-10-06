#ifndef __FLASH_H__
#define __FLASH_H__

#include "stdint.h"

#define KEY_1            0x45670123
#define KEY_2            0xCDEF89AB

extern uint32_t _START_OF_FLASH, _START_OF_BOOT;

#define BOOT_BASE_PTR      ((uint32_t*)&_START_OF_BOOT)

#define USER_APP_BASE_ADDR ((uint32_t)&_START_OF_FLASH)
#define USER_APP_BASE_PTR  ((uint32_t*)USER_APP_BASE_ADDR)

#define PROG_START_SECTOR (5u)

/** 128 kB Sector Size **/
#define BOOT_SECTOR_SIZE ((uint32_t)131072)

/** Program Structure **
 * Sector 0 - 4 : Bootloader (0x08000000 - 0x0801FFFF)
 * Sector 5 - 6 : Program    (0x08020000 - 0x0805FFFF)
 */

void writeInit(uint32_t progsize);
void writeFlash(uint32_t*, uint32_t);
void completeWrite();

#endif
