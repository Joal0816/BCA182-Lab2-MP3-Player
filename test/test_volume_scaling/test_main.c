#include "unity.h"
#include "volume_control.h"

void setUp(void) {
    volume_set_level(50);
}

void tearDown(void) {}

void test_scale_adc_to_volume_boundaries(void) {
    TEST_ASSERT_EQUAL_UINT8(0, scale_adc_to_volume(0));
    TEST_ASSERT_EQUAL_UINT8(100, scale_adc_to_volume(ADC_MAX_RAW_VALUE));
    TEST_ASSERT_EQUAL_UINT8(100, scale_adc_to_volume(5000));
}

void test_scale_adc_to_volume_intermediate(void) {
    // 2047.5 is 50% of 4095
    TEST_ASSERT_EQUAL_UINT8(50, scale_adc_to_volume(2048));
    // 1024 is ~25%
    TEST_ASSERT_EQUAL_UINT8(25, scale_adc_to_volume(1024));
    // 3071 is ~75%
    TEST_ASSERT_EQUAL_UINT8(75, scale_adc_to_volume(3071));
}

void test_volume_percent_to_gain(void) {
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0f, volume_percent_to_gain(0));
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.5f, volume_percent_to_gain(50));
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 1.0f, volume_percent_to_gain(100));
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 1.0f, volume_percent_to_gain(120));
}

void test_volume_get_and_set_level(void) {
    volume_set_level(0);
    TEST_ASSERT_EQUAL_UINT8(0, volume_get_level());
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0f, volume_get_gain());

    volume_set_level(80);
    TEST_ASSERT_EQUAL_UINT8(80, volume_get_level());
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.8f, volume_get_gain());

    volume_set_level(150); // should clamp to 100
    TEST_ASSERT_EQUAL_UINT8(100, volume_get_level());
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 1.0f, volume_get_gain());
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_scale_adc_to_volume_boundaries);
    RUN_TEST(test_scale_adc_to_volume_intermediate);
    RUN_TEST(test_volume_percent_to_gain);
    RUN_TEST(test_volume_get_and_set_level);
    return UNITY_END();
}
