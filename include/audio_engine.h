#ifndef AUDIO_ENGINE_H
#define AUDIO_ENGINE_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

void audio_engine_init(void);
void audio_engine_play_song(uint8_t song_index);
void audio_engine_pause(void);
void audio_engine_resume(void);
void audio_engine_stop(void);
void audio_engine_set_volume(uint8_t volume_percent);
uint8_t audio_engine_get_volume(void);
bool audio_engine_is_playing(void);
uint16_t audio_engine_get_current_note_index(void);

#ifdef __cplusplus
}
#endif

#endif /* AUDIO_ENGINE_H */
