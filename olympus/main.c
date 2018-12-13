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

static void SystemClock_Config(void);

void SystemClock_Config(void);

Timer_Return led6Toggle(void* dummy)
{
    HAL_GPIO_TogglePin(GPIO_DEBUG_6);

    return CONTINUE_TIMER;
}

Timer_Return updateSteerDataLink(void* dummy)
{
    if(updateHeraSteer() != COMMS_OK)
    {
        setSevenSeg(HERA_STEER_FAIL);
        printf("UpdateHeraSteer Failed\r\n");
    }

    if(updateHephaestusSteerPot(heraData.steer) != COMMS_OK)
    {
        setSevenSeg(HEPHAESTUS_STEER_FAIL);
        printf("UpdateHephaestusSteerPot Failed\r\n");
    }

    return CONTINUE_TIMER;
}

void zeusDataCallback(void* dummy, uint8_t* data, uint32_t len, I2CStatus status)
{
    if (status == I2C_ACK)
    {
        printf("Zeus Data: ");
        while (len--)
        {
            printf("%X ", *data++);
        }
        printf("\r\n");
    } else if (status == I2C_NACK) {
        printf("Zeus NACK\r\n");
    } else {
        printf("Zeus ERR\r\n");
    }
}

struct netif gnetif;
>>>>>>> Implement initial ethernet code for revision 3

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
    MX_USB_OTG_FS_USB_Init();

    lwip_init();
    Netif_Config();
    tcp_echoserver_init();
    User_notification(&gnetif);

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
        serviceUSBWrite();

        ethernetif_input(&gnetif);
        sys_check_timeouts();

        consoleProcessBytes();

        uint8_t dataIn;
        while (doubleBuffer_read(&usbReceiveBuffer, &dataIn, 1))
        {
            runCommsFSM((char)dataIn);
        }
    }
}


/** System Clock Configuration
*/
static void SystemClock_Config(void)
{

#ifdef DEV
    RCC_OscInitTypeDef RCC_OscInitStruct;
    RCC_ClkInitTypeDef RCC_ClkInitStruct;

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = 16;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLM = 13;
    RCC_OscInitStruct.PLL.PLLN = 195;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 5;
    HAL_RCC_OscConfig(&RCC_OscInitStruct);

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK |
                            RCC_CLOCKTYPE_SYSCLK |
                            RCC_CLOCKTYPE_PCLK1|
                            RCC_CLOCKTYPE_PCLK2;

    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

    HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3);

    HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

    HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
#else
    RCC_OscInitTypeDef RCC_OscInitStruct;
    RCC_ClkInitTypeDef RCC_ClkInitStruct;

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
    RCC_OscInitStruct.LSIState = RCC_LSI_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 16;
    RCC_OscInitStruct.PLL.PLLN = 192;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 5;
    HAL_RCC_OscConfig(&RCC_OscInitStruct);

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_PCLK1
                              |RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
    HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3);

    HAL_RCC_EnableCSS();

    HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
#endif
}
