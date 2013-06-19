#ifndef __MOUSE_JUDGE_H__
#define __MOUSE_JUDGE_H__
#include<inc/queue.h>
#include<stddef.h>
#include<inc/curve.h>
/*
#define OBJECT_EQ(__po1, __po2)(((__po1)->pos_x == (__po2)->pos_x)\
		&& ((__po1)->pos_y == (__po2)->pos_y)	\
		&& ((__po1)->type == (__po2)->type))
*/
#define OBJECT_EQ(__po1, __po2)(((__po1)->id == (__po2)->id) &&\
		(__po1)->type == (__po2)->type)
enum object_type{
	object_hit = 0,
	object_slide_start,
	object_slide_end,
	object_slide_tick
};

enum object_score{
	score_300 = 0,
	score_100,
	score_50,
	score_miss,
	score_nonhit
};

struct mouse_judge_t;
struct object_t
{
	TAILQ_ENTRY(object_t) queue_entry;
	float pos_x, pos_y;	// position of the object
	int id;
	enum object_type type;	// type of the object (slider / hit)
	enum object_score score; // target score (300, 100, 50, miss or ignore)
};

TAILQ_HEAD(object_queue_head_t, object_t);

struct mouse_judge_t{
	struct object_queue_head_t *object_queue_array;
	size_t length;
	int redius_hit;
	int redius_slide;
	int threshold_300;
	int threshold_100;
	int threshold_50;
	int threshold_miss;
	int threshold_total;
	float last_rate;
};

struct mouse_judge_t *mouse_judge_init(size_t total_msec, int redius_hit, int redius_slide, int t300, int t100, int t50, int tm);
void mouse_judge_insert_hitobject(struct mouse_judge_t *mouse_judge, int pos_x, int pos_y, size_t time, enum object_type type, int id);
void mouse_judge_insert_slider(struct mouse_judge_t *mouse_judge, struct Dot *p, int dot_num, int repeat, int id);
enum object_score mouse_judge_click(struct mouse_judge_t *mouse_judge, int pos_x, int pos_y, size_t time, int *id_store, enum object_type* type_store);
enum object_score mouse_judge_up(struct mouse_judge_t *mouse_judge, int pos_x, int pos_y, size_t time, int *id_store, enum object_type* type_store);
int mouse_judge_drag(struct mouse_judge_t *mouse_judge, int pos_x, int pos_y, size_t time, int *id_store);
#endif
