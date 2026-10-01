#ifndef SONG_DEF_H
#define SONG_DEF_H

#include "song.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Basic music notes (period in milliseconds) */
#define So__1      5.102f
#define Si__1      4.059f

#define Do         3.822f
#define DoD        3.608f
#define Re         3.405f
#define ReD        3.214f
#define Mi         3.033f
#define Fa         2.863f
#define FaD        2.702f
#define So         2.551f
#define SoD        2.407f
#define La         2.272f
#define LaD        2.145f
#define Si         2.024f

#define Do_2       1.911f
#define DoD_2      1.803f
#define Re_2       1.702f
#define ReD_2      1.607f
#define Mi_2       1.516f
#define Fa_2       1.431f
#define FaD_2      1.351f
#define So_2       1.275f
#define SoD_2      1.204f
#define La_2       1.136f
#define LaD_2      1.073f
#define Si_2       1.012f

#define Do_3       0.955f
#define DoD_3      0.901f
#define Re_3       0.853f
#define No         0.0f

/* Beat length definitions */
#define b0         1.0f
#define b1         0.5f
#define b2         0.25f
#define b3         0.125f
#define b4         0.075f

#define NUM_SONGS  8

/* 8 Song definitions */
extern const Song FUR_ELISE;
extern const Song CANNON_IN_D;
extern const Song MINUET_IN_G_MAJOR;
extern const Song TURKISH_MARCH;
extern const Song NOCTRUNE_IN_E_FLAT;
extern const Song WALTZ_NO2;
extern const Song NOCTRUNE_IN_C_SHARP_MAJOR;
extern const Song SYMPHONY_NO40;

/* Global songs array and accessors */
extern const Song SONGS[NUM_SONGS];

const Song* get_song(size_t index);
size_t get_song_count(void);

#ifdef __cplusplus
}
#endif

#endif /* SONG_DEF_H */
