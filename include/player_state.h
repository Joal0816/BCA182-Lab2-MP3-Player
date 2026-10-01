#ifndef PLAYER_STATE_H
#define PLAYER_STATE_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    PLAYER_STATE_STOPPED = 0,
    PLAYER_STATE_PLAYING,
    PLAYER_STATE_PAUSED,
    PLAYER_STATE_CONFIRMING
} PlayerState;

typedef enum {
    PLAYER_EVENT_USER_BTN = 0,
    PLAYER_EVENT_BTN1_SELECT,
    PLAYER_EVENT_BTN1_CONFIRM,
    PLAYER_EVENT_CONFIRM_TIMEOUT,
    PLAYER_EVENT_TICK_SECOND,
    PLAYER_EVENT_SONG_FINISHED
} PlayerEvent;

typedef struct {
    PlayerState current_state;
    PlayerState previous_state;
    uint8_t current_song_index;
    uint8_t candidate_song_index;
    uint8_t countdown_seconds;
} PlayerContext;

void player_init(PlayerContext* ctx);
void player_handle_event(PlayerContext* ctx, PlayerEvent evt, uint8_t aux_data);
PlayerState player_get_state(const PlayerContext* ctx);
uint8_t player_get_current_song(const PlayerContext* ctx);
uint8_t player_get_candidate_song(const PlayerContext* ctx);
uint8_t player_get_countdown(const PlayerContext* ctx);
void player_tick_second(PlayerContext* ctx);

#ifdef __cplusplus
}
#endif

#endif /* PLAYER_STATE_H */
