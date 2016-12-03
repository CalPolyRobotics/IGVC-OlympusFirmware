#ifndef __FLASH_H__
#define __FLASH_H__

#define FLASH_PROG_START   (uint32_t*)0x080A0000
#define KEY_1              0x45670123
#define KEY_2              0xCDEF89AB

void writeInit()
void writeFlash(uint32_t*, uint32_t)
void completeWrite()
void jumpToApp()
void eraseSecotr(uint8_t)

#endif
