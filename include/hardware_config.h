#ifndef HARDWARE_CONFIG_H
#define HARDWARE_CONFIG_H

#ifndef NATIVE_TEST
#include "stm32f4xx_hal.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Buttons Configuration (Pull-up resistor setup, active LOW) */
#define BTN1_PIN                    GPIO_PIN_0
#define BTN1_PORT                   GPIOB

#define BTN2_PIN                    GPIO_PIN_1
#define BTN2_PORT                   GPIOB

#define BTN3_PIN                    GPIO_PIN_2
#define BTN3_PORT                   GPIOB

#define BTN4_PIN                    GPIO_PIN_3
#define BTN4_PORT                   GPIOB

/* User button (PA0 on STM32F407 boards, active HIGH with pull-down) */
#define USER_BTN_PIN                GPIO_PIN_0
#define USER_BTN_PORT               GPIOA

/* RGB LED Configuration (Active HIGH by default)
 * Moved Red to PF11 and Blue to PF12 to avoid conflict with LCD Backlight (PF9).
 */
#define LED_RED_PIN                 GPIO_PIN_11
#define LED_RED_PORT                GPIOF

#define LED_GREEN_PIN               GPIO_PIN_14
#define LED_GREEN_PORT              GPIOF

#define LED_BLUE_PIN                GPIO_PIN_12
#define LED_BLUE_PORT               GPIOF

/* Audio Buzzer / Speaker Pin (PB8 / TIM4_CH3) */
#define AUDIO_PIN                   GPIO_PIN_8
#define AUDIO_PORT                  GPIOB

/* Potentiometer ADC Pin (PA1 / ADC1 Channel 1) */
#define POT_ADC_PIN                 GPIO_PIN_1
#define POT_ADC_PORT                GPIOA
#define POT_ADC_CHANNEL             ADC_CHANNEL_1

/* ST7789 FSMC 8080 LCD Non-FSMC and Control Pins */
#define LCD_RST_PIN                 GPIO_PIN_3
#define LCD_RST_PORT                GPIOD

#define LCD_BL_PIN                  GPIO_PIN_9
#define LCD_BL_PORT                 GPIOF

#define LCD_CS_PIN                  GPIO_PIN_10
#define LCD_CS_PORT                 GPIOG

#define LCD_RS_PIN                  GPIO_PIN_13
#define LCD_RS_PORT                 GPIOD

#define LCD_WR_PIN                  GPIO_PIN_5
#define LCD_WR_PORT                 GPIOD

#define LCD_RD_PIN                  GPIO_PIN_4
#define LCD_RD_PORT                 GPIOD

/* UART Configuration */
#define UART_BAUDRATE               115200

#ifdef __cplusplus
}
#endif

#endif /* HARDWARE_CONFIG_H */
