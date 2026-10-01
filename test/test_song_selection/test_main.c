#include "unity.h"
#include "song_def.h"
#include <string.h>

void setUp(void) {}
void tearDown(void) {}

void test_song_count_is_eight(void) {
    TEST_ASSERT_EQUAL_UINT32(8, get_song_count());
}

void test_all_songs_have_valid_metadata_and_notes(void) {
    for (size_t i = 0; i < get_song_count(); i++) {
        const Song* song = get_song(i);
        TEST_ASSERT_NOT_NULL(song);
        TEST_ASSERT_NOT_NULL(song->name1);
        TEST_ASSERT_TRUE(strlen(song->name1) > 0);
        TEST_ASSERT_NOT_NULL(song->name2);
        TEST_ASSERT_TRUE(strlen(song->name2) > 0);
        TEST_ASSERT_NOT_NULL(song->note);
        TEST_ASSERT_NOT_NULL(song->beat);
        TEST_ASSERT_TRUE(song->tempo > 0.0f);
        TEST_ASSERT_TRUE(song->length > 0);

        // Verify beat values are valid positive numbers
        for (int b = 0; b < song->length; b++) {
            TEST_ASSERT_TRUE(song->beat[b] > 0.0f);
            TEST_ASSERT_TRUE(song->note[b] >= 0.0f);
        }
    }
}

void test_song_out_of_bounds_returns_null(void) {
    TEST_ASSERT_NULL(get_song(8));
    TEST_ASSERT_NULL(get_song(99));
}

void test_individual_song_definitions_match_array(void) {
    TEST_ASSERT_EQUAL_STRING(FUR_ELISE.name1, get_song(0)->name1);
    TEST_ASSERT_EQUAL_PTR(FUR_ELISE.note, get_song(0)->note);
    TEST_ASSERT_EQUAL_INT(FUR_ELISE.length, get_song(0)->length);

    TEST_ASSERT_EQUAL_STRING(CANNON_IN_D.name1, get_song(1)->name1);
    TEST_ASSERT_EQUAL_PTR(CANNON_IN_D.note, get_song(1)->note);
    TEST_ASSERT_EQUAL_INT(CANNON_IN_D.length, get_song(1)->length);

    TEST_ASSERT_EQUAL_STRING(MINUET_IN_G_MAJOR.name1, get_song(2)->name1);
    TEST_ASSERT_EQUAL_PTR(MINUET_IN_G_MAJOR.note, get_song(2)->note);
    TEST_ASSERT_EQUAL_INT(MINUET_IN_G_MAJOR.length, get_song(2)->length);

    TEST_ASSERT_EQUAL_STRING(TURKISH_MARCH.name1, get_song(3)->name1);
    TEST_ASSERT_EQUAL_PTR(TURKISH_MARCH.note, get_song(3)->note);
    TEST_ASSERT_EQUAL_INT(TURKISH_MARCH.length, get_song(3)->length);

    TEST_ASSERT_EQUAL_STRING(NOCTRUNE_IN_E_FLAT.name1, get_song(4)->name1);
    TEST_ASSERT_EQUAL_PTR(NOCTRUNE_IN_E_FLAT.note, get_song(4)->note);
    TEST_ASSERT_EQUAL_INT(NOCTRUNE_IN_E_FLAT.length, get_song(4)->length);

    TEST_ASSERT_EQUAL_STRING(WALTZ_NO2.name1, get_song(5)->name1);
    TEST_ASSERT_EQUAL_PTR(WALTZ_NO2.note, get_song(5)->note);
    TEST_ASSERT_EQUAL_INT(WALTZ_NO2.length, get_song(5)->length);

    TEST_ASSERT_EQUAL_STRING(NOCTRUNE_IN_C_SHARP_MAJOR.name1, get_song(6)->name1);
    TEST_ASSERT_EQUAL_PTR(NOCTRUNE_IN_C_SHARP_MAJOR.note, get_song(6)->note);
    TEST_ASSERT_EQUAL_INT(NOCTRUNE_IN_C_SHARP_MAJOR.length, get_song(6)->length);

    TEST_ASSERT_EQUAL_STRING(SYMPHONY_NO40.name1, get_song(7)->name1);
    TEST_ASSERT_EQUAL_PTR(SYMPHONY_NO40.note, get_song(7)->note);
    TEST_ASSERT_EQUAL_INT(SYMPHONY_NO40.length, get_song(7)->length);
}

void test_song_titles_contain_expected_names(void) {
    TEST_ASSERT_NOT_NULL(strstr(get_song(0)->name1, "Fur Elise"));
    TEST_ASSERT_NOT_NULL(strstr(get_song(1)->name1, "Canon"));
    TEST_ASSERT_NOT_NULL(strstr(get_song(2)->name1, "Minuet"));
    TEST_ASSERT_NOT_NULL(strstr(get_song(3)->name1, "Turkish March"));
    TEST_ASSERT_NOT_NULL(strstr(get_song(4)->name1, "Nocturne in E"));
    TEST_ASSERT_NOT_NULL(strstr(get_song(5)->name1, "Waltz"));
    TEST_ASSERT_NOT_NULL(strstr(get_song(6)->name1, "Nocturne in C"));
    TEST_ASSERT_NOT_NULL(strstr(get_song(7)->name1, "Symphony No. 40"));
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_song_count_is_eight);
    RUN_TEST(test_all_songs_have_valid_metadata_and_notes);
    RUN_TEST(test_song_out_of_bounds_returns_null);
    RUN_TEST(test_individual_song_definitions_match_array);
    RUN_TEST(test_song_titles_contain_expected_names);
    return UNITY_END();
}
