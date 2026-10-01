#include "unity.h"
#include "player_state.h"

static PlayerContext ctx;

void setUp(void) {
    player_init(&ctx);
}

void tearDown(void) {}

void test_initial_state_is_stopped(void) {
    TEST_ASSERT_EQUAL(PLAYER_STATE_STOPPED, player_get_state(&ctx));
    TEST_ASSERT_EQUAL_UINT8(0, player_get_current_song(&ctx));
    TEST_ASSERT_EQUAL_UINT8(0, player_get_candidate_song(&ctx));
    TEST_ASSERT_EQUAL_UINT8(0, player_get_countdown(&ctx));
}

void test_user_button_toggles_play_and_pause(void) {
    // STOPPED -> PLAYING
    player_handle_event(&ctx, PLAYER_EVENT_USER_BTN, 0);
    TEST_ASSERT_EQUAL(PLAYER_STATE_PLAYING, player_get_state(&ctx));

    // PLAYING -> PAUSED
    player_handle_event(&ctx, PLAYER_EVENT_USER_BTN, 0);
    TEST_ASSERT_EQUAL(PLAYER_STATE_PAUSED, player_get_state(&ctx));

    // PAUSED -> PLAYING
    player_handle_event(&ctx, PLAYER_EVENT_USER_BTN, 0);
    TEST_ASSERT_EQUAL(PLAYER_STATE_PLAYING, player_get_state(&ctx));
}

void test_button1_select_enters_confirming_state(void) {
    // Current song is 0, select candidate 5
    player_handle_event(&ctx, PLAYER_EVENT_BTN1_SELECT, 5);
    TEST_ASSERT_EQUAL(PLAYER_STATE_CONFIRMING, player_get_state(&ctx));
    TEST_ASSERT_EQUAL_UINT8(5, player_get_candidate_song(&ctx));
    TEST_ASSERT_EQUAL_UINT8(0, player_get_current_song(&ctx));
    TEST_ASSERT_EQUAL_UINT8(5, player_get_countdown(&ctx));
}

void test_button1_confirm_applies_new_song(void) {
    player_handle_event(&ctx, PLAYER_EVENT_BTN1_SELECT, 3);
    TEST_ASSERT_EQUAL(PLAYER_STATE_CONFIRMING, player_get_state(&ctx));

    player_handle_event(&ctx, PLAYER_EVENT_BTN1_CONFIRM, 0);
    TEST_ASSERT_EQUAL(PLAYER_STATE_PLAYING, player_get_state(&ctx));
    TEST_ASSERT_EQUAL_UINT8(3, player_get_current_song(&ctx));
    TEST_ASSERT_EQUAL_UINT8(0, player_get_countdown(&ctx));
}

void test_confirm_timeout_reverts_state_and_preserves_song(void) {
    // Start in PLAYING with song 0
    player_handle_event(&ctx, PLAYER_EVENT_USER_BTN, 0);
    TEST_ASSERT_EQUAL(PLAYER_STATE_PLAYING, player_get_state(&ctx));

    // Select song 6
    player_handle_event(&ctx, PLAYER_EVENT_BTN1_SELECT, 6);
    TEST_ASSERT_EQUAL(PLAYER_STATE_CONFIRMING, player_get_state(&ctx));

    // Timeout occurs
    player_handle_event(&ctx, PLAYER_EVENT_CONFIRM_TIMEOUT, 0);
    TEST_ASSERT_EQUAL(PLAYER_STATE_PLAYING, player_get_state(&ctx));
    TEST_ASSERT_EQUAL_UINT8(0, player_get_current_song(&ctx));
    TEST_ASSERT_EQUAL_UINT8(0, player_get_countdown(&ctx));
}

void test_tick_second_countdown_and_auto_revert(void) {
    // From PAUSED state
    player_handle_event(&ctx, PLAYER_EVENT_USER_BTN, 0); // PLAYING
    player_handle_event(&ctx, PLAYER_EVENT_USER_BTN, 0); // PAUSED
    TEST_ASSERT_EQUAL(PLAYER_STATE_PAUSED, player_get_state(&ctx));

    player_handle_event(&ctx, PLAYER_EVENT_BTN1_SELECT, 2);
    TEST_ASSERT_EQUAL(PLAYER_STATE_CONFIRMING, player_get_state(&ctx));
    TEST_ASSERT_EQUAL_UINT8(5, player_get_countdown(&ctx));

    player_tick_second(&ctx);
    TEST_ASSERT_EQUAL_UINT8(4, player_get_countdown(&ctx));
    TEST_ASSERT_EQUAL(PLAYER_STATE_CONFIRMING, player_get_state(&ctx));

    player_tick_second(&ctx);
    player_tick_second(&ctx);
    player_tick_second(&ctx);
    TEST_ASSERT_EQUAL_UINT8(1, player_get_countdown(&ctx));
    TEST_ASSERT_EQUAL(PLAYER_STATE_CONFIRMING, player_get_state(&ctx));

    // 5th tick expires the countdown
    player_tick_second(&ctx);
    TEST_ASSERT_EQUAL_UINT8(0, player_get_countdown(&ctx));
    TEST_ASSERT_EQUAL(PLAYER_STATE_PAUSED, player_get_state(&ctx));
    TEST_ASSERT_EQUAL_UINT8(0, player_get_current_song(&ctx));
}

void test_user_btn_during_confirming_cancels_selection(void) {
    player_handle_event(&ctx, PLAYER_EVENT_BTN1_SELECT, 4);
    TEST_ASSERT_EQUAL(PLAYER_STATE_CONFIRMING, player_get_state(&ctx));

    player_handle_event(&ctx, PLAYER_EVENT_USER_BTN, 0);
    TEST_ASSERT_EQUAL(PLAYER_STATE_STOPPED, player_get_state(&ctx));
    TEST_ASSERT_EQUAL_UINT8(0, player_get_current_song(&ctx));
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_initial_state_is_stopped);
    RUN_TEST(test_user_button_toggles_play_and_pause);
    RUN_TEST(test_button1_select_enters_confirming_state);
    RUN_TEST(test_button1_confirm_applies_new_song);
    RUN_TEST(test_confirm_timeout_reverts_state_and_preserves_song);
    RUN_TEST(test_tick_second_countdown_and_auto_revert);
    RUN_TEST(test_user_btn_during_confirming_cancels_selection);
    return UNITY_END();
}
