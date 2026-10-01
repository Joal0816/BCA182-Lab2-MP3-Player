#ifndef LCD_DRIVER_H
#define LCD_DRIVER_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* LCD Resolution */
#define LCD_WIDTH                   240
#define LCD_HEIGHT                  240

/* Standard 16-bit RGB565 Colors (RT-Spark BSP matching) */
#define WHITE                       0xFFFF
#define BLACK                       0x0000
#define BLUE                        0x001F
#define BRED                        0XF81F
#define GRED                        0XFFE0
#define GBLUE                       0X07FF
#define RED                         0xF800
#define MAGENTA                     0xF81F
#define GREEN                       0x07E0
#define CYAN                        0x07FF
#define YELLOW                      0xFFE0
#define BROWN                       0XBC40
#define BRRED                       0XFC07
#define GRAY                        0x8410
#define DARKBLUE                    0X01CF
#define LIGHTBLUE                   0X7D7C
#define GRAYBLUE                    0X5458
#define LIGHTGREEN                  0X841F
#define LGRAY                       0XC618
#define LGRAYBLUE                   0XA651
#define LBBLUE                      0X2B12

/* Aliases for LCD_COLOR_* definitions */
#define LCD_COLOR_BLACK             BLACK
#define LCD_COLOR_WHITE             WHITE
#define LCD_COLOR_RED               RED
#define LCD_COLOR_GREEN             GREEN
#define LCD_COLOR_BLUE              BLUE
#define LCD_COLOR_YELLOW            YELLOW
#define LCD_COLOR_CYAN              CYAN
#define LCD_COLOR_MAGENTA           MAGENTA
#define LCD_COLOR_GRAY              GRAY
#define LCD_COLOR_DARKGRAY          0x4208
#define LCD_COLOR_NAVY              DARKBLUE
#define LCD_COLOR_ORANGE            0xFD20

/* LCD Device Descriptor Structure (RT-Spark BSP drv_lcd.h) */
typedef struct
{
    uint16_t width;      /* LCD width */
    uint16_t height;     /* LCD height */
    uint16_t id;         /* LCD ID */
    uint8_t  dir;        /* 0: vertical, 1: horizontal */
    uint16_t wramcmd;    /* Write GRAM command (0x2C) */
    uint16_t setxcmd;    /* Set X command (0x2A) */
    uint16_t setycmd;    /* Set Y command (0x2B) */
} _lcd_dev;

extern _lcd_dev lcddev;

#ifndef NATIVE_TEST
#include "stm32f4xx_hal.h"

/* FSMC Bank 1 NOR/SRAM 3 addresses:
 * Bit 18 (PD13 / FSMC_A18) connects to ST7789 DCX (RS).
 * In 8-bit mode on STM32 FSMC:
 * Address 0x6803FFFE has bit 18 = 0 (Command REG)
 * Address 0x68040000 has bit 18 = 1 (Data RAM)
 */
typedef struct
{
    __IO uint8_t  _u8_REG;
    __IO uint8_t  RESERVED;
    __IO uint8_t  _u8_RAM;
    __IO uint16_t _u16_RAM;
} LCD_CONTROLLER_TypeDef;

#define LCD_BASE ((uint32_t)(0x68000000 | 0x0003FFFE))
#define LCD      ((LCD_CONTROLLER_TypeDef *)LCD_BASE)
#endif

/**
 * @brief Initialize the ST7789 FSMC LCD hardware, GPIOs, PWM backlight, and mutex.
 */
void lcd_init(void);

/**
 * @brief Set display direction (0: vertical, 1: horizontal).
 */
void lcd_display_dir(uint8_t dir);
void LCD_Display_Dir(uint8_t dir);

/**
 * @brief Initialize TIM14 PWM backlight control on PF9.
 */
void lcd_backlight_init(void);

/**
 * @brief Set backlight brightness percentage [0-100].
 */
void lcd_backlight_set(uint8_t value);
void LCD_BackLightSet(uint8_t value);

/**
 * @brief Acquire the LCD mutex (reentrant recursive mutex).
 * @param timeout_ms Maximum time to wait in milliseconds.
 * @return true if acquired, false otherwise.
 */
bool lcd_lock(uint32_t timeout_ms);

/**
 * @brief Release the LCD mutex.
 */
void lcd_unlock(void);

/**
 * @brief Set display window boundaries.
 */
void lcd_set_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);

/**
 * @brief Set address window directly (BSP naming).
 */
void lcd_address_set(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);

/**
 * @brief Write 16-bit color word.
 */
void lcd_write_half_word(uint16_t da);

/**
 * @brief Fill a rectangular region with color.
 */
void lcd_fill_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);

/**
 * @brief Clear LCD screen with specified color.
 */
void lcd_clear_screen(uint16_t color);

/**
 * @brief Clear LCD screen to black.
 */
void lcd_clear(void);

/**
 * @brief Draw a single pixel with specified color.
 */
void lcd_draw_point(uint16_t x, uint16_t y, uint16_t color);

/**
 * @brief Draw a line between (x1, y1) and (x2, y2).
 */
void lcd_draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);

/**
 * @brief Draw a single character using the 8x16 font.
 */
void lcd_draw_char(uint16_t x, uint16_t y, char c, uint16_t fg_color, uint16_t bg_color);
void lcd_show_char(uint16_t x, uint16_t y, char c, uint16_t fg_color, uint16_t bg_color);

/**
 * @brief Draw a string using the 8x16 font.
 */
void lcd_draw_string(uint16_t x, uint16_t y, const char* str, uint16_t fg_color, uint16_t bg_color);
void lcd_show_string(uint16_t x, uint16_t y, const char* str, uint16_t fg_color, uint16_t bg_color);

/**
 * @brief Backward compatibility for character LCD line updates.
 */
void lcd_set_cursor(uint8_t col, uint8_t row);
void lcd_print(const char* str);
void lcd_display_lines(const char* line1, const char* line2);

#ifdef __cplusplus
}
#endif

#endif /* LCD_DRIVER_H */
