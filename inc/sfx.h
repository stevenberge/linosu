#ifndef __SFX_H__
#define __SFX_H__
#include<SDL.h>
#include<SDL_mixer.h>
/* sfx */
struct sfx_t{
	Mix_Chunk *mix_chunk;
};
void sfx_init(void);
void sfx_play_by_type(unsigned int);
void sfx_combo_miss(void);
void sfx_tick(void);
void sfx_destroy(void);
#endif

