#ifndef __CELL_H__
#define __CELL_H__
#include <inc/queue.h>
#include <GL/glut.h>
#include <inc/time_seq.h>
#include <inc/parser.h>
#include <inc/curve.h>
/*
 * Project Linosu!
 * data structure for cell, use structures in time_seq.h.
 *
 * Author: Kerry Wang (kerrywang881122@gmail.com)
 * History: 4/30/2011: first version.
 */

struct Background{
	//public part
	struct event_t event_header;
};

struct Approach{
	//pulic part
	struct event_t event_header;

	//other part
	float px, py;
	float current_zoomrate;
	GLuint *tex_graph;
};

struct Hitcircle{
	//pulic part
	struct event_t event_header;
	//other part
	float px, py;
	GLuint *tex_graph;
};

struct Circlenumber{
	//pulic part
	struct event_t event_header;
	//other part
	float px, py;
	GLuint *tex_graph;
};

struct Slider{
	//pulic part 实现多态的关键部分
	struct event_t event_header;
	//other part
	struct Dot *points;
	int max_len;
	int reverse_time;
	int color_index;
	int sound_type;
};

/**
 * a score cell. print the score;
 */
struct Hitscore{
	//public part 实现对齐（event_t)
	struct event_t event_header;
	//other part
	float px, py;
	GLuint *tex_graph;
	float current_zoomrate; // use to make a coming out of a score
};

/**
 * a slider ball.
 */
struct Sliderball{
	//public part
	struct event_t event_header;
	//other part
	float px, py;
	GLuint *tex_graph;
	float dx, dy;
	int sound_type;
	int ever_played;
};


/*
 * constant in the painting
 * modified: time_interval is now not used. time seq is now
 * created as the interval is 1, i.e the term i in the seq is the 
 * event at i ms.
 */
#define TIME_INTERVAL 1  // 30 fps
#define CIRCLE_CLOSE_TIME 800 // 0.5 second for closing
#define APPROACH_INIT_ZOOMRATE 3 // initial zoom rate of approach circle
#define HITSCORE_INIT_ZOOMRATE 0.3 // init zoom rate of a hit score
#define HITSCORE_APPEAR_TIME 100
#define HITCIRCLE_EXIST_TIME 1000 // hitcircle exists 1 second
#define HITSCORE_EXIST_TIME 500 // hit score exist time


// a definition of score texture, use to index the array
enum
{
	HIT_50 = 0,
	HIT_100,
	HIT_100k,
	HIT_300,
	HIT_300k,
	HIT_300g,
	HIT_miss,
	HIT_30
};


/**
 * Functions to initialize cells and cell-triggers
 */
struct Approach *new_approach(float x, float y, GLuint *text);
void approach_trigger(void *self, struct time_seq_t *time_seq, size_t time);
struct Hitcircle *__attribute__((optimize("O0"))) new_hitcircle(float x, float y, GLuint *text, int with_overLay);
void hitcircle_trigger(void *self, struct time_seq_t *time_seq, size_t time);
void hitcircle_trigger_no_overlay(void *self, struct time_seq_t *time_seq, size_t time);
struct Circlenumber *__attribute__((optimize("O0"))) new_circlenumber(float x, float y, GLuint *text);
void circlenumber_trigger(void *self, struct time_seq_t *time_seq, size_t time);
struct Hitscore *__attribute__((optimize("O0"))) new_hitscore(float x, float y, GLuint *text);
void hitscore_trigger(void *self, struct time_seq_t *time_seq, size_t time);
struct Sliderball *new_sliderball(float x, float y, GLuint *text, float dx, float dy, int sound_type);
void sliderball_trigger(void *self, struct time_seq_t *time_seq, size_t time);
struct Slider *__attribute__((optimize("O0"))) new_slider(struct Dot* points, int len, int color_index, int reverse_time);
void slider_trigger(void *self, struct time_seq_t *time_seq, size_t time);
struct Background* __attribute__((optimize("O0"))) new_background();
void background_trigger(void *self, struct time_seq_t *time_seq, size_t time);


/**
 * Functions to fulfill the time_seq with the data read from the .osu file
 *
 * color_index is the index of the hitcircle_tex array and color array
 */
void create_hitcircle_event(float x, float y, int color_index, struct time_seq_t *time_seq, size_t time, size_t last_time, int with_overlay);
void create_approach_event(float x, float y, int color_index, struct time_seq_t *time_seq, size_t time);
void create_circlenumber_event(float x, float y, int number, struct time_seq_t *time_seq, size_t time, size_t last_time);
void create_hitscore_event(float x, float y, int score, struct time_seq_t *time_seq, size_t time);
void create_sliderball_event(float x, float y, int index, struct time_seq_t *time_seq, size_t time, size_t last_time, float dx, float dy, int sound_type);
void cell_init();

void create_slider(struct Dot *points, int len, struct time_seq_t * time_seq, int color_index, int number, int reverse_time, int sound_type);
//this is different from create_sliderball_event
void create_slider_ball(struct Dot *points, int len, struct time_seq_t *time_seq, int scan_time, int sound_type);
void create_background(char *background, struct time_seq_t *time_seq, int max_time);
#endif
