#include "lcd_driver.h"
#include "lcd_font.h"
#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef NATIVE_TEST
#include "stm32f4xx_hal.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "hardware_config.h"

static SRAM_HandleTypeDef hsram;
static SemaphoreHandle_t s_lcd_mutex = NULL;

/* FSMC Bank 1 NOR/SRAM 3 addresses:
 * Bit 18 (PD13 / FSMC_A18) connects to ST7789 DCX (RS).
 * In 8-bit mode on STM32 FSMC:
 * Address 0x6803FFFE has bit 18 = 0 (Command REG)
 * Address 0x68040000 has bit 18 = 1 (Data RAM)
 */
typedef struct
{
    __IO uint8_t _u8_REG;
    __IO uint8_t RESERVED;
    __IO uint8_t _u8_RAM;
    __IO uint16_t _u16_RAM;
} LCD_TypeDef;

#define LCD_BASE ((uint32_t)(0x68000000 | 0x0003FFFE))
#define LCD_DEV  ((LCD_TypeDef *)LCD_BASE)

static inline void LCD_WR_REG(uint8_t cmd) {
    LCD_DEV->_u8_REG = cmd;
}

static inline void LCD_WR_DATA8(uint8_t data) {
    LCD_DEV->_u8_RAM = data;
}

static inline void LCD_WR_DATA16(uint16_t data) {
    LCD_DEV->_u16_RAM = data;
}

void lcd_write_half_word(uint16_t da) {
    uint16_t data = (uint16_t)((da >> 8) | ((da & 0xFF) << 8));
    LCD_WR_DATA16(data);
}

static inline void lcd_write_cmd(uint8_t cmd) {
    LCD_WR_REG(cmd);
}

static inline void lcd_write_data8(uint8_t data) {
    LCD_WR_DATA8(data);
}

static inline void lcd_write_data16(uint16_t data) {
    LCD_DEV->_u8_RAM = (uint8_t)(data >> 8);
    LCD_DEV->_u8_RAM = (uint8_t)(data & 0xFF);
}

static void lcd_fsmc_init(void) {
    FSMC_NORSRAM_TimingTypeDef read_timing = {0};
    FSMC_NORSRAM_TimingTypeDef write_timing = {0};

    hsram.Instance = FSMC_NORSRAM_DEVICE;
    hsram.Extended = FSMC_NORSRAM_EXTENDED_DEVICE;

    read_timing.AddressSetupTime = 0x0F;
    read_timing.AddressHoldTime = 0x00;
    read_timing.DataSetupTime = 60;
    read_timing.BusTurnAroundDuration = 0x00;
    read_timing.CLKDivision = 0x00;
    read_timing.DataLatency = 0x00;
    read_timing.AccessMode = FSMC_ACCESS_MODE_A;

    write_timing.AddressSetupTime = 9;
    write_timing.AddressHoldTime = 0x00;
    write_timing.DataSetupTime = 8;
    write_timing.BusTurnAroundDuration = 0x00;
    write_timing.CLKDivision = 0x00;
    write_timing.DataLatency = 0x00;
    write_timing.AccessMode = FSMC_ACCESS_MODE_A;

    hsram.Init.NSBank = FSMC_NORSRAM_BANK3;
    hsram.Init.DataAddressMux = FSMC_DATA_ADDRESS_MUX_DISABLE;
    hsram.Init.MemoryType = FSMC_MEMORY_TYPE_SRAM;
    hsram.Init.MemoryDataWidth = FSMC_NORSRAM_MEM_BUS_WIDTH_8;
    hsram.Init.BurstAccessMode = FSMC_BURST_ACCESS_MODE_DISABLE;
    hsram.Init.WaitSignalPolarity = FSMC_WAIT_SIGNAL_POLARITY_LOW;
    hsram.Init.WrapMode = FSMC_WRAP_MODE_DISABLE;
    hsram.Init.WaitSignalActive = FSMC_WAIT_TIMING_BEFORE_WS;
    hsram.Init.WriteOperation = FSMC_WRITE_OPERATION_ENABLE;
    hsram.Init.WaitSignal = FSMC_WAIT_SIGNAL_DISABLE;
    hsram.Init.ExtendedMode = FSMC_EXTENDED_MODE_ENABLE;
    hsram.Init.AsynchronousWait = FSMC_ASYNCHRONOUS_WAIT_DISABLE;
    hsram.Init.WriteBurst = FSMC_WRITE_BURST_DISABLE;
    hsram.Init.PageSize = FSMC_PAGE_SIZE_NONE;

    HAL_SRAM_Init(&hsram, &read_timing, &write_timing);
}

void lcd_init(void) {
    if (!s_lcd_mutex) {
        s_lcd_mutex = xSemaphoreCreateMutex();
    }

    /* 1. Configure all FSMC and LCD GPIO pins and peripheral clocks */
    HAL_FSMC_MspInit();

    /* 2. Hardware reset sequence (RT-Spark BSP) */
    HAL_GPIO_WritePin(LCD_RST_PORT, LCD_RST_PIN, GPIO_PIN_RESET);
    HAL_Delay(100);
    HAL_GPIO_WritePin(LCD_RST_PORT, LCD_RST_PIN, GPIO_PIN_SET);
    HAL_Delay(100);

    /* 3. FSMC Peripheral Initialization */
    lcd_fsmc_init();
    HAL_Delay(100);

    /* 4. Ensure Backlight (PF9) is turned ON */
    HAL_GPIO_WritePin(LCD_BL_PORT, LCD_BL_PIN, GPIO_PIN_SET);

    /* ST7789 Initialization Sequence (RT-Spark BSP) */
    /* Memory Data Access Control */
    LCD_WR_REG(0x36);
    LCD_WR_DATA8(0x00);

    /* RGB 5-6-5-bit (16-bit format) */
    LCD_WR_REG(0x3A); 
    LCD_WR_DATA8(0x55);

    /* Porch Setting */
    LCD_WR_REG(0xB2);
    LCD_WR_DATA8(0x0C);
    LCD_WR_DATA8(0x0C);
    LCD_WR_DATA8(0x00);
    LCD_WR_DATA8(0x33);
    LCD_WR_DATA8(0x33);

    /* Gate Control */
    LCD_WR_REG(0xB7); 
    LCD_WR_DATA8(0x35); 

    /* VCOM Setting */
    LCD_WR_REG(0xBB);
    LCD_WR_DATA8(0x37);

    /* LCM Control */
    LCD_WR_REG(0xC0);
    LCD_WR_DATA8(0x2C);

    /* VDV and VRH Command Enable */
    LCD_WR_REG(0xC2);
    LCD_WR_DATA8(0x01);

    /* VRH Set */
    LCD_WR_REG(0xC3);
    LCD_WR_DATA8(0x12); 

    /* VDV Set */
    LCD_WR_REG(0xC4);
    LCD_WR_DATA8(0x20); 

    /* Frame Rate Control in Normal Mode */
    LCD_WR_REG(0xC6); 
    LCD_WR_DATA8(0x0F); 

    /* Power Control 1 */
    LCD_WR_REG(0xD0); 
    LCD_WR_DATA8(0xA4);
    LCD_WR_DATA8(0xA1);

    /* Positive Voltage Gamma Control */
    LCD_WR_REG(0xE0);
    LCD_WR_DATA8(0xD0);
    LCD_WR_DATA8(0x04);
    LCD_WR_DATA8(0x0D);
    LCD_WR_DATA8(0x11);
    LCD_WR_DATA8(0x13);
    LCD_WR_DATA8(0x2B);
    LCD_WR_DATA8(0x3F);
    LCD_WR_DATA8(0x54);
    LCD_WR_DATA8(0x4C);
    LCD_WR_DATA8(0x18);
    LCD_WR_DATA8(0x0D);
    LCD_WR_DATA8(0x0B);
    LCD_WR_DATA8(0x1F);
    LCD_WR_DATA8(0x23);

    /* Negative Voltage Gamma Control */
    LCD_WR_REG(0xE1);
    LCD_WR_DATA8(0xD0);
    LCD_WR_DATA8(0x04);
    LCD_WR_DATA8(0x0C);
    LCD_WR_DATA8(0x11);
    LCD_WR_DATA8(0x13);
    LCD_WR_DATA8(0x2C);
    LCD_WR_DATA8(0x3F);
    LCD_WR_DATA8(0x44);
    LCD_WR_DATA8(0x51);
    LCD_WR_DATA8(0x2F);
    LCD_WR_DATA8(0x1F);
    LCD_WR_DATA8(0x1F);
    LCD_WR_DATA8(0x20);
    LCD_WR_DATA8(0x23);

    /* Display Inversion On */
    LCD_WR_REG(0x21); 

    /* Sleep Out */
    LCD_WR_REG(0x11); 
    HAL_Delay(120);

    /* Display On */
    LCD_WR_REG(0x29); 
    HAL_Delay(100);

    lcd_clear();
}

bool lcd_lock(uint32_t timeout_ms) {
    if (xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED) {
        return true;
    }
    if (!s_lcd_mutex) {
        s_lcd_mutex = xSemaphoreCreateMutex();
    }
    TickType_t ticks = (timeout_ms == UINT32_MAX) ? portMAX_DELAY : pdMS_TO_TICKS(timeout_ms);
    return (xSemaphoreTake(s_lcd_mutex, ticks) == pdTRUE);
}

void lcd_unlock(void) {
    if (xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED) {
        return;
    }
    if (s_lcd_mutex) {
        xSemaphoreGive(s_lcd_mutex);
    }
}

void lcd_address_set(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
    LCD_WR_REG(0x2A);
    LCD_WR_DATA8((uint8_t)(x1 >> 8));
    LCD_WR_DATA8((uint8_t)(x1 & 0xFF));
    LCD_WR_DATA8((uint8_t)(x2 >> 8));
    LCD_WR_DATA8((uint8_t)(x2 & 0xFF));

    LCD_WR_REG(0x2B);
    LCD_WR_DATA8((uint8_t)(y1 >> 8));
    LCD_WR_DATA8((uint8_t)(y1 & 0xFF));
    LCD_WR_DATA8((uint8_t)(y2 >> 8));
    LCD_WR_DATA8((uint8_t)(y2 & 0xFF));

    LCD_WR_REG(0x2C);
}

void lcd_set_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    if (x0 >= LCD_WIDTH) x0 = LCD_WIDTH - 1;
    if (y0 >= LCD_HEIGHT) y0 = LCD_HEIGHT - 1;
    if (x1 >= LCD_WIDTH) x1 = LCD_WIDTH - 1;
    if (y1 >= LCD_HEIGHT) y1 = LCD_HEIGHT - 1;

    lcd_address_set(x0, y0, x1, y1);
}

void lcd_fill_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {
    if (x >= LCD_WIDTH || y >= LCD_HEIGHT || w == 0 || h == 0) return;
    if (x + w > LCD_WIDTH) w = LCD_WIDTH - x;
    if (y + h > LCD_HEIGHT) h = LCD_HEIGHT - y;

    uint8_t hi = (uint8_t)(color >> 8);
    uint8_t lo = (uint8_t)(color & 0xFF);

    lcd_address_set(x, y, x + w - 1, y + h - 1);
    uint32_t total = (uint32_t)w * h;
    for (uint32_t i = 0; i < total; i++) {
        LCD_DEV->_u8_RAM = hi;
        LCD_DEV->_u8_RAM = lo;
    }
}

void lcd_clear_screen(uint16_t color) {
    if (lcd_lock(100)) {
        uint8_t hi = (uint8_t)(color >> 8);
        uint8_t lo = (uint8_t)(color & 0xFF);

        lcd_address_set(0, 0, LCD_WIDTH - 1, LCD_HEIGHT - 1);
        uint32_t total = (uint32_t)LCD_WIDTH * LCD_HEIGHT;
        for (uint32_t i = 0; i < total; i++) {
            LCD_DEV->_u8_RAM = hi;
            LCD_DEV->_u8_RAM = lo;
        }
        lcd_unlock();
    }
}

void lcd_clear(void) {
    lcd_clear_screen(LCD_COLOR_BLACK);
}

void lcd_draw_point(uint16_t x, uint16_t y, uint16_t color) {
    if (x >= LCD_WIDTH || y >= LCD_HEIGHT) return;
    lcd_address_set(x, y, x, y);
    LCD_DEV->_u8_RAM = (uint8_t)(color >> 8);
    LCD_DEV->_u8_RAM = (uint8_t)(color & 0xFF);
}

void lcd_draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color) {
    if (lcd_lock(100)) {
        int dx = abs((int)x2 - (int)x1);
        int dy = abs((int)y2 - (int)y1);
        int sx = (x1 < x2) ? 1 : -1;
        int sy = (y1 < y2) ? 1 : -1;
        int err = ((dx > dy) ? dx : -dy) / 2;
        int e2;

        while (1) {
            lcd_draw_point(x1, y1, color);
            if (x1 == x2 && y1 == y2) break;
            e2 = err;
            if (e2 > -dx) { err -= dy; x1 += sx; }
            if (e2 < dy) { err += dx; y1 += sy; }
        }
        lcd_unlock();
    }
}

void lcd_show_char(uint16_t x, uint16_t y, char c, uint16_t fg_color, uint16_t bg_color) {
    if (x + 8 > LCD_WIDTH || y + 16 > LCD_HEIGHT) return;
    if (c < ' ' || c > '~') c = ' ';

    uint16_t font_offset = ((uint16_t)(c - ' ')) * 16;
    uint8_t fg_hi = (uint8_t)(fg_color >> 8);
    uint8_t fg_lo = (uint8_t)(fg_color & 0xFF);
    uint8_t bg_hi = (uint8_t)(bg_color >> 8);
    uint8_t bg_lo = (uint8_t)(bg_color & 0xFF);

    lcd_address_set(x, y, x + 7, y + 15);

    for (int r = 0; r < 16; r++) {
        uint8_t line = asc2_1608[font_offset + r];
        for (int b = 0; b < 8; b++) {
            if (line & (0x80 >> b)) {
                LCD_DEV->_u8_RAM = fg_hi;
                LCD_DEV->_u8_RAM = fg_lo;
            } else {
                LCD_DEV->_u8_RAM = bg_hi;
                LCD_DEV->_u8_RAM = bg_lo;
            }
        }
    }
}

void lcd_draw_char(uint16_t x, uint16_t y, char c, uint16_t fg_color, uint16_t bg_color) {
    lcd_show_char(x, y, c, fg_color, bg_color);
}

void lcd_show_string(uint16_t x, uint16_t y, const char* str, uint16_t fg_color, uint16_t bg_color) {
    if (!str) return;
    if (lcd_lock(100)) {
        uint16_t cur_x = x;
        uint16_t cur_y = y;
        while (*str) {
            if (*str == '\n') {
                cur_x = x;
                cur_y += 16;
                str++;
                continue;
            }
            if (cur_x + 8 > LCD_WIDTH) {
                cur_x = x;
                cur_y += 16;
            }
            if (cur_y + 16 > LCD_HEIGHT) {
                break;
            }
            lcd_show_char(cur_x, cur_y, *str++, fg_color, bg_color);
            cur_x += 8;
        }
        lcd_unlock();
    }
}

void lcd_draw_string(uint16_t x, uint16_t y, const char* str, uint16_t fg_color, uint16_t bg_color) {
    lcd_show_string(x, y, str, fg_color, bg_color);
}

static uint8_t s_cursor_col = 0;
static uint8_t s_cursor_row = 0;

void lcd_set_cursor(uint8_t col, uint8_t row) {
    s_cursor_col = col;
    s_cursor_row = row;
}

void lcd_print(const char* str) {
    if (!str) return;
    if (lcd_lock(100)) {
        while (*str) {
            uint16_t x = s_cursor_col * 8;
            uint16_t y = s_cursor_row * 16;
            if (x + 8 <= LCD_WIDTH && y + 16 <= LCD_HEIGHT) {
                lcd_draw_char(x, y, *str, LCD_COLOR_WHITE, LCD_COLOR_BLACK);
                s_cursor_col++;
                if (s_cursor_col >= 30) {
                    s_cursor_col = 0;
                    s_cursor_row++;
                }
            }
            str++;
        }
        lcd_unlock();
    }
}

void lcd_display_lines(const char* line1, const char* line2) {
    char buf1[31];
    char buf2[31];
    snprintf(buf1, sizeof(buf1), "%-30.30s", line1 ? line1 : "");
    snprintf(buf2, sizeof(buf2), "%-30.30s", line2 ? line2 : "");
    lcd_draw_string(0, 0, buf1, LCD_COLOR_WHITE, LCD_COLOR_BLACK);
    lcd_draw_string(0, 16, buf2, LCD_COLOR_WHITE, LCD_COLOR_BLACK);
}

#else

/* Native test stubs */
static char s_line1[31] = {0};
static char s_line2[31] = {0};

void lcd_init(void) {}
bool lcd_lock(uint32_t timeout_ms) { (void)timeout_ms; return true; }
void lcd_unlock(void) {}
void lcd_set_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) { (void)x0; (void)y0; (void)x1; (void)y1; }
void lcd_address_set(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) { (void)x1; (void)y1; (void)x2; (void)y2; }
void lcd_write_half_word(uint16_t da) { (void)da; }
void lcd_fill_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) { (void)x; (void)y; (void)w; (void)h; (void)color; }
void lcd_clear_screen(uint16_t color) { (void)color; memset(s_line1, 0, sizeof(s_line1)); memset(s_line2, 0, sizeof(s_line2)); }
void lcd_clear(void) { lcd_clear_screen(0); }
void lcd_draw_point(uint16_t x, uint16_t y, uint16_t color) { (void)x; (void)y; (void)color; }
void lcd_draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color) { (void)x1; (void)y1; (void)x2; (void)y2; (void)color; }
void lcd_draw_char(uint16_t x, uint16_t y, char c, uint16_t fg_color, uint16_t bg_color) { (void)x; (void)y; (void)c; (void)fg_color; (void)bg_color; }
void lcd_show_char(uint16_t x, uint16_t y, char c, uint16_t fg_color, uint16_t bg_color) { (void)x; (void)y; (void)c; (void)fg_color; (void)bg_color; }
void lcd_draw_string(uint16_t x, uint16_t y, const char* str, uint16_t fg_color, uint16_t bg_color) { (void)x; (void)y; (void)str; (void)fg_color; (void)bg_color; }
void lcd_show_string(uint16_t x, uint16_t y, const char* str, uint16_t fg_color, uint16_t bg_color) { (void)x; (void)y; (void)str; (void)fg_color; (void)bg_color; }
void lcd_set_cursor(uint8_t col, uint8_t row) { (void)col; (void)row; }
void lcd_print(const char* str) { (void)str; }
void lcd_display_lines(const char* line1, const char* line2) {
    snprintf(s_line1, sizeof(s_line1), "%-30.30s", line1 ? line1 : "");
    snprintf(s_line2, sizeof(s_line2), "%-30.30s", line2 ? line2 : "");
}

#endif
