#ifndef VOLUME_CONTROL_H
#define VOLUME_CONTROL_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ADC_MAX_RAW_VALUE 4095

/**
 * @brief Linearly scales raw 12-bit ADC value (0..4095) to volume percentage (0..100%).
 * Clamps input values above 4095 to 100%.
 */
uint8_t scale_adc_to_volume(uint16_t adc_val);

/**
 * @brief Converts a volume percentage (0..100) to a linear gain factor (0.0f..1.0f).
 */
float volume_percent_to_gain(uint8_t volume_percent);

void volume_set_level(uint8_t level);
uint8_t volume_get_level(void);
float volume_get_gain(void);

#ifdef __cplusplus
}
#endif

#endif /* VOLUME_CONTROL_H */
