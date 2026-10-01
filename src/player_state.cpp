#include "player_state.h"
#include "song_def.h"

void player_init(PlayerContext* ctx) {
    if (!ctx) return;
    ctx->current_state = PLAYER_STATE_STOPPED;
    ctx->previous_state = PLAYER_STATE_STOPPED;
    ctx->current_song_index = 0;
    ctx->candidate_song_index = 0;
    ctx->countdown_seconds = 0;
}

void player_handle_event(PlayerContext* ctx, PlayerEvent evt, uint8_t aux_data) {
    if (!ctx) return;

    switch (evt) {
        case PLAYER_EVENT_USER_BTN:
            if (ctx->current_state == PLAYER_STATE_PLAYING) {
                ctx->previous_state = ctx->current_state;
                ctx->current_state = PLAYER_STATE_PAUSED;
            } else if (ctx->current_state == PLAYER_STATE_PAUSED || ctx->current_state == PLAYER_STATE_STOPPED) {
                ctx->previous_state = ctx->current_state;
                ctx->current_state = PLAYER_STATE_PLAYING;
            } else if (ctx->current_state == PLAYER_STATE_CONFIRMING) {
                // If in confirming state, pressing user button cancels confirmation
                ctx->countdown_seconds = 0;
                ctx->current_state = ctx->previous_state;
            }
            break;

        case PLAYER_EVENT_BTN1_SELECT:
            // aux_data is candidate song index (0..7)
            if (aux_data >= NUM_SONGS) {
                aux_data = NUM_SONGS - 1;
            }
            ctx->previous_state = ctx->current_state;
            ctx->candidate_song_index = aux_data;
            ctx->countdown_seconds = 5;
            ctx->current_state = PLAYER_STATE_CONFIRMING;
            break;

        case PLAYER_EVENT_BTN1_CONFIRM:
            if (ctx->current_state == PLAYER_STATE_CONFIRMING) {
                ctx->current_song_index = ctx->candidate_song_index;
                ctx->countdown_seconds = 0;
                ctx->previous_state = ctx->current_state;
                ctx->current_state = PLAYER_STATE_PLAYING;
            }
            break;

        case PLAYER_EVENT_CONFIRM_TIMEOUT:
            if (ctx->current_state == PLAYER_STATE_CONFIRMING) {
                ctx->countdown_seconds = 0;
                ctx->current_state = ctx->previous_state;
            }
            break;

        case PLAYER_EVENT_TICK_SECOND:
            if (ctx->current_state == PLAYER_STATE_CONFIRMING) {
                if (ctx->countdown_seconds > 0) {
                    ctx->countdown_seconds--;
                    if (ctx->countdown_seconds == 0) {
                        ctx->current_state = ctx->previous_state;
                    }
                }
            }
            break;

        case PLAYER_EVENT_SONG_FINISHED:
            // MP3 player loops current song or continues
            if (ctx->current_state == PLAYER_STATE_PLAYING) {
                // Remains in playing state
            }
            break;

        default:
            break;
    }
}

PlayerState player_get_state(const PlayerContext* ctx) {
    return ctx ? ctx->current_state : PLAYER_STATE_STOPPED;
}

uint8_t player_get_current_song(const PlayerContext* ctx) {
    return ctx ? ctx->current_song_index : 0;
}

uint8_t player_get_candidate_song(const PlayerContext* ctx) {
    return ctx ? ctx->candidate_song_index : 0;
}

uint8_t player_get_countdown(const PlayerContext* ctx) {
    return ctx ? ctx->countdown_seconds : 0;
}

void player_tick_second(PlayerContext* ctx) {
    player_handle_event(ctx, PLAYER_EVENT_TICK_SECOND, 0);
}
