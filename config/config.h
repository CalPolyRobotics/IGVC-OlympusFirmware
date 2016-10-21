#ifndef __CONFIG_H__
#define __CONFIG_G__

#include "main.h"

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
#define MAX_NUM_CALLBACK_TIMERS  10
#define CALLBACK_TIMER_IRQ TIM1_BRK_TIM9_IRQn
#define CALLBACK_TIMER_ISR TIM1_BRK_TIM9_IRQHandler

//USB Input Buffer
#define USB_RECEIVE_BUFFER_SIZE 128

#define USB_SEND_BUFFER_SIZE 64
#define USB_SEND_BUFFER_NUM  4

#define USB_SEND_BUFFER_SIZE 64
#define USB_SEND_BUFFER_NUM  4

//Olympus GPIO
#define NUM_DEBUG_PINS  12

#define GPIO_DEBUG_1    GPIOA, GPIO_PIN_4
#define GPIO_DEBUG_2    GPIOA, GPIO_PIN_5
#define GPIO_DEBUG_3    GPIOA, GPIO_PIN_7
#define GPIO_DEBUG_4    GPIOB, GPIO_PIN_3
#define GPIO_DEBUG_5    GPIOB, GPIO_PIN_4
#define GPIO_DEBUG_6    GPIOB, GPIO_PIN_5
#define GPIO_DEBUG_7    GPIOC, GPIO_PIN_4
#define GPIO_DEBUG_8    GPIOC, GPIO_PIN_5
#define GPIO_DEBUG_9    GPIOC, GPIO_PIN_7
#define GPIO_DEBUG_10   GPIOC, GPIO_PIN_8
#define GPIO_DEBUG_11   GPIOD, GPIO_PIN_2
#define GPIO_DEBUG_12   GPIOH, GPIO_PIN_1

//Zeus GPIO
#define GPIO_BOARD_KILL_PIN  GPIO_PIN_9
#define GPIO_BOARD_KILL_PORT GPIOA

#define ZEUS_ADC_I2C_ADDR    (0x33 << 1)

//Hera GPIO
// #define GPIO_TEMP_1     GPIOC, 1
// #define GPIO_TEMP_2     GPIOC, 2
// #define GPIO_TEMP_3     GPIOC, 3
// #define GPIO_TEMP_4     PORTA, 0
// #define GPIO_TEMP_5     PORTA, 1

// #define GPIO_SONAR_1    PORTA, 8
// #define GPIO_SONAR_2    PORTA, 2
// #define GPIO_SONAR_3    GPIOB, 0
// #define GPIO_SONAR_4    GPIOB, 9
// #define GPIO_SONAR_5    PORTA, 3
// #define GPIO_SONAR_6    GPIOC, 9
// #define SONAR_1_AF      GPIO_AF1_TIM1
// #define SONAR_2_AF      GPIO_AF1_TIM2
// #define SONAR_3_AF      GPIO_AF2_TIM3
// #define SONAR_4_AF      GPIO_AF2_TIM4
// #define SONAR_5_AF      GPIO_AF2_TIM5
// #define SONAR_6_AF      GPIO_AF3_TIM8

// #define GPIO_HALLEFFECT_1  GPIOB, 8
// #define GPIO_HALLEFFECT_2  GPIOB, 14
// #define HALLEFFECT_1_AF    GPIO_AF3_TIM10
// #define HALLEFFECT_2_AF    GPIO_AF9_TIM12

// #define GPIO_STEERING_ENCODER GPIOC, 0

//Hephaesus GPIO
#define GPIO_STEER_LEFT_PIN  GPIO_PIN_6
#define GPIO_STEER_LEFT_PORT GPIOC
#define GPIO_STEER_LEFT GPIO_STEER_LEFT_PORT, GPIO_STEER_LEFT_PIN

#define GPIO_STEER_RIGHT_PIN  GPIO_PIN_6
#define GPIO_STEER_RIGHT_PORT GPIOA
#define GPIO_STEER_RIGHT GPIO_STEER_RIGHT_PORT, GPIO_STEER_RIGHT_PIN

#define STEERING_CONTROL_PERIOD 100
#define STEERING_CONTROL_DEADZONE 5

//Apollo GPIO

//Dionysus GPIO

//Iris GPIO
#define GPIO_USART_TX   GPIO_PIN_6
#define GPIO_USART_RX   GPIO_PIN_7
#define GPIO_USART_PORT GPIOB
#define GPIO_USART_AF   GPIO_AF7_USART1;

#define IRIS_SEG_I2C_ADDR  (0x21)

#define COMMS_USART        USART1
#define COMMS_BAUDRATE     115200
#define COMMS_TX_BUFFER_SIZE (2048)
#define COMMS_RX_BUFFER_SIZE (1024)
#define COMMS_TX_NUM_BUFFERS (4)

#define COMMS_TX_DMA       DMA2
#define COMMS_RX_DMA       DMA2
#define COMMS_TX_DMA_STREAM DMA2_Stream7
#define COMMS_RX_DMA_STREAM DMA2_Stream5
#define COMMS_TX_DMA_CHANNEL DMA_CHANNEL_4
#define COMMS_RX_DMA_CHANNEL DMA_CHANNEL_4
#define COMMS_TX_DMA_IRQ   DMA2_Stream7_IRQn
#define COMMS_TX_DMA_ISR   DMA2_Stream7_IRQHandler

//Hermes GPIO
#define HERMES_SPEED_DAC_I2C_ADDR (0x62)

#define GPIO_AUTO_THROTTLE_ENABLE_PIN   GPIO_PIN_12
#define GPIO_AUTO_THROTTLE_ENABLE_PORT  GPIOB
#define GPIO_AUTO_THROTTLE_ENABLE GPIO_AUTO_THROTTLE_ENABLE_PORT, GPIO_AUTO_THROTTLE_ENABLE_PIN

#define SPEED_DAC_INCR_PERIOD   (201)
#define SPEED_DAC_INCR      (5)

//Janus GPIO
#define GPIO_FNR_DIRECTION_PIN  GPIO_PIN_13
#define GPIO_FNR_DIRECTION_PORT GPIOC
#define GPIO_FNR_ENABLE_PIN     GPIO_PIN_14
#define GPIO_FNR_ENABLE_PORT    GPIOC

#define GPIO_COMPUTER_DRIVE_PIN GPIO_PIN_15
#define GPIO_COMPUTER_DRIVE_PORT GPIOC

#define SEVEN_SEG_UPDATE_PERIOD (10)




#endif
