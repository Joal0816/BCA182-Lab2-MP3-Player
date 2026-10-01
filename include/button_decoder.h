#ifndef BUTTON_DECODER_H
#define BUTTON_DECODER_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Decodes 3 binary button states into a song index [0..7].
 * @param b2 Button 2 state (MSB, weight 4)
 * @param b3 Button 3 state (bit 1, weight 2)
 * @param b4 Button 4 state (LSB, weight 1)
 * @return Song index 0 to 7
 */
uint8_t decode_binary_buttons(bool b2, bool b3, bool b4);

/**
 * @brief Decodes active-low button pins (typical pull-up resistor circuit where 0 = pressed).
 * @param pin2_val Pin 2 digital reading (0 = pressed, 1 = unpressed)
 * @param pin3_val Pin 3 digital reading (0 = pressed, 1 = unpressed)
 * @param pin4_val Pin 4 digital reading (0 = pressed, 1 = unpressed)
 * @return Song index 0 to 7
 */
uint8_t decode_active_low_buttons(bool pin2_val, bool pin3_val, bool pin4_val);

#ifdef __cplusplus
}
#endif

#endif /* BUTTON_DECODER_H */
