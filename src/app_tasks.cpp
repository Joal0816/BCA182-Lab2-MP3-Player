#include "app_tasks.h"
#include "player_state.h"
#include "button_decoder.h"
#include "volume_control.h"
#include "audio_engine.h"
#include "lcd_driver.h"
#include "song_def.h"
#include "rtos_timer_interface.h"

#include <stdio.h>

#ifndef NATIVE_TEST
#include "stm32f4xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "hardware_config.h"

extern ADC_HandleTypeDef s_hadc1;
static PlayerContext s_player;
static Timeout s_confirm_timeout;
static Ticker s_second_ticker;

static void set_rgb_leds(PlayerState state) {
    switch (state) {
        case PLAYER_STATE_PLAYING:
            // Blue LED ON
            HAL_GPIO_WritePin(LED_RED_PORT, LED_RED_PIN, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(LED_GREEN_PORT, LED_GREEN_PIN, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(LED_BLUE_PORT, LED_BLUE_PIN, GPIO_PIN_SET);
            break;

        case PLAYER_STATE_PAUSED:
            // Red LED ON
            HAL_GPIO_WritePin(LED_RED_PORT, LED_RED_PIN, GPIO_PIN_SET);
            HAL_GPIO_WritePin(LED_GREEN_PORT, LED_GREEN_PIN, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(LED_BLUE_PORT, LED_BLUE_PIN, GPIO_PIN_RESET);
            break;

        case PLAYER_STATE_CONFIRMING:
            // Green LED ON
            HAL_GPIO_WritePin(LED_RED_PORT, LED_RED_PIN, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(LED_GREEN_PORT, LED_GREEN_PIN, GPIO_PIN_SET);
            HAL_GPIO_WritePin(LED_BLUE_PORT, LED_BLUE_PIN, GPIO_PIN_RESET);
            break;

        case PLAYER_STATE_STOPPED:
        default:
            HAL_GPIO_WritePin(LED_RED_PORT, LED_RED_PIN, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(LED_GREEN_PORT, LED_GREEN_PIN, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(LED_BLUE_PORT, LED_BLUE_PIN, GPIO_PIN_RESET);
            break;
    }
}

static void on_confirm_timeout_cb(void) {
    s_second_ticker.detach();
    player_handle_event(&s_player, PLAYER_EVENT_CONFIRM_TIMEOUT, 0);
}

static void on_second_tick_cb(void) {
    player_tick_second(&s_player);
}

void update_lcd_leds_thread(void *pvParameters) {
    (void)pvParameters;

    PlayerState prev_state = (PlayerState)0xFF;
    char hdr[31];
    char l_status[31];
    char l_song[31];
    char l_comp[31];
    char l_vol[31];
    char l_hint[31];

    uint16_t hdr_color = LCD_COLOR_CYAN;
    uint16_t status_color = LCD_COLOR_WHITE;
    uint16_t song_color = LCD_COLOR_WHITE;
    uint16_t comp_color = LCD_COLOR_GRAY;
    uint16_t vol_color = LCD_COLOR_CYAN;
    uint16_t hint_color = LCD_COLOR_DARKGRAY;

    for (;;) {
        PlayerState state = player_get_state(&s_player);
        set_rgb_leds(state);

        uint8_t curr_idx = player_get_current_song(&s_player);
        const Song* curr_song = get_song(curr_idx);

        if (state != prev_state) {
            lcd_clear();
            prev_state = state;
        }

        switch (state) {
            case PLAYER_STATE_CONFIRMING: {
                uint8_t cand_idx = player_get_candidate_song(&s_player);
                const Song* cand_song = get_song(cand_idx);
                uint8_t cd = player_get_countdown(&s_player);

                snprintf(hdr, sizeof(hdr), "%-30.30s", "     >> CHANGE SONG? <<");
                snprintf(l_status, sizeof(l_status), "%-30.30s", "     Candidate Song:");
                snprintf(l_song, sizeof(l_song), "%-30.30s", cand_song ? cand_song->name1 : "");
                char cd_buf[32];
                snprintf(cd_buf, sizeof(cd_buf), "Confirm in: %ds", cd);
                snprintf(l_comp, sizeof(l_comp), "%-30.30s", cd_buf);
                snprintf(l_vol, sizeof(l_vol), "%-30.30s", "Press BTN1 to OK");
                snprintf(l_hint, sizeof(l_hint), "%-30.30s", "USER Button = Cancel");

                hdr_color = LCD_COLOR_GREEN;
                status_color = LCD_COLOR_YELLOW;
                song_color = LCD_COLOR_WHITE;
                comp_color = LCD_COLOR_YELLOW;
                vol_color = LCD_COLOR_GREEN;
                hint_color = LCD_COLOR_DARKGRAY;
                break;
            }

            case PLAYER_STATE_PLAYING: {
                uint8_t vol = volume_get_level();
                char bar[11];
                int filled = (vol + 5) / 10;
                for (int i = 0; i < 10; i++) bar[i] = (i < filled) ? '=' : ' ';
                bar[10] = '\0';
                char vol_buf[32];
                snprintf(vol_buf, sizeof(vol_buf), "Vol: [%s] %3d%%", bar, vol);

                snprintf(hdr, sizeof(hdr), "%-30.30s", "      == MP3 PLAYER ==");
                snprintf(l_status, sizeof(l_status), "%-30.30s", "       State: PLAYING");
                snprintf(l_song, sizeof(l_song), "%-30.30s", curr_song ? curr_song->name1 : "");
                snprintf(l_comp, sizeof(l_comp), "%-30.30s", curr_song ? curr_song->name2 : "");
                snprintf(l_vol, sizeof(l_vol), "%-30.30s", vol_buf);
                snprintf(l_hint, sizeof(l_hint), "%-30.30s", "USER=Pause | BTN1=Select");

                hdr_color = LCD_COLOR_CYAN;
                status_color = LCD_COLOR_GREEN;
                song_color = LCD_COLOR_WHITE;
                comp_color = LCD_COLOR_GRAY;
                vol_color = LCD_COLOR_CYAN;
                hint_color = LCD_COLOR_DARKGRAY;
                break;
            }

            case PLAYER_STATE_PAUSED: {
                uint8_t vol = volume_get_level();
                char bar[11];
                int filled = (vol + 5) / 10;
                for (int i = 0; i < 10; i++) bar[i] = (i < filled) ? '=' : ' ';
                bar[10] = '\0';
                char vol_buf[32];
                snprintf(vol_buf, sizeof(vol_buf), "Vol: [%s] %3d%%", bar, vol);

                snprintf(hdr, sizeof(hdr), "%-30.30s", "      == MP3 PLAYER ==");
                snprintf(l_status, sizeof(l_status), "%-30.30s", "       State: PAUSED");
                snprintf(l_song, sizeof(l_song), "%-30.30s", curr_song ? curr_song->name1 : "");
                snprintf(l_comp, sizeof(l_comp), "%-30.30s", curr_song ? curr_song->name2 : "");
                snprintf(l_vol, sizeof(l_vol), "%-30.30s", vol_buf);
                snprintf(l_hint, sizeof(l_hint), "%-30.30s", "USER=Resume | BTN1=Select");

                hdr_color = LCD_COLOR_RED;
                status_color = LCD_COLOR_RED;
                song_color = LCD_COLOR_WHITE;
                comp_color = LCD_COLOR_GRAY;
                vol_color = LCD_COLOR_YELLOW;
                hint_color = LCD_COLOR_DARKGRAY;
                break;
            }

            case PLAYER_STATE_STOPPED:
            default: {
                uint8_t vol = volume_get_level();
                char bar[11];
                int filled = (vol + 5) / 10;
                for (int i = 0; i < 10; i++) bar[i] = (i < filled) ? '=' : ' ';
                bar[10] = '\0';
                char vol_buf[32];
                snprintf(vol_buf, sizeof(vol_buf), "Vol: [%s] %3d%%", bar, vol);

                snprintf(hdr, sizeof(hdr), "%-30.30s", "      == MP3 PLAYER ==");
                snprintf(l_status, sizeof(l_status), "%-30.30s", "       State: STOPPED");
                snprintf(l_song, sizeof(l_song), "%-30.30s", "Press USER Button to Play");
                snprintf(l_comp, sizeof(l_comp), "%-30.30s", "Buttons 2-4: Song Binary");
                snprintf(l_vol, sizeof(l_vol), "%-30.30s", vol_buf);
                snprintf(l_hint, sizeof(l_hint), "%-30.30s", "Button 1: Select Candidate");

                hdr_color = LCD_COLOR_YELLOW;
                status_color = LCD_COLOR_GRAY;
                song_color = LCD_COLOR_WHITE;
                comp_color = LCD_COLOR_GRAY;
                vol_color = LCD_COLOR_YELLOW;
                hint_color = LCD_COLOR_DARKGRAY;
                break;
            }
        }

        // Draw full screen layout with mutex protection
        lcd_draw_string(0, 20, hdr, hdr_color, LCD_COLOR_BLACK);
        lcd_draw_string(0, 50, l_status, status_color, LCD_COLOR_BLACK);
        lcd_draw_string(0, 85, l_song, song_color, LCD_COLOR_BLACK);
        lcd_draw_string(0, 120, l_comp, comp_color, LCD_COLOR_BLACK);
        lcd_draw_string(0, 160, l_vol, vol_color, LCD_COLOR_BLACK);
        lcd_draw_string(0, 200, l_hint, hint_color, LCD_COLOR_BLACK);

        // Cooperative scheduling delay
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void polling_buttons(void *pvParameters) {
    (void)pvParameters;

    player_init(&s_player);

    bool prev_btn1 = false;
    bool prev_user = false;

    for (;;) {
        // Read buttons
        // Active-low push buttons with pull-ups
        bool btn1_pressed = (HAL_GPIO_ReadPin(BTN1_PORT, BTN1_PIN) == GPIO_PIN_RESET);
        bool btn2_val = (HAL_GPIO_ReadPin(BTN2_PORT, BTN2_PIN) == GPIO_PIN_SET);
        bool btn3_val = (HAL_GPIO_ReadPin(BTN3_PORT, BTN3_PIN) == GPIO_PIN_SET);
        bool btn4_val = (HAL_GPIO_ReadPin(BTN4_PORT, BTN4_PIN) == GPIO_PIN_SET);

        // User button (PA0, active-high)
        bool user_pressed = (HAL_GPIO_ReadPin(USER_BTN_PORT, USER_BTN_PIN) == GPIO_PIN_SET);

        // Detect user button rising edge
        if (user_pressed && !prev_user) {
            PlayerState prev = player_get_state(&s_player);
            player_handle_event(&s_player, PLAYER_EVENT_USER_BTN, 0);
            PlayerState curr = player_get_state(&s_player);

            if (curr == PLAYER_STATE_PLAYING) {
                if (prev == PLAYER_STATE_STOPPED) {
                    audio_engine_play_song(player_get_current_song(&s_player));
                } else {
                    audio_engine_resume();
                }
            } else if (curr == PLAYER_STATE_PAUSED) {
                audio_engine_pause();
            }
        }
        prev_user = user_pressed;

        // Detect button 1 rising edge
        if (btn1_pressed && !prev_btn1) {
            PlayerState state = player_get_state(&s_player);

            if (state == PLAYER_STATE_CONFIRMING) {
                // Confirm selection
                s_confirm_timeout.detach();
                s_second_ticker.detach();
                player_handle_event(&s_player, PLAYER_EVENT_BTN1_CONFIRM, 0);

                // Play the newly confirmed song
                audio_engine_play_song(player_get_current_song(&s_player));
            } else {
                // Select candidate song using binary buttons 2-4
                uint8_t cand_idx = decode_active_low_buttons(btn2_val, btn3_val, btn4_val);
                player_handle_event(&s_player, PLAYER_EVENT_BTN1_SELECT, cand_idx);

                // Start 5-second countdown timer and 1-second display ticker
                s_confirm_timeout.attach(on_confirm_timeout_cb, 5.0f);
                s_second_ticker.attach(on_second_tick_cb, 1.0f);
            }
        }
        prev_btn1 = btn1_pressed;

        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

void adjust_volume(void *pvParameters) {
    (void)pvParameters;

    for (;;) {
        HAL_ADC_Start(&s_hadc1);
        if (HAL_ADC_PollForConversion(&s_hadc1, 10) == HAL_OK) {
            uint32_t raw_adc = HAL_ADC_GetValue(&s_hadc1);
            uint8_t vol = scale_adc_to_volume((uint16_t)raw_adc);
            audio_engine_set_volume(vol);
        }
        HAL_ADC_Stop(&s_hadc1);

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

#else

/* Native test implementations */
void update_lcd_leds_thread(void *pvParameters) { (void)pvParameters; }
void polling_buttons(void *pvParameters) { (void)pvParameters; }
void adjust_volume(void *pvParameters) { (void)pvParameters; }

#endif
