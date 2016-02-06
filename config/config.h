#ifndef __CONFIG_H__
#define __CONFIG_G__

#define FIRMWARE_MAJOR_VERSION   0
#define FIRMWARE_MINOR_VERSION   0
#define FIRMWARE_REVISOIN  1

// //COMMS USART
// #define COMMS_USART        USART1
// #define COMMS_BAUDRATE     115200
// #define COMMS_IRQ          USART1_IRQn
// #define COMMS_ISR          USART1_IRQHandler
// #define COMMS_RX_FIFO_SIZE 64
// #define COMMS_TX_FIFO_SIZE 64

// //I2C
// #define IGVC_I2C        I2C2
// #define I2C_CLOCK_SPEED 100000

//Callback Timer
#define CALLBACK_TIMER  TIM9
#define CALLBACK_FREQUENCY 1000 //1 ms
#define TIMER_FREQUENCY 1000000
#define TIMER_MAX_VALUE    0xFFFF
#define MAX_NUM_CALLBACK_TIMERS  10
#define CALLBACK_TIMER_IRQ TIM1_BRK_TIM9_IRQn
#define CALLBACK_TIMER_ISR TIM1_BRK_TIM9_IRQHandler


// //Olympus GPIO
// #define GPIO_DEBUG_1    PORTA, 4
// #define GPIO_DEBUG_2    PORTA, 5
// #define GPIO_DEBUG_3    PORTA, 7
// #define GPIO_DEBUG_4    GPIOB, 3
// #define GPIO_DEBUG_5    GPIOB, 4
// #define GPIO_DEBUG_6    GPIOB, 5
// #define GPIO_DEBUG_7    GPIOC, 4
// #define GPIO_DEBUG_8    GPIOC, 5
// #define GPIO_DEBUG_9    GPIOC, 7
// #define GPIO_DEBUG_10   GPIOC, 8
// #define GPIOD_DEBUG_11  GPIOD, 2

// #define GPIO_USB_ID     PORTA, 10
// #define GPIO_USB_DM     PORTA, 11
// #define GPIO_USB_DP     PORTA, 12
// #define USB_ID_AF       GPIO_AF10_OTG_FS
// #define USB_DM_AF       GPIO_AF10_OTG_FS
// #define USB_DP_AF       GPIO_AF10_OTG_FS

// #define GPIO_I2C_SCL    GPIOB, 10
// #define GPIO_I2C_SDA    GPIOB, 11
// #define I2C_SCL_AF      GPIO_AF4_I2C1
// #define I2C_SDA_AF      GPIO_AF4_I2C1

// //Zeus GPIO
// #define GPIO_SYSTEM_SHUTDOWN  PORTA, 9
// #define GPIO_ZEUS_GPIO  PORTC, 11

// //Hera GPIO
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

// //Hephaesus GPIO
#define GPIO_SPEED_PWM  PORTA, 6
#define GPIO_STEERING_DIRECTION  GPIOC, 6
#define GPIO_HEPHAESTUS_GPIO  PORTB, 15
#define STEERING_PWM_AF    GPIO_AF9_TIM13
#define STEERING_PWM_TIM   TIM9
#define STEERING_PWM_PERIOD 10000

// //Apollo GPIO
// #define GPIO_SPI_CS     GPIOA, 15
// #define GPIO_SHIFT_CLK  GPIOC, 10
// #define GPIO_SHIFT_DATA GPIOC, 12
// #define SPI_CS_AF       GPIO_AF6_SPI3
// #define SHIFT_CLK_AF    GPIO_AF6_SPI3
// #define SHIFT_DATA_AF   GPIO_AF6_SPI3


// //Dionysus GPIO
// #define GPIO_BOARD_ENABLE  GPIOB, 1
// #define GPIO_DIONYSUS_GPIO GPIOB, 13

// //Iris GPIO
// #define GPIO_USART_TX   GPIOB, 6
// #define GPIO_USART_RX   GPIOB, 7
// #define USART_TX_AF     GPIO_AF7_USART1
// #define USART_RX_AF     GPIO_AF7_USART1

// //Hermes GPIO
// #define GPIO_AUTOMAN    GPIOB, 12

// //Janus GPIO
// #define GPIO_FNR_REVERSE   GPIOC, 13
// #define GPIO_FNR_FORWARD   GPIOC, 14
// #define GPIO_COMPUTER_DRIVE   GPIOC, 15 

// Steering
#define STEERING_CONTROL_PERIOD 100


#endif