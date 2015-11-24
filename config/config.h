#ifndef __CONFIG_H__
#define __CONFIG_G__

#define FIRMWARE_MAJOR_VERSION   0
#define FIRMWARE_MINOR_VERSION   0
#define FIRMWARE_REVISOIN  1

//Olympus GPIO
#define GPIO_DEBUG_1    PORTA, 4
#define GPIO_DEBUG_2    PORTA, 5
#define GPIO_DEBUG_3    PORTA, 7
#define GPIO_DEBUG_4    GPIOB, 3
#define GPIO_DEBUG_5    GPIOB, 4
#define GPIO_DEBUG_6    GPIOB, 5
#define GPIO_DEBUG_7    GPIOC, 4
#define GPIO_DEBUG_8    GPIOC, 5
#define GPIO_DEBUG_9    GPIOC, 7
#define GPIO_DEBUG_10   GPIOC, 8
#define GPIOD_DEBUG_11  GPIOD, 2

#define GPIO_USB_ID     PORTA, 10
#define GPIO_USB_DM     PORTA, 11
#define GPIO_USB_DP     PORTA, 12

#define GPIO_I2C_SCL    GPIOB, 10
#define GPIO_I2C_SDA    GPIOB, 11

//Zeus GPIO
#define GPIO_SYSTEM_SHUTDOWN  PORTA, 9
#define GPIO_ZEUS_GPIO  PORTC, 11

//Hera GPIO
#define GPIO_TEMP_1     GPIOC, 1
#define GPIO_TEMP_2     GPIOC, 2
#define GPIO_TEMP_3     GPIOC, 3
#define GPIO_TEMP_4     PORTA, 0
#define GPIO_TEMP_5     PORTA, 1

#define GPIO_SONAR_1    PORTA, 8
#define GPIO_SONAR_2    PORTA, 2
#define GPIO_SONAR_3    GPIOB, 0
#define GPIO_SONAR_4   GPIOB, 9
#define GPIO_SONAR_5    PORTA, 3
#define GPIO_SONAR_6    GPIOC, 9

#define GPIO_HALLEFFECT_1  GPIOB, 8
#define GPIO_HALLEFFECT_2  GPIOB, 14

#define GPIO_STEERING_ENCODER GPIOC, 0

//Hephaesus GPIO
#define GPIO_SPEED_PWM  PORTA, 6
#define GPIO_STEERING_DIRECTION  GPIOC, 6
#define GPIO_HEPHAESTUS_GPIO  PORTB, 15

//Apollo GPIO
#define GPIO_SPI_CS     GPIOA, 15
#define GPIO_SHIFT_CLK  GPIOC, 10
#define GPIO_SHIFT_DATA GPIOC, 12

//Dionysus GPIO
#define GPIO_BOARD_ENABLE  GPIOB, 1
#define GPIO_DIONYSUS_GPIO GPIOB, 13

//Iris GPIO
#define GPIO_USART_TX   GPIOB, 6
#define GPIO_USART_RX   GPIOB, 7

//Hermes GPIO
#define GPIO_AUTOMAN    GPIOB, 12

//Janus GPIO
#define GPIO_FNR_REVERSE   GPIOC, 13
#define GPIO_FNR_FORWARD   GPIOC, 14
#define GPIO_COMPUTER_DRIVE   GPIOC, 15


#endif