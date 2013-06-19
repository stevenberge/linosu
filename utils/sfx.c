#include <inc/sfx.h>
#include <stdio.h>
#include <inc/res.h>
#include <assert.h>
#include <string.h>
enum{
normal_hitnormal = 0,
normal_hitwhistle,
normal_hitfinish,
normal_hitclap,
combobreak,
normal_slidertick,
normal_sliderslide,
normal_sliderwhistle,
SFX_KIND_NUM,
};
static struct sfx_t sfx[SFX_KIND_NUM];
void
sfx_init(void)
{
	int i, j;
	static const char *sfx_file_name[] =
	{
		STR(MATERIAL_DIR/normal-hitnormal.wav),
		STR(MATERIAL_DIR/normal-hitwhistle.wav),
		STR(MATERIAL_DIR/normal-hitfinish.wav),
		STR(MATERIAL_DIR/normal-hitclap.wav),
		STR(MATERIAL_DIR/combobreak.wav),
		STR(MATERIAL_DIR/normal-slidertick.wav),
		STR(MATERIAL_DIR/normal-sliderslide.wav),
		STR(MATERIAL_DIR/normal-sliderwhistle.wav),
	};
	if (SDL_Init(SDL_INIT_AUDIO) != 0) {
		fprintf(stderr, "Unable to initialize SDL: %s\n", SDL_GetError());
		exit(1);
	}

	int audio_rate = 44100;
	Uint16 audio_format = AUDIO_S16SYS;
	int audio_channels = 2;
	int audio_buffers = 256;

	if(Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers) != 0) {
		fprintf(stderr, "Unable to initialize audio: %s\n", Mix_GetError());
		exit(1);
	}
	for (i = 0; i < SFX_KIND_NUM; i++){
		sfx[i].mix_chunk = Mix_LoadWAV(sfx_file_name[i]);
		Mix_VolumeChunk(sfx[i].mix_chunk, MIX_MAX_VOLUME/2.5);
		if (!sfx[i].mix_chunk)
			fprintf(stderr, "Unable to load WAV file: %s\n", Mix_GetError());
	}
	Mix_AllocateChannels(11);
}
void 
sfx_combo_miss(void)
{
	Mix_PlayChannel(-1, sfx[combobreak].mix_chunk, 0);
	//printf("%d\n",Mix_PlayChannel(-1, sfx[combobreak].mix_chunk, 0));
}
void 
sfx_tick(void)
{
	//Mix_PlayChannel(-1, sfx[normal_slidertick].mix_chunk, 0);
	//printf("%d\n",Mix_PlayChannel(-1, sfx[normal_slidertick].mix_chunk, 0));
}
void
sfx_play_by_type(unsigned int sound_type)
{
	int i;
	Mix_PlayChannel(-1, sfx[normal_hitnormal].mix_chunk, 0);
	//printf("%d\n",Mix_PlayChannel(-1, sfx[normal_hitnormal].mix_chunk, 0));
	for (i = 1; i < SFX_KIND_NUM; i ++){
		if (sound_type & (1 << i)){
			Mix_PlayChannel(-1, sfx[i].mix_chunk, 0);
		}
	}

}
void
sfx_destroy(void)
{
	int i;
	for(i = 0; i <SFX_KIND_NUM; i ++){
		Mix_FreeChunk(sfx[i].mix_chunk);
	}
		Mix_CloseAudio();
		SDL_Quit();
}

