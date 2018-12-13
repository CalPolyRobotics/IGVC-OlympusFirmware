#ifndef __CONFIG_H__
#define __CONFIG_G__

#define FIRMWARE_MAJOR_VERSION   1
#define FIRMWARE_MINOR_VERSION   1
#define FIRMWARE_REVISION  0

#define AHB_CLK_FREQ 120000000UL
#define APB1_CLK_FREQ 30000000UL
#define APB1_TMR_CLK_FREQ 60000000UL
#define APB2_CLK_FREQ 60000000UL
#define APB2_TMR_CLK_FREQ 120000000UL

//Callback Timer
#define CALLBACK_TIMER  TIM9
#define CALLBACK_FREQUENCY 1000
#define TIMER_FREQUENCY 100000
#define TIMER_MAX_VALUE    0xFFFF
#define MAX_NUM_CALLBACK_TIMERS  32
#define CALLBACK_TIMER_IRQ TIM1_BRK_TIM9_IRQn
#define CALLBACK_TIMER_ISR TIM1_BRK_TIM9_IRQHandler

//USB Input Buffer
#define USB_RECEIVE_BUFFER_SIZE 128

#define USB_SEND_BUFFER_SIZE 64
#define USB_SEND_BUFFER_NUM  4

#define USB_SEND_BUFFER_SIZE 64
#define USB_SEND_BUFFER_NUM  4

//Iris GPIO
#define GPIO_USART_TX   GPIO_PIN_6
#define GPIO_USART_RX   GPIO_PIN_7
#define GPIO_USART_PORT GPIOC
#define GPIO_USART_AF   GPIO_AF8_USART6;

#define COMMS_USART          USART6
#define COMMS_BAUDRATE       115200
#define COMMS_TX_BUFFER_SIZE (2048)
#define COMMS_RX_BUFFER_SIZE (1024)
#define COMMS_TX_NUM_BUFFERS (4)

#define COMMS_TX_DMA         DMA2
#define COMMS_RX_DMA         DMA2
#define COMMS_TX_DMA_STREAM  DMA2_Stream6
#define COMMS_RX_DMA_STREAM  DMA2_Stream1
#define COMMS_TX_DMA_CHANNEL DMA_CHANNEL_5
#define COMMS_RX_DMA_CHANNEL DMA_CHANNEL_5
#define COMMS_TX_DMA_IRQ     DMA2_Stream6_IRQn
#define COMMS_TX_DMA_ISR     DMA2_Stream6_IRQHandler

//Hermes GPIO
#define SPEED_DAC_INCR_PERIOD   (20)
#define SPEED_DAC_INCR          (10)

//LEDS
#define LED_1_PRT GPIOB
#define LED_1_PIN GPIO_PIN_0
#define LED_2_PRT GPIOB
#define LED_2_PIN GPIO_PIN_1
#define LED_3_PRT GPIOA
#define LED_3_PIN GPIO_PIN_3
#define LED_4_PRT GPIOA
#define LED_4_PIN GPIO_PIN_4
#define LED_5_PRT GPIOA
#define LED_5_PIN GPIO_PIN_5
#define LED_6_PRT GPIOA
#define LED_6_PIN GPIO_PIN_6

#define LED_CLOCKS_ENABLE() \
    do{ \
        __GPIOA_CLK_ENABLE(); \
        __GPIOB_CLK_ENABLE(); \
    }while(0)

//USB
#define USB_PORT GPIOA
#define USB_ID_PIN GPIO_PIN_10
#define USB_DM_PIN GPIO_PIN_11
#define USB_DP_PIN GPIO_PIN_12

//SPI PINS
#define SPI3_PRT GPIOB
#define SPI3_SCK  GPIO_PIN_3
#define SPI3_MISO GPIO_PIN_4
#define SPI3_MOSI GPIO_PIN_5

#define APOL_SS_PRT GPIOB
#define APOL_SS_PIN GPIO_PIN_9
#define APOL_IT_PRT GPIOC
#define APOL_IT_PIN GPIO_PIN_15

#define HEPH_SS_PRT GPIOC
#define HEPH_SS_PIN GPIO_PIN_8
#define HEPH_IT_PRT GPIOB
#define HEPH_IT_PIN GPIO_PIN_14

#define HERM_SS_PRT GPIOB
#define HERM_SS_PIN GPIO_PIN_6
#define HERM_IT_PRT GPIOB
#define HERM_IT_PIN GPIO_PIN_7

#define JANU_SS_PRT GPIOC
#define JANU_SS_PIN GPIO_PIN_13
#define JANU_IT_PRT GPIOC
#define JANU_IT_PIN GPIO_PIN_14

#define ZEUS_SS_PRT GPIOA
#define ZEUS_SS_PIN GPIO_PIN_8
#define ZEUS_IT_PRT GPIOA
#define ZEUS_IT_PIN GPIO_PIN_9

#define SPI_CLOCKS_ENABLE() \
    do{ \
        __SPI3_CLK_ENABLE(); \
        __GPIOA_CLK_ENABLE(); \
        __GPIOB_CLK_ENABLE(); \
        __GPIOC_CLK_ENABLE(); \
    }while(0)

//Error Codes
#define ERROR_HARD_FAULT        "FF"
#define ERROR_WATCH_DOG         "FE"
#define APOLLO_STATUS_FAIL      "FD"
#define HEPHAESTUS_STATUS_FAIL  "FC"
#define HERA_STATUS_FAIL        "FB"
#define JANUS_STATUS_FAIL       "FA"
#define HERA_STEER_FAIL         "F9"
#define HEPH_SET_STEER_FAIL     "F8"
#define HERA_SPEED_FAIL         "F7"
#define HERA_SONAR_FAIL         "F6"
#define HEPH_STEER_POT_FAIL     "F5"
#define HERMES_STATUS_FAIL      "F4"
#define ZEUS_STATUS_FAIL        "F3"

//Ethernet Configuration

/*Static IP ADDRESS: IP_ADDR0.IP_ADDR1.IP_ADDR2.IP_ADDR3 */
#define IP_ADDR0   (uint8_t) 192
#define IP_ADDR1   (uint8_t) 168
#define IP_ADDR2   (uint8_t) 0
#define IP_ADDR3   (uint8_t) 10
   
/*NETMASK*/
#define NETMASK_ADDR0   (uint8_t) 255
#define NETMASK_ADDR1   (uint8_t) 255
#define NETMASK_ADDR2   (uint8_t) 255
#define NETMASK_ADDR3   (uint8_t) 0

/*Gateway Address*/
#define GW_ADDR0   (uint8_t) 192
#define GW_ADDR1   (uint8_t) 168
#define GW_ADDR2   (uint8_t) 0
#define GW_ADDR3   (uint8_t) 1


#endif
