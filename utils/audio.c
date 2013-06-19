#include <inc/audio.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define FILE_MP3 0
#define FILE_OGG 1
#define FILE_WAV 2
static void
on_pad_added_ogg (GstElement *element,
              GstPad     *pad,
              gpointer    data)
{
  GstPad *sinkpad;
  GstElement *decoder = (GstElement *) data;

  /* We can now link this pad with the vorbis-decoder sink pad */
  g_print ("Dynamic pad created, linking demuxer/decoder\n");

  sinkpad = gst_element_get_static_pad (decoder, "sink");

  gst_pad_link (pad, sinkpad);

  gst_object_unref (sinkpad);
}

static void
on_pad_added_wav (GstElement *element , GstPad *pad , gpointer data){
 
	gchar *name;
	GstElement *sink = (GstElement*)data;
 
	name = gst_pad_get_name(pad);
	gst_element_link_pads(element , name , sink , "sink");
	g_free(name);
}
static int
file_detect(const char *f)
{
	int len = strlen(f);
	if (!strcasecmp(f + len - 3, "mp3"))
		return FILE_MP3;
	if (!strcasecmp(f + len - 3, "ogg"))
		return FILE_OGG;
	if (!strcasecmp(f + len - 3, "wav"))
		return FILE_WAV;
	return -1;
}
int
load_mp3 (struct sound_t *sound, const char *file)
{
	static char buf[1024];
	static int unique = 0;
	sound->pipeline = gst_pipeline_new (file);
	snprintf(buf, 1024, "%d%s",unique,file);
	sound->source   = gst_element_factory_make ("filesrc", strcat(buf,"file-source"));
	snprintf(buf, 1024, "%d%s",unique,file);
	sound->decoder  = gst_element_factory_make ("mad", strcat(buf, "mad-decoder"));
	snprintf(buf, 1024, "%d%s",unique,file);
	sound->sink     = gst_element_factory_make ("autoaudiosink", strcat(buf, "audio-output"));
	unique++;
	if (!sound->pipeline 
			|| !sound->source 
			|| !sound->decoder 
			|| !sound->sink){
		g_printerr ("One element could not be created. Exiting.\n");
		return -1;
	}
	g_object_set (G_OBJECT (sound->source), "location", file, NULL);
	gst_bin_add_many (GST_BIN (sound->pipeline),
			sound->source, sound->decoder, sound->sink, NULL);
	gst_element_link_many (sound->source, sound->decoder, sound->sink, NULL);
	return 0;

}
int
load_ogg(struct sound_t *sound, const char *file)
{	
	static char buf[1024];
	static int unique = 0;
	sound->pipeline = gst_pipeline_new (file);

	snprintf(buf, 1024, "%d%s",unique,file);
	sound->source = gst_element_factory_make ("filesrc", strcat(buf,"file-source"));
	snprintf(buf, 1024, "%d%s",unique,file);
	sound->demuxer = gst_element_factory_make ("oggdemux", strcat(buf,"ogg-demuxer"));
	snprintf(buf, 1024, "%d%s",unique,file);
	sound->decoder = gst_element_factory_make ("vorbisdec", strcat(buf, "vorbis-decoder"));
	snprintf(buf, 1024, "%d%s",unique,file);
	sound->conv = gst_element_factory_make ("audioconvert", strcat(buf, "converter"));
	snprintf(buf, 1024, "%d%s",unique,file);
	sound->sink = gst_element_factory_make ("autoaudiosink", strcat(buf, "audio-output"));
	unique++;
	if (!sound->pipeline || !sound->source || !sound->demuxer || !sound->decoder || !sound->conv || !sound->sink) {
		g_printerr ("One element could not be created. Exiting.\n");
		return -1;
	}
	g_object_set (G_OBJECT (sound->source), "location", file, NULL);
	gst_bin_add_many (GST_BIN (sound->pipeline),
			sound->source, sound->demuxer, 
			sound->decoder, sound->conv, 
			sound->sink, NULL);
	gst_element_link (sound->source, sound->demuxer);
	gst_element_link_many (sound->decoder, sound->conv, sound->sink, NULL);
	g_signal_connect (sound->demuxer, "pad-added", G_CALLBACK (on_pad_added_ogg), sound->decoder);
}

int
load_wav(struct sound_t *sound, const char *file)
{
	GstElement *pipeline, *source, *parser, *sink;
	static char buf[1024];
	static int unique = 0;
	pipeline = gst_pipeline_new(file);

	snprintf(buf, 1024, "%d%s",unique,file);
	source = gst_element_factory_make("filesrc" , strcat(buf,"source"));
	snprintf(buf, 1024, "%d%s",unique,file);
	parser = gst_element_factory_make("wavparse" , strcat(buf,"parser"));
	snprintf(buf, 1024, "%d%s",unique,file);
	sink = gst_element_factory_make ("autoaudiosink", strcat(buf,"audio-output"));
	unique++;

	if (!source || !parser || !sink)
		return -1;
	g_object_set(G_OBJECT(source) , "location", file, NULL);

	gst_bin_add_many(GST_BIN(pipeline)
			, source , parser , sink , NULL);

	g_signal_connect(parser
			, "pad-added" , G_CALLBACK(on_pad_added_wav) , sink);
	gst_element_link(source , parser);
	sound->pipeline = pipeline;
	sound->parser = parser;
	sound->source = source;
	sound->sink = sink;
	return 0;
}

struct sound_t *
sound_load(const char *file)
{
	struct sound_t *sound = (struct sound_t *)malloc(sizeof (struct sound_t));
	assert(sound);
	memset(sound, 0, sizeof(struct sound_t));
	switch (file_detect(file)){
		case FILE_MP3:
			if (load_mp3(sound, file) < 0)
				return NULL;
			return sound;
		case FILE_OGG:
			if (load_ogg(sound, file) < 0)
				return NULL;
			return sound;
		case FILE_WAV:
			if (load_wav(sound, file) < 0)
				return NULL;
			return sound;
		default:
			return NULL;
	}
}
	inline void
sound_play(struct sound_t *sound)
{
	gst_element_seek (sound->pipeline, 1.0, 
					GST_FORMAT_TIME, GST_SEEK_FLAG_FLUSH,
					GST_SEEK_TYPE_SET, 0,
					GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE);
	gst_element_set_state (sound->pipeline, GST_STATE_PLAYING);
	
}

	inline void
sound_stop(struct sound_t *sound)
{
	gst_element_set_state (sound->pipeline, GST_STATE_NULL);
}
	inline gint64
sound_query_position(struct sound_t *sound)
{
	GstFormat fmt = GST_FORMAT_TIME;
	gint64 pos;
	gst_element_query_position (sound->pipeline, &fmt, &pos);
	return pos;
}
	inline gint64
sound_query_duration(struct sound_t *sound)
{
	GstFormat fmt = GST_FORMAT_TIME;
	gint64 len;
	gst_element_query_duration (sound->pipeline, &fmt, &len);
	return len;
}
	inline void
sound_free(struct sound_t *sound)
{
	gst_object_unref (GST_OBJECT (sound->pipeline));
	free(sound);
}
