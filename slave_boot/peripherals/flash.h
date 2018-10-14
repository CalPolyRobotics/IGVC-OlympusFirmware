#ifndef __FLASH_H_
#define __FLASH_H_

#include <stdint.h>

#define PAGE_SIZE      1024u
#define APP_START_PAGE 5u
#define BOOT_NUM_PAGES 5u
#define APP_NUM_PAGES  27u
#define APP_MAX_SIZE   (APP_NUM_PAGES * PAGE_SIZE)
#define APP_START_ADDR ((uint32_t*)0x8001400u)

void writeInit(uint32_t progSize);
void writeFlash(uint16_t* addr, uint16_t data);
void writeComplete();
void jumpToApp(uint32_t* data);

#endif
