#include <stdio.h>
#include <stddef.h>

#include "stm32f2xx_hal.h"

#include "adc.h"
#include "dma.h"
#include "gpio.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "usb_otg.h"
#include "comms.h"

#include "console.h"
#include "dac.h"
#include "error.h"
#include "kill.h"
#include "led.h"
#include "pwradc.h"
#include "sevenSeg.h"
#include "sounds.h"
#include "speedDAC.h"
#include "status.h"

#include "config.h"
#include "buffer8.h"
#include "doubleBuffer.h"
#include "submoduleComms.h"
#include "timerCallback.h"
#include "sysclock.h"

#include "hephaestus.h"
#include "janus.h"

#include "lwip/opt.h"
#include "lwip/init.h"
#include "lwip/netif.h"
#include "lwip/timeouts.h"
#include "netif/etharp.h"
#include "eth.h"
#include "ethernetif.h"
#include "tcp_echoserver.h"
#include "app_ethernet.h"

#include <stddef.h>

struct netif gnetif;

int usingUSB;

int main(void)
{
    RCC->AHB1ENR |= 0xFFFFFFFF;
    RCC->AHB2ENR |= 0xFFFFFFFF;
    RCC->AHB3ENR |= 0xFFFFFFFF;
    RCC->APB1ENR |= 0xFFFFFFFF;
    RCC->APB2ENR |= 0xFFFFFFFF;

    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* Configure the system clock */
    SystemClock_Config();
    initIGVCCallbackTimer();

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_DMA_Init();
    MX_SPI3_Init();

    commsUsartInit();

    usingUSB = HAL_GPIO_ReadPin(COM_SEL_PRT, COM_SEL_PIN) == GPIO_PIN_SET;

    if(usingUSB)
    {
        MX_USB_OTG_FS_USB_Init();
    }
    else
    {
        ethernetif_init(&gnetif);

        lwip_init();
        Netif_Config();
        tcp_echoserver_init();
        User_notification(&gnetif);
    }

    initSpeedDAC();

    setSevenSeg("42");
    adc_init();

    checkAllSubmodules();

    runStartupSong();

    addCallbackTimer(1000, heartbeat, NULL);
    addCallbackTimer(1000, updateJanus, NULL);

    printf("Hello.\r\n");
    while(1)
    {
        serviceTxDma();
        serviceCallbackTimer();

        consoleProcessBytes();

        uint8_t dataIn;
        if(usingUSB)
        {
            serviceUSBWrite();

            while (doubleBuffer_read(&usbReceiveBuffer, &dataIn, 1))
            {
                runCommsFSM((char)dataIn, NULL);
            }
        }
        else
        {
            ethernetif_input(&gnetif);
            sys_check_timeouts();
        }
    }
}
