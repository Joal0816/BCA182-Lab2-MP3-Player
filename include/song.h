#ifndef SONG_H
#define SONG_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef const float* ptrtoar;

typedef struct Song {
    const char* name1;
    const char* name2;
    ptrtoar note;
    ptrtoar beat;
    float tempo;
    int length;

#ifdef __cplusplus
    constexpr Song() 
        : name1(nullptr), name2(nullptr), note(nullptr), beat(nullptr), tempo(0.0f), length(0) {}

    constexpr Song(const char* in_name1, const char* in_name2, ptrtoar in_note, ptrtoar in_beat, float in_tempo, int in_length)
        : name1(in_name1), name2(in_name2), note(in_note), beat(in_beat), tempo(in_tempo), length(in_length) {}
#endif
} Song;

#ifdef __cplusplus
}
#endif

#endif /* SONG_H */
