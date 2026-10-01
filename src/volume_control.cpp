#include "volume_control.h"

static volatile uint8_t s_current_volume = 50; // Default 50%

uint8_t scale_adc_to_volume(uint16_t adc_val) {
    if (adc_val >= ADC_MAX_RAW_VALUE) {
        return 100;
    }
    // Round to nearest integer: (adc_val * 100 + 2047) / 4095
    uint32_t scaled = ((uint32_t)adc_val * 100 + (ADC_MAX_RAW_VALUE / 2)) / ADC_MAX_RAW_VALUE;
    if (scaled > 100) {
        scaled = 100;
    }
    return (uint8_t)scaled;
}

float volume_percent_to_gain(uint8_t volume_percent) {
    if (volume_percent > 100) {
        volume_percent = 100;
    }
    return (float)volume_percent / 100.0f;
}

void volume_set_level(uint8_t level) {
    if (level > 100) {
        level = 100;
    }
    s_current_volume = level;
}

uint8_t volume_get_level(void) {
    return s_current_volume;
}

float volume_get_gain(void) {
    return volume_percent_to_gain(s_current_volume);
}
