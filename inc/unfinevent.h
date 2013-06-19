#ifndef __UNFINEVENT_H__
#define __UNFINEVENT_H__
#include <inc/queue.h>
#include <GL/glut.h>
#include <inc/time_seq.h>
#include <inc/cell.h>
#include <inc/mouse_judge.h>
/**
 * Project Linosu!
 * data structure for unfinished event. use time_seq.h
 * and functions in cell.h
 * Author: Kerry Wang
 */

struct UnfinEvent
{
	int finish_time;
	float px;
	float py;
	int finished;
	int sound_type;
	int object_type;
};


/**
 * function for unfinished events array
 *
 */
void add_unfin_event(struct UnfinEvent *u_event, int id, float x, float y, int time, int sound_type, int object_type);
void finish_event(struct UnfinEvent *u_event, int id, int score, int time, int success, struct time_seq_t *time_seq); 
void kill_unfin_event_before(struct UnfinEvent *u_event, int event_num, int time, struct time_seq_t *time_seq);
void set_current_tick_time(struct UnfinEvent *u_event, int event_num, int time);

#define EVENT_SUCCESS 1
#define EVENT_FAIL 0
#endif
