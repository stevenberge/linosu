#ifndef __AUDIO_H_
#define __AUDIO_H_
#include <gst/gst.h>
#include <glib.h>
struct sound_t{
GstElement *pipeline,
		   *source,
		   *parser,
		   *demuxer,
		   *decoder,
		   *conv,
		   *sink;
};
struct sound_t *sound_load(const char *file);
gint64 sound_query_position(struct sound_t *sound);
gint64 sound_query_duration(struct sound_t *sound);
void sound_play(struct sound_t *sound);
void sound_stop(struct sound_t *sound);
void sound_free(struct sound_t *sound);
#endif
