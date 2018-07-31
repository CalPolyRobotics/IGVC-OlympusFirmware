#ifndef __BOOT_H__
#define __BOOT_H__

typedef union uint32{
    uint32_t u32;
    uint16_t u16[2];
    uint8_t  u8[4];
}uint32_u;

void runBootFSM(uint32_t data);
void jumpToApp(uint32_t* address);

#endif
