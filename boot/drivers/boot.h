#ifndef __BOOT_H__
#define __BOOT_H__

#if defined(STM32F207xx)
   #define BOOT_GPIO_PORT GPIOC
   #define BOOT_GPIO_PIN  GPIO_PIN_13
#elif defined(STM32F205xx)
   #define BOOT_GPIO_PORT GPIOB
   #define BOOT_GPIO_PIN  GPIO_PIN_15
#endif

typedef union uint32{
    uint32_t u32;
    uint16_t u16[2];
    uint8_t  u8[4];
}uint32_u;

void runBootFSM(uint32_t data);
void jumpToApp(uint32_t* address);

#endif
