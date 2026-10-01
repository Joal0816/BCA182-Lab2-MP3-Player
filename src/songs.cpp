#include "song_def.h"

// Für Elise - Beethoven (length: 72)
static const float Fur_Elise_note[] = {
    Mi_2, ReD_2, Mi_2, ReD_2, Mi_2, Si, Re_2, Do_2,
    La, No, Do, Do, Mi, La,
    Si, No, Mi, Mi, SoD, Si,
    Do_2, No, Mi, Mi, Mi_2, ReD_2,
    Mi_2, ReD_2, Mi_2, Si, Re_2, Do_2,
    La, No, Do, Do, Mi, La,
    Si, No, Mi, Mi, Do_2, Si,
    La, No, Si, Si, Do_2, Re_2,
    Mi_2, No, So, So, Fa_2, Mi_2,
    Re_2, No, Fa, Fa, Mi_2, Re_2,
    Do_2, No, Mi, Mi, Re_2, Do_2,
    Si, Mi, Mi_2, ReD_2
};

static const float Fur_Elise_beat[] = {
    b3, b3, b3, b3, b3, b3, b3, b3,
    b2, b3, b4, b3, b3, b3,
    b2, b3, b4, b3, b3, b3,
    b2, b3, b4, b3, b3, b3,
    b3, b3, b3, b3, b3, b3,
    b2, b3, b4, b3, b3, b3,
    b2, b3, b4, b3, b3, b3,
    b2, b3, b4, b3, b3, b3,
    b2, b3, b4, b3, b3, b3,
    b2, b3, b4, b3, b3, b3,
    b2, b3, b4, b3, b3, b3,
    b2, b1, b1, b3
};

static const float Fur_Elise_tempo = 0.18f;

// Canon In D - Pachelbel (length: 88)
static const float Canon_In_D_note[] = {
    FaD_2, DoD_2, Re_2, FaD, Mi_2, La, So, La,
    Re_2, No, Re_2, DoD_2, Si, DoD_2, FaD_2, La_2, Si_2,
    So_2, FaD_2, Mi_2, So_2, FaD_2, Mi_2, Re_2, DoD_2,
    Si, La, Si, Re_2, No, Re_2, DoD_2,
    La_2, FaD_2, So_2, La_2, FaD_2, So_2, La_2, La, Si, DoD_2, Re_2, Mi_2, FaD_2, So_2,
    FaD_2, Re_2, Mi_2, FaD_2, FaD, So, La, Si, La, So, La, FaD, So, La,
    So, Si, La, So, FaD, Mi, FaD, Mi, Re, Mi, FaD, So, La, Si,
    So, Si, La, Si, DoD_2, Re_2, DoD_2, Si, La, Si, DoD_2, Re_2, Mi_2, FaD_2
};

static const float Canon_In_D_beat[] = {
    b2, b2, b2, b2, b2, b2, b2, b2,
    b3, b4, b2, b2, b2, b2, b2, b2, b2,
    b2, b2, b2, b2, b2, b2, b2, b2,
    b2, b2, b2, b3, b4, b1, b1,
    b2, b3, b3, b2, b3, b3, b3, b3, b3, b3, b3, b3, b3, b3,
    b2, b3, b3, b2, b3, b3, b3, b3, b3, b3, b3, b3, b3, b3,
    b2, b3, b3, b2, b3, b3, b3, b3, b3, b3, b3, b3, b3, b3,
    b2, b3, b3, b2, b3, b3, b3, b3, b3, b3, b3, b3, b3, b3
};

static const float Canon_In_D_tempo = 0.41f;

// Minuet in G major - Bach (length: 90)
static const float Minuet_In_G_major_note[] = {
    Re_2, Re_2, So, La, Si, Do_2,
    Re_2, Re_2, So, No, So, No,
    Mi_2, Mi_2, Do_2, Re_2, Mi_2, FaD_2,
    So_2, So_2, So, No, So, No,
    Do_2, Do_2, Re_2, Do_2, Si, La,
    Si, Si, Do_2, Si, La, So,
    FaD, FaD, So, La, Si, So,
    Si, La, La,
    Re_2, Re_2, So, La, Si, Do_2,
    Re_2, Re_2, So, No, So, No,
    Mi_2, Mi_2, Do_2, Re_2, Mi_2, FaD_2,
    So_2, So_2, So, No, So, No,
    Do_2, Do_2, Re_2, Do_2, Si, La,
    Si, Si, Do_2, Si, La, So,
    La, La, Si, La, So, FaD,
    So, So, No
};

static const float Minuet_In_G_major_beat[] = {
    b1, b4, b2, b2, b2, b2,
    b1, b4, b1, b4, b1, b4,
    b1, b4, b2, b2, b2, b2,
    b1, b4, b1, b4, b1, b4,
    b1, b4, b2, b2, b2, b2,
    b1, b4, b2, b2, b2, b2,
    b1, b4, b2, b2, b2, b2,
    b2, b1, b1,
    b1, b4, b2, b2, b2, b2,
    b1, b4, b1, b4, b1, b4,
    b1, b4, b2, b2, b2, b2,
    b1, b4, b1, b4, b1, b4,
    b1, b4, b2, b2, b2, b2,
    b1, b4, b2, b2, b2, b2,
    b1, b4, b2, b2, b2, b2,
    b1, b1, b1
};

static const float Minuet_In_G_major_tempo = 0.13f;

// Turkish March - Mozart (length: 176)
static const float Turkish_March_note[] = {
    Si, La, SoD, La,
    Do_2, Do_2, No, Re_2, Do_2, Si, Do_2,
    Mi_2, Mi_2, No, Fa_2, Mi_2, ReD_2, Mi_2,
    Si_2, La_2, SoD_2, La_2, Si_2, La_2, SoD_2, La_2,
    Do_3, Do_3, No, La_2, No, Do_3, No,
    Si_2, No, La_2, No, So_2, No, La_2, No,
    Si_2, No, La_2, No, So_2, No, La_2, No,
    Si_2, No, La_2, No, So_2, No, FaD_2, No,
    Mi_2, No, Mi_2, No, Fa_2, No,
    So_2, No, So_2, No, La_2, So_2, Fa_2, Mi_2,
    Re_2, Re_2, No, Mi_2, No, Fa_2, No,
    So_2, No, So_2, No, La_2, So_2, Fa_2, Mi_2,
    Re_2, Re_2, No, Do_2, No, Re_2, No,
    Mi_2, No, Mi_2, No, Fa_2, Mi_2, Re_2, Do_2,
    Si, Si, No, Do_2, No, Re_2, No,
    Mi_2, No, Mi_2, No, Fa_2, Mi_2, Re_2, Do_2,
    Si, Si, No, Si, La, SoD, La,
    Do_2, Do_2, No, Re_2, Do_2, Si, Do_2,
    Mi_2, Mi_2, No, Fa_2, Mi_2, ReD_2, Mi_2,
    Si_2, La_2, SoD_2, La_2, Si_2, La_2, SoD_2, La_2,
    Do_3, Do_3, No, La_2, No, Si_2, No,
    Do_3, No, Si_2, No, La_2, No, SoD_2, No,
    La_2, No, Mi_2, No, Fa_2, No, Re_2, No,
    Do_2, Do_2, No, Si, La, Si,
    La, La
};

static const float Turkish_March_beat[] = {
    b3, b3, b3, b3,
    b2, b2, b2, b3, b3, b3, b3,
    b2, b2, b2, b3, b3, b3, b3,
    b3, b3, b3, b3, b3, b3, b3, b3,
    b2, b2, b2, b3, b3, b3, b3,
    b3, b3, b3, b3, b3, b3, b3, b3,
    b3, b3, b3, b3, b3, b3, b3, b3,
    b3, b3, b3, b3, b3, b3, b3, b3,
    b1, b1, b3, b3, b3, b3,
    b3, b3, b3, b3, b3, b3, b3, b3,
    b2, b2, b2, b3, b3, b3, b3,
    b3, b3, b3, b3, b3, b3, b3, b3,
    b2, b2, b2, b3, b3, b3, b3,
    b3, b3, b3, b3, b3, b3, b3, b3,
    b2, b2, b2, b3, b3, b3, b3,
    b3, b3, b3, b3, b3, b3, b3, b3,
    b2, b2, b2, b3, b3, b3, b3,
    b2, b2, b2, b3, b3, b3, b3,
    b2, b2, b2, b3, b3, b3, b3,
    b3, b3, b3, b3, b3, b3, b3, b3,
    b2, b2, b2, b3, b3, b3, b3,
    b3, b3, b3, b3, b3, b3, b3, b3,
    b3, b3, b3, b3, b3, b3, b3, b3,
    b2, b2, b2, b2, b3, b3,
    b1, b1
};

static const float Turkish_March_tempo = 0.15f;

// Nocturne in E flat - Chopin (length: 116)
static const float Nocturne_in_E_flat_note[] = {
    LaD, So_2, So_2, So_2, So_2, Fa_2, So_2,
    Fa_2, Fa_2, Fa_2, ReD_2, ReD_2, LaD,
    So_2, So_2, Do_2, Do_3, Do_3, So_2,
    LaD_2, LaD_2, LaD_2, SoD_2, SoD_2, So_2,
    Fa_2, Fa_2, Fa_2, So_2, So_2, Re_2,
    ReD_2, ReD_2, ReD_2, Do_2, Do_2, Do_2,
    LaD, Re_3, Do_3, LaD_2, SoD_2, So_2, SoD_2, Do_2, Re_2,
    ReD_2, ReD_2, ReD_2, No, No, LaD,
    So_2, So_2, So_2, Fa_2, So_2, Fa_2, Mi_2, Fa_2, So_2,
    Fa_2, ReD_2, ReD_2, ReD_2, Fa_2, ReD_2, Re_2, ReD_2, Fa_2,
    So_2, Si, Do_2, DoD_2, Do_2, Fa_2,
    Mi_2, SoD_2, So_2, DoD_3, Do_3, So_2,
    LaD_2, LaD_2, LaD_2, SoD_2, SoD_2, So_2,
    Fa_2, Fa_2, Mi_2, Fa_2, So_2, So_2, Re_2,
    ReD_2, ReD_2, ReD_2, Do_2, Do_2, Do_2,
    LaD, Re_3, Do_3, LaD_2, SoD_2, So_2, SoD_2, Do_2, Re_2,
    ReD_2, ReD_2, ReD_2, No, No, No
};

static const float Nocturne_in_E_flat_beat[] = {
    b0, b0, b0, b0, b0, b0, b0,
    b0, b0, b0, b0, b0, b0,
    b0, b0, b0, b0, b0, b0,
    b0, b0, b0, b0, b0, b0,
    b0, b0, b0, b0, b0, b0,
    b0, b0, b0, b0, b0, b0,
    b1, b1, b1, b1, b1, b1, b1, b1, b1,
    b0, b0, b0, b0, b0, b0,
    b0, b0, b0, b1, b1, b1, b1, b1, b1,
    b0, b0, b0, b1, b1, b1, b1, b1, b1,
    b1, b1, b1, b1, b1, b1,
    b1, b1, b1, b1, b1, b1,
    b0, b0, b0, b0, b0, b0,
    b0, b0, b1, b1, b0, b0, b0,
    b0, b0, b0, b0, b0, b0,
    b1, b1, b1, b1, b1, b1, b1, b1, b1,
    b0, b0, b0, b0, b0, b0
};

static const float Nocturne_in_E_flat_tempo = 0.31f;

// Waltz No. 2 - Shostakovich (length: 135)
static const float Waltz_No2_note[] = {
    So, So, So, ReD, ReD, Re,
    Do, Do, Do, No, Do, Re,
    ReD, Do, ReD, So, So, SoD,
    So, So, So, Fa, Fa, Fa,
    No, Fa, Fa, Fa, Re, Re, Do,
    Si__1, Si__1, Si__1, Si__1, So__1, Si__1,
    Re, Si__1, Re, Fa, So, SoD,
    FaD, FaD, FaD, So, So, So,
    ReD_2, ReD_2, ReD_2, Re_2, Re_2, Do_2,
    LaD, LaD, SoD, Fa, Fa, Fa,
    Re_2, Re_2, Re_2, Do_2, Do_2, LaD,
    No, LaD, LaD, LaD, So, So, So,
    No, ReD, No, Fa, No,
    So, No, So, No, Fa, No, So, No, SoD, No,
    Fa, No, Fa, No, ReD, No, Fa, No, So, No,
    ReD, No, No, So, No,
    No, Do_2, No, Re_2, No,
    ReD_2, No, ReD_2, No, Re_2, No, ReD_2, No, Fa_2, No,
    Re_2, No, Re_2, No, Do_2, No, Re_2, No, ReD_2, No,
    Do_2, Do_2, Do_2, No, No, No
};

static const float Waltz_No2_beat[] = {
    b1, b1, b1, b1, b1, b1,
    b1, b1, b1, b1, b1, b1,
    b1, b1, b1, b1, b1, b1,
    b1, b1, b1, b1, b1, b1,
    b4, b1, b1, b1, b1, b1, b1,
    b1, b1, b1, b1, b1, b1,
    b1, b1, b1, b1, b1, b1,
    b1, b1, b1, b1, b1, b1,
    b1, b1, b1, b1, b1, b1,
    b1, b1, b1, b1, b1, b1,
    b1, b1, b1, b1, b1, b1,
    b4, b1, b1, b1, b1, b1, b1,
    b1, b2, b2, b2, b2,
    b2, b3, b3, b3, b3, b3, b3, b3, b3, b3,
    b2, b2, b3, b3, b3, b3, b3, b3, b3, b3,
    b2, b2, b1, b2, b2,
    b1, b2, b2, b2, b2,
    b2, b2, b3, b3, b3, b3, b3, b3, b3, b3,
    b2, b2, b3, b3, b3, b3, b3, b3, b3, b3,
    b1, b1, b1, b1, b1, b1
};

static const float Waltz_No2_tempo = 0.1f;

// Nocturne in C sharp minor - Chopin (length: 64)
static const float Nocturne_in_C_sharp_minor_note[] = {
    SoD_2, SoD_2, FaD_2, SoD_2, FaD_2, SoD_2, FaD_2, SoD_2,
    FaD_2, SoD_2, FaD_2, SoD_2, FaD_2, SoD_2, FaD_2, SoD_2,
    FaD_2, SoD_2, Mi_2, FaD_2, SoD_2, DoD_2,
    DoD_3, DoD_3, DoD_3, Si_2, DoD_3, Si_2, La_2,
    No, La_2, No, SoD_2, ReD_2,
    Mi_2, FaD_2, DoD_2, DoD_2,
    No, DoD_2, ReD_2, No, ReD_2, Mi_2, ReD_2, Mi_2,
    ReD_2, Mi_2, ReD_2, Mi_2, ReD_2, Mi_2, ReD_2, Mi_2,
    ReD_2, Mi_2, ReD_2, Mi_2, ReD_2, DoD_2,
    ReD_2, SoD, SoD, No
};

static const float Nocturne_in_C_sharp_minor_beat[] = {
    b0, b0, b4, b4, b4, b4, b4, b4,
    b4, b4, b4, b4, b4, b4, b4, b4,
    b4, b4, b3, b3, b0, b0,
    b0, b3, b4, b4, b3, b3, b3,
    b4, b0, b0, b0, b0,
    b0, b1, b1, b1,
    b4, b2, b2, b2, b4, b4, b4, b4,
    b4, b4, b4, b4, b4, b4, b4, b4,
    b4, b4, b4, b4, b2, b2,
    b0, b0, b0, b0
};

static const float Nocturne_in_C_sharp_minor_tempo = 0.13f;

// Symphony No. 40 - Mozart (length: 168)
static const float Symphony_No40_note[] = {
    ReD_2, No, Re_2, No,
    Re_2, No, ReD_2, No, Re_2, No, Re_2, No, ReD_2, No, Re_2, No,
    Re_2, No, LaD_2, No, No, LaD_2, No, La_2, No,
    So_2, No, So_2, No, Fa_2, No, ReD_2, No, ReD_2, No, Re_2, No,
    Do_2, No, Do_2, No, No, Re_2, No, Do_2, No,
    Do_2, No, Re_2, No, Do_2, No, Do_2, No, Re_2, No, Do_2, No,
    Do_2, No, La_2, No, No, La_2, No, So_2, No,
    FaD_2, No, FaD_2, No, ReD_2, No, Re_2, No, Re_2, No, Do_2, No,
    LaD, No, LaD, No, No, LaD_2, No, La_2, No,
    La_2, No, Do_3, No, FaD_2, No, La_2, No,
    So_2, No, Re_2, No, No, LaD_2, No, La_2, No,
    La_2, No, Do_3, No, FaD_2, No, La_2, No,
    So_2, No, LaD_2, No, La_2, No, So_2, Fa_2, No, ReD_2, No,
    No, FaD_2, No, So_2, No, La_2, No,
    LaD_2, No, Do_3, No, LaD_2, No, La_2, No, So_2, No,
    FaD_2, No, No, DoD_3, No,
    Re_3, No, No, DoD_3, No,
    Re_3, No, No, DoD_3, No,
    Re_3, No, DoD_3, No, Re_3, No, DoD_3, No,
    Re_3, Re_3, No, No
};

static const float Symphony_No40_beat[] = {
    b3, b3, b3, b3,
    b2, b2, b3, b3, b3, b3, b2, b2, b3, b3, b3, b3,
    b2, b2, b2, b2, b1, b3, b3, b3, b3,
    b2, b2, b3, b3, b3, b3, b2, b2, b3, b3, b3, b3,
    b2, b2, b2, b2, b1, b3, b3, b3, b3,
    b2, b2, b3, b3, b3, b3, b2, b2, b3, b3, b3, b3,
    b2, b2, b2, b2, b1, b3, b3, b3, b3,
    b2, b2, b3, b3, b3, b3, b2, b2, b3, b3, b3, b3,
    b2, b2, b2, b2, b1, b3, b3, b3, b3,
    b2, b2, b2, b2, b2, b2, b2, b2,
    b2, b2, b2, b2, b1, b3, b3, b3, b3,
    b2, b2, b2, b2, b2, b2, b2, b2,
    b2, b2, b2, b2, b3, b3, b3, b3, b3, b3, b3, b3,
    b1, b2, b2, b2, b2, b2,
    b2, b2, b3, b3, b3, b3, b2, b2, b2, b2,
    b2, b2, b1, b1, b1,
    b2, b2, b1, b1, b1,
    b2, b2, b1, b1, b1,
    b2, b2, b2, b2, b2, b2, b2, b2,
    b1, b1, b1, b0
};

static const float Symphony_No40_tempo = 0.1f;

// 8 Song objects
const Song FUR_ELISE("Fur Elise -", "Beethoven", Fur_Elise_note, Fur_Elise_beat, Fur_Elise_tempo, 72);
const Song CANNON_IN_D("Canon In D - ", "Pachebelbel", Canon_In_D_note, Canon_In_D_beat, Canon_In_D_tempo, 88);
const Song MINUET_IN_G_MAJOR("Minuet in G", "major - Bach", Minuet_In_G_major_note, Minuet_In_G_major_beat, Minuet_In_G_major_tempo, 90);
const Song TURKISH_MARCH("Turkish March - ", " Mozart", Turkish_March_note, Turkish_March_beat, Turkish_March_tempo, 176);
const Song NOCTRUNE_IN_E_FLAT("Nocturne in E ", "flat -Chopin", Nocturne_in_E_flat_note, Nocturne_in_E_flat_beat, Nocturne_in_E_flat_tempo, 116);
const Song WALTZ_NO2("Waltz No. 2 - ", "Shostakovich", Waltz_No2_note, Waltz_No2_beat, Waltz_No2_tempo, 135);
const Song NOCTRUNE_IN_C_SHARP_MAJOR("Nocturne in C ", "sharp - Chopin", Nocturne_in_C_sharp_minor_note, Nocturne_in_C_sharp_minor_beat, Nocturne_in_C_sharp_minor_tempo, 64);
const Song SYMPHONY_NO40("Symphony No. 40 ", "- Mozart", Symphony_No40_note, Symphony_No40_beat, Symphony_No40_tempo, 168);

// Song array containing all 8 songs in index order 0 to 7
const Song SONGS[NUM_SONGS] = {
    FUR_ELISE,
    CANNON_IN_D,
    MINUET_IN_G_MAJOR,
    TURKISH_MARCH,
    NOCTRUNE_IN_E_FLAT,
    WALTZ_NO2,
    NOCTRUNE_IN_C_SHARP_MAJOR,
    SYMPHONY_NO40
};

const Song* get_song(size_t index) {
    if (index < NUM_SONGS) {
        return &SONGS[index];
    }
    return nullptr;
}

size_t get_song_count(void) {
    return NUM_SONGS;
}
