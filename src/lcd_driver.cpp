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
#include "task.h"
#include "hardware_config.h"

_lcd_dev lcddev = {
    LCD_WIDTH,  /* width */
    LCD_HEIGHT, /* height */
    0,          /* id */
    0,          /* dir (0: vertical) */
    0x2C,       /* wramcmd */
    0x2A,       /* setxcmd */
    0x2B        /* setycmd */
};

static SRAM_HandleTypeDef hsram;
static TIM_HandleTypeDef s_htim14;
static SemaphoreHandle_t s_lcd_mutex = NULL;

#define LCD_WR_REG(cmd)    do { LCD->_u8_REG = (uint8_t)(cmd); } while(0)
#define LCD_WR_DATA8(val)  do { LCD->_u8_RAM = (uint8_t)(val); } while(0)
#define LCD_WR_DATA16(val) do { LCD->_u16_RAM = (uint16_t)(val); } while(0)
#define LCD_RD_DATA8()     (LCD->_u8_RAM)

void lcd_write_half_word(uint16_t da) {
    uint16_t data = 0;
    data = da >> 8;
    data += (da & 0xff) << 8;
    LCD_WR_DATA16(data);
}

bool lcd_lock(uint32_t timeout_ms) {
    if (xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED) {
        return true;
    }
    if (!s_lcd_mutex) {
        s_lcd_mutex = xSemaphoreCreateRecursiveMutex();
    }
    TickType_t ticks = (timeout_ms == UINT32_MAX) ? portMAX_DELAY : pdMS_TO_TICKS(timeout_ms);
    return (xSemaphoreTakeRecursive(s_lcd_mutex, ticks) == pdTRUE);
}

void lcd_unlock(void) {
    if (xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED) {
        return;
    }
    if (s_lcd_mutex) {
        xSemaphoreGiveRecursive(s_lcd_mutex);
    }
}

void lcd_address_set(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
    LCD_WR_REG(lcddev.setxcmd);
    LCD_WR_DATA8((uint8_t)(x1 >> 8));
    LCD_WR_DATA8((uint8_t)(x1 & 0xFF));
    LCD_WR_DATA8((uint8_t)(x2 >> 8));
    LCD_WR_DATA8((uint8_t)(x2 & 0xFF));

    LCD_WR_REG(lcddev.setycmd);
    LCD_WR_DATA8((uint8_t)(y1 >> 8));
    LCD_WR_DATA8((uint8_t)(y1 & 0xFF));
    LCD_WR_DATA8((uint8_t)(y2 >> 8));
    LCD_WR_DATA8((uint8_t)(y2 & 0xFF));

    LCD_WR_REG(lcddev.wramcmd);
}

void lcd_display_dir(uint8_t dir) {
    lcddev.dir = dir;
    if (dir == 0) {
        lcddev.width = 240;
        lcddev.height = 240;
        lcddev.wramcmd = 0x2C;
        lcddev.setxcmd = 0x2A;
        lcddev.setycmd = 0x2B;
        LCD_WR_REG(0x36);
        LCD_WR_DATA8(0x00);
    } else {
        lcddev.width = 240;
        lcddev.height = 240;
        lcddev.wramcmd = 0x2C;
        lcddev.setxcmd = 0x2A;
        lcddev.setycmd = 0x2B;
        LCD_WR_REG(0x36);
        LCD_WR_DATA8(0x70);
    }
    lcd_address_set(0, 0, lcddev.width - 1, lcddev.height - 1);
}

void LCD_Display_Dir(uint8_t dir) {
    lcd_display_dir(dir);
}

void lcd_backlight_init(void) {
    s_htim14.Instance = TIM14;
    s_htim14.Init.Prescaler = 84 - 1; /* 84MHz APB1 timer clock / 84 = 1MHz (1us tick) */
    s_htim14.Init.CounterMode = TIM_COUNTERMODE_UP;
    s_htim14.Init.Period = 50 - 1;    /* 50us period = 20kHz (PWM_BL_PERIOD 50000ns) */
    s_htim14.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    s_htim14.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    if (HAL_TIM_PWM_Init(&s_htim14) == HAL_OK) {
        TIM_OC_InitTypeDef sConfigOC = {0};
        sConfigOC.OCMode = TIM_OCMODE_PWM1;
        sConfigOC.Pulse = 40; /* 80% duty cycle (40/50), matching RT-Spark LCD_BackLightSet(80) */
        sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
        sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
        HAL_TIM_PWM_ConfigChannel(&s_htim14, &sConfigOC, TIM_CHANNEL_1);
        HAL_TIM_PWM_Start(&s_htim14, TIM_CHANNEL_1);
    }
}

void lcd_backlight_set(uint8_t value) {
    if (value > 100) value = 100;
    uint32_t pulse = (uint32_t)(50 * value) / 100;
    if (s_htim14.Instance == TIM14) {
        __HAL_TIM_SET_COMPARE(&s_htim14, TIM_CHANNEL_1, pulse);
    }
}

void LCD_BackLightSet(uint8_t value) {
    lcd_backlight_set(value);
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

    HAL_StatusTypeDef status = HAL_SRAM_Init(&hsram, &read_timing, &write_timing);
    if (status != HAL_OK) {
        printf("[LCD] ERROR: HAL_SRAM_Init failed: %d\r\n", status);
    } else {
        printf("[LCD] HAL_SRAM_Init OK\r\n");
    }
}

void lcd_init(void) {
    if (!s_lcd_mutex) {
        s_lcd_mutex = xSemaphoreCreateRecursiveMutex();
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

    /* 4. Read LCD ID (ST7789 command 0x04) */
    LCD_WR_REG(0x04);
    (void)LCD_RD_DATA8(); /* dummy read */
    uint8_t id1 = LCD_RD_DATA8();
    uint8_t id2 = LCD_RD_DATA8();
    uint8_t id3 = LCD_RD_DATA8();
    lcddev.id = ((uint16_t)id2 << 8) | id3;
    printf("[LCD] Read ID: 0x%04X (id1=0x%02X, id2=0x%02X, id3=0x%02X)\r\n",
           lcddev.id, id1, id2, id3);

    /* 5. ST7789 Initialization Sequence (RT-Spark BSP) */
    /* Memory Data Access Control */
    LCD_WR_REG(0x36);
    LCD_WR_DATA8(0x00);

    /* RGB 5-6-5-bit (16-bit format, RT-Spark BSP uses 0x65) */
    LCD_WR_REG(0x3A); 
    LCD_WR_DATA8(0x65);

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

    /* 6. Write timing speedup (RT-Spark BSP) */
    FSMC_Bank1E->BWTR[4] &= ~(0XF << 0);
    FSMC_Bank1E->BWTR[4] &= ~(0XF << 8);
    FSMC_Bank1E->BWTR[4] |= 3 << 0;
    FSMC_Bank1E->BWTR[4] |= 2 << 8;

    FSMC_Bank1E->BWTR[6] &= ~(0XF << 0);
    FSMC_Bank1E->BWTR[6] &= ~(0XF << 8);
    FSMC_Bank1E->BWTR[6] |= 3 << 0;
    FSMC_Bank1E->BWTR[6] |= 2 << 8;

    /* 7. Default display direction */
    lcd_display_dir(0);

    /* 8. Clear screen before turning on backlight */
    lcd_clear_screen(LCD_COLOR_BLACK);

    /* 9. Initialize backlight PWM (20kHz, 80% duty) and start */
    lcd_backlight_init();
    lcd_backlight_set(80);
}

void lcd_set_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    if (x0 >= lcddev.width) x0 = lcddev.width - 1;
    if (y0 >= lcddev.height) y0 = lcddev.height - 1;
    if (x1 >= lcddev.width) x1 = lcddev.width - 1;
    if (y1 >= lcddev.height) y1 = lcddev.height - 1;

    lcd_address_set(x0, y0, x1, y1);
}

void lcd_fill_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {
    if (x >= lcddev.width || y >= lcddev.height || w == 0 || h == 0) return;
    if (x + w > lcddev.width) w = lcddev.width - x;
    if (y + h > lcddev.height) h = lcddev.height - y;

    if (lcd_lock(100)) {
        lcd_address_set(x, y, x + w - 1, y + h - 1);
        uint32_t total = (uint32_t)w * h;
        for (uint32_t i = 0; i < total; i++) {
            lcd_write_half_word(color);
        }
        lcd_unlock();
    }
}

void lcd_clear_screen(uint16_t color) {
    if (lcd_lock(100)) {
        uint32_t totalpoint = (uint32_t)lcddev.width * lcddev.height;
        lcd_address_set(0, 0, lcddev.width - 1, lcddev.height - 1);
        for (uint32_t index = 0; index < totalpoint; index++) {
            lcd_write_half_word(color);
        }
        lcd_unlock();
    }
}

void lcd_clear(void) {
    lcd_clear_screen(LCD_COLOR_BLACK);
}

void lcd_draw_point(uint16_t x, uint16_t y, uint16_t color) {
    if (x >= lcddev.width || y >= lcddev.height) return;
    if (lcd_lock(100)) {
        lcd_address_set(x, y, x, y);
        lcd_write_half_word(color);
        lcd_unlock();
    }
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
    if (x + 8 > lcddev.width || y + 16 > lcddev.height) return;
    if (c < ' ' || c > '~') c = ' ';

    uint16_t font_offset = ((uint16_t)(c - ' ')) * 16;
    if (lcd_lock(100)) {
        lcd_address_set(x, y, x + 7, y + 15);
        for (int r = 0; r < 16; r++) {
            uint8_t line = asc2_1608[font_offset + r];
            for (int b = 0; b < 8; b++) {
                if (line & (0x80 >> b)) {
                    lcd_write_half_word(fg_color);
                } else {
                    lcd_write_half_word(bg_color);
                }
            }
        }
        lcd_unlock();
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
            if (cur_x + 8 > lcddev.width) {
                cur_x = x;
                cur_y += 16;
            }
            if (cur_y + 16 > lcddev.height) {
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
            if (x + 8 <= lcddev.width && y + 16 <= lcddev.height) {
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

_lcd_dev lcddev = {
    LCD_WIDTH,  /* width */
    LCD_HEIGHT, /* height */
    0,          /* id */
    0,          /* dir */
    0x2C,       /* wramcmd */
    0x2A,       /* setxcmd */
    0x2B        /* setycmd */
};

void lcd_init(void) {}
void lcd_display_dir(uint8_t dir) { lcddev.dir = dir; }
void LCD_Display_Dir(uint8_t dir) { lcd_display_dir(dir); }
void lcd_backlight_init(void) {}
void lcd_backlight_set(uint8_t value) { (void)value; }
void LCD_BackLightSet(uint8_t value) { (void)value; }
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
