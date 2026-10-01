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

/* 16-bit RGB565 Colors */
#define LCD_COLOR_BLACK             0x0000
#define LCD_COLOR_WHITE             0xFFFF
#define LCD_COLOR_RED               0xF800
#define LCD_COLOR_GREEN             0x07E0
#define LCD_COLOR_BLUE              0x001F
#define LCD_COLOR_YELLOW            0xFFE0
#define LCD_COLOR_CYAN              0x07FF
#define LCD_COLOR_MAGENTA           0xF81F
#define LCD_COLOR_GRAY              0x8410
#define LCD_COLOR_DARKGRAY          0x4208
#define LCD_COLOR_NAVY              0x000F
#define LCD_COLOR_ORANGE            0xFD20

/**
 * @brief Initialize the ST7789 FSMC LCD hardware, GPIOs, and mutex.
 */
void lcd_init(void);

/**
 * @brief Acquire the LCD mutex.
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
