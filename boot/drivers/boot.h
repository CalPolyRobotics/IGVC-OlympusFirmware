#ifndef __BOOT_H__
#define __BOOT_H__

void runBootFSM(uint8_t data);
void jumpToApp(uint32_t* address);

#endif
