#include "unity.h"
#include "button_decoder.h"

void setUp(void) {}
void tearDown(void) {}

void test_decode_binary_buttons_all_combinations(void) {
    TEST_ASSERT_EQUAL_UINT8(0, decode_binary_buttons(false, false, false));
    TEST_ASSERT_EQUAL_UINT8(1, decode_binary_buttons(false, false, true));
    TEST_ASSERT_EQUAL_UINT8(2, decode_binary_buttons(false, true,  false));
    TEST_ASSERT_EQUAL_UINT8(3, decode_binary_buttons(false, true,  true));
    TEST_ASSERT_EQUAL_UINT8(4, decode_binary_buttons(true,  false, false));
    TEST_ASSERT_EQUAL_UINT8(5, decode_binary_buttons(true,  false, true));
    TEST_ASSERT_EQUAL_UINT8(6, decode_binary_buttons(true,  true,  false));
    TEST_ASSERT_EQUAL_UINT8(7, decode_binary_buttons(true,  true,  true));
}

void test_decode_active_low_buttons_all_combinations(void) {
    // Active low: 1 = released, 0 = pressed
    TEST_ASSERT_EQUAL_UINT8(0, decode_active_low_buttons(true,  true,  true));
    TEST_ASSERT_EQUAL_UINT8(1, decode_active_low_buttons(true,  true,  false));
    TEST_ASSERT_EQUAL_UINT8(2, decode_active_low_buttons(true,  false, true));
    TEST_ASSERT_EQUAL_UINT8(3, decode_active_low_buttons(true,  false, false));
    TEST_ASSERT_EQUAL_UINT8(4, decode_active_low_buttons(false, true,  true));
    TEST_ASSERT_EQUAL_UINT8(5, decode_active_low_buttons(false, true,  false));
    TEST_ASSERT_EQUAL_UINT8(6, decode_active_low_buttons(false, false, true));
    TEST_ASSERT_EQUAL_UINT8(7, decode_active_low_buttons(false, false, false));
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_decode_binary_buttons_all_combinations);
    RUN_TEST(test_decode_active_low_buttons_all_combinations);
    return UNITY_END();
}
