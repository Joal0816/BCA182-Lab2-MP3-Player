#include "button_decoder.h"

uint8_t decode_binary_buttons(bool b2, bool b3, bool b4) {
    uint8_t val = 0;
    if (b2) val |= 0x04;
    if (b3) val |= 0x02;
    if (b4) val |= 0x01;
    return val;
}

uint8_t decode_active_low_buttons(bool pin2_val, bool pin3_val, bool pin4_val) {
    // In pull-up active-low setup: 0 is pressed, 1 is released
    bool b2 = !pin2_val;
    bool b3 = !pin3_val;
    bool b4 = !pin4_val;
    return decode_binary_buttons(b2, b3, b4);
}
