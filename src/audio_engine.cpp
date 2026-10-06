#include "audio_engine.h"
#include "song_def.h"
#include "volume_control.h"
#include "rtos_timer_interface.h"

#ifndef NATIVE_TEST
#include "stm32f4xx_hal.h"
#include "hardware_config.h"

static TIM_HandleTypeDef s_htim4;
static Ticker s_note_ticker;
static const Song* s_active_song = nullptr;
static uint16_t s_note_idx = 0;
static bool s_playing = false;
static uint32_t s_note_ms_remaining = 0;

static void audio_set_frequency(float freq_hz, float volume_gain) {
    if (freq_hz <= 0.0f || volume_gain <= 0.001f) {
        // Silence
        __HAL_TIM_SET_COMPARE(&s_htim4, TIM_CHANNEL_3, 0);
        return;
    }

    // APB1 timer clock on STM32F407 is 84MHz
    uint32_t prescaler = 84 - 1; // 1MHz counting clock
    uint32_t arr = (uint32_t)(1000000.0f / freq_hz);
    if (arr < 2) arr = 2;

    // Duty cycle scaled by volume (50% max duty for square wave)
    uint32_t ccr = (uint32_t)((float)arr * 0.5f * volume_gain);

    __HAL_TIM_SET_PRESCALER(&s_htim4, prescaler);
    __HAL_TIM_SET_AUTORELOAD(&s_htim4, arr);
    __HAL_TIM_SET_COMPARE(&s_htim4, TIM_CHANNEL_3, ccr);
}

static void audio_advance_note(void) {
    if (!s_playing || !s_active_song) {
        audio_set_frequency(0, 0);
        return;
    }

    if (s_note_idx >= s_active_song->length) {
        // Loop back to beginning for MP3 player replay
        s_note_idx = 0;
    }

    float note_period_ms = s_active_song->note[s_note_idx];
    float beat_val = s_active_song->beat[s_note_idx];
    float tempo = s_active_song->tempo;

    float freq = (note_period_ms > 0.0f) ? (1000.0f / note_period_ms) : 0.0f;
    float gain = volume_get_gain();

    audio_set_frequency(freq, gain);

    // Duration in seconds: beat * tempo * 4.0
    float duration_s = beat_val * tempo * 4.0f;
    if (duration_s < 0.02f) duration_s = 0.02f;

    s_note_idx++;

    // Remaining time for this note, consumed by the 1 ms music Ticker
    s_note_ms_remaining = (uint32_t)(duration_s * 1000.0f);
    if (s_note_ms_remaining == 0) s_note_ms_remaining = 1;
}

/* Music sequencer step, called repeatedly by the Ticker interface (lab
 * requirement: use the Ticker interface to play music). One tick = 1 ms
 * (configTICK_RATE_HZ = 1000), giving millisecond note timing accuracy. */
static void audio_ticker_cb(void) {
    if (!s_playing) return;
    if (s_note_ms_remaining > 0) {
        s_note_ms_remaining--;
    }
    if (s_note_ms_remaining == 0) {
        audio_advance_note();
    }
}

void audio_engine_init(void) {
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_TIM4_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = AUDIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF2_TIM4;
    HAL_GPIO_Init(AUDIO_PORT, &GPIO_InitStruct);

    s_htim4.Instance = TIM4;
    s_htim4.Init.Prescaler = 84 - 1;
    s_htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
    s_htim4.Init.Period = 1000;
    s_htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    s_htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    HAL_TIM_PWM_Init(&s_htim4);

    TIM_OC_InitTypeDef sConfigOC = {0};
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 0;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    HAL_TIM_PWM_ConfigChannel(&s_htim4, &sConfigOC, TIM_CHANNEL_3);
    HAL_TIM_PWM_Start(&s_htim4, TIM_CHANNEL_3);
}

void audio_engine_play_song(uint8_t song_index) {
    s_active_song = get_song(song_index);
    if (!s_active_song) return;

    s_note_idx = 0;
    s_playing = true;
    audio_advance_note();
    s_note_ticker.attach_ms(audio_ticker_cb, 1);
}

void audio_engine_pause(void) {
    s_playing = false;
    s_note_ticker.detach();
    audio_set_frequency(0, 0);
}

void audio_engine_resume(void) {
    // Guard: if already playing (e.g. confirmation canceled mid-song),
    // don't re-trigger the current note or disturb the pending note timer.
    if (!s_active_song || s_playing) return;
    s_playing = true;
    audio_advance_note();
    s_note_ticker.attach_ms(audio_ticker_cb, 1);
}

void audio_engine_stop(void) {
    s_playing = false;
    s_note_ticker.detach();
    s_note_idx = 0;
    s_note_ms_remaining = 0;
    audio_set_frequency(0, 0);
}

void audio_engine_set_volume(uint8_t volume_percent) {
    volume_set_level(volume_percent);
    if (s_playing && s_active_song && s_note_idx > 0) {
        float note_period = s_active_song->note[s_note_idx - 1];
        float freq = (note_period > 0.0f) ? (1000.0f / note_period) : 0.0f;
        audio_set_frequency(freq, volume_percent_to_gain(volume_percent));
    }
}

uint8_t audio_engine_get_volume(void) {
    return volume_get_level();
}

bool audio_engine_is_playing(void) {
    return s_playing;
}

uint16_t audio_engine_get_current_note_index(void) {
    return s_note_idx;
}

#else

/* Native test stubs */
static bool s_playing = false;
static uint16_t s_note_idx = 0;
void audio_engine_init(void) {}
void audio_engine_play_song(uint8_t song_index) { (void)song_index; s_playing = true; s_note_idx = 0; }
void audio_engine_pause(void) { s_playing = false; }
void audio_engine_resume(void) { s_playing = true; }
void audio_engine_stop(void) { s_playing = false; s_note_idx = 0; }
void audio_engine_set_volume(uint8_t volume_percent) { volume_set_level(volume_percent); }
uint8_t audio_engine_get_volume(void) { return volume_get_level(); }
bool audio_engine_is_playing(void) { return s_playing; }
uint16_t audio_engine_get_current_note_index(void) { return s_note_idx; }

#endif
