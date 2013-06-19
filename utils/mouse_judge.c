#include <inc/mouse_judge.h>

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>

static void 
mouse_judge_map(struct mouse_judge_t *mouse_judge, void (* func) (struct object_queue_head_t *))
{
	int i;
	for (i = 0; i < mouse_judge->length; i++)
		func(&(mouse_judge->object_queue_array[i]));
}

static void
mouse_judge_init_mapper(struct object_queue_head_t *queue)
{
	queue->tqh_first = NULL;
	queue->tqh_last = &(queue->tqh_first);
	TAILQ_INIT(queue);
}

struct mouse_judge_t *
mouse_judge_init(size_t total_msec, int redius_hit, int redius_slide, int t300, int t100, int t50, int tm)
{
	struct object_queue_head_t *ptr;
	struct mouse_judge_t *rtn;
	rtn = (struct mouse_judge_t *)malloc(sizeof(struct mouse_judge_t));
	ptr = (struct object_queue_head_t *)malloc(sizeof(struct object_queue_head_t) * total_msec);
	rtn -> object_queue_array = ptr;
	rtn -> length = total_msec;
	
	rtn -> redius_hit = redius_hit;
	rtn -> redius_slide = redius_slide;

	rtn -> threshold_300 = t300;
	rtn -> threshold_100 = t100;
	rtn -> threshold_50 = t50;
	rtn -> threshold_miss = tm;

	rtn -> threshold_total = (rtn -> threshold_300 
			+ rtn->threshold_100
			+ rtn->threshold_50
			+ rtn->threshold_miss
			+ (int)(rtn->threshold_300 * (rtn->last_rate))
			+ (int)(rtn->threshold_100 * (rtn->last_rate))
			+ (int)(rtn->threshold_50  * (rtn->last_rate))
			+ (int)(rtn->threshold_miss* (rtn->last_rate))
			+ 1);
	//initialize all the queues
	rtn -> last_rate = 0.3;
	mouse_judge_map(rtn, mouse_judge_init_mapper);
	return rtn;
}
static inline struct object_t * 
object_init(float pos_x, float pos_y, enum object_type type, enum object_score score, int id)
{
	struct object_t *o;
	o = (struct object_t *)malloc(sizeof(struct object_t));
	assert(o);
	o->pos_x  = pos_x;
	o->pos_y  = pos_y;
	o->type   = type;
	o-> score = score;
	o-> id    = id;
	return o;
}

void 
mouse_judge_insert_hitobject(struct mouse_judge_t *mouse_judge, int pos_x, int pos_y, size_t time, enum object_type type, int id)
{
	int prev = time - 1, fwd = time + 1, i;
	
	struct object_t *po;
	int threshold_300 = mouse_judge->threshold_300,
		threshold_100 = mouse_judge->threshold_100,
		threshold_50  = mouse_judge->threshold_50,
		threshold_miss= mouse_judge->threshold_miss,
		last_rate     = mouse_judge->last_rate,
		length        = mouse_judge->length;
	po = object_init(pos_x, pos_y, type, score_300, id);
	TAILQ_INSERT_TAIL(mouse_judge->object_queue_array + time, po, queue_entry);
	//insert 300
	for (i = 0; i < threshold_300; i++)
	{
		if (prev > 0){
			po = object_init(pos_x, pos_y, type, score_300, id);
			TAILQ_INSERT_TAIL(mouse_judge->object_queue_array + prev, po, queue_entry);
		}
		if (fwd < length && fwd < (int)(threshold_300 * last_rate)){
			po = object_init(pos_x, pos_y, type, score_300, id);
			TAILQ_INSERT_TAIL(mouse_judge->object_queue_array + fwd, po, queue_entry);
		}
		prev--, fwd++;
	}
	//insert 100
	for (i = 0; i < threshold_100; i++)
	{
		if (prev > 0){
			po = object_init(pos_x, pos_y, type, score_100, id);
			TAILQ_INSERT_TAIL(mouse_judge->object_queue_array + prev, po, queue_entry);
		}
		if (fwd < length && fwd < (int)(threshold_100 * last_rate)){
			po = object_init(pos_x, pos_y, type, score_100, id);
			TAILQ_INSERT_TAIL(mouse_judge->object_queue_array + fwd, po, queue_entry);
		}
		prev--, fwd++;
	}
	//insert 50
	for (i = 0; i < threshold_50; i++)
	{
		if (prev > 0){
			po = object_init(pos_x, pos_y, type, score_50, id);
			TAILQ_INSERT_TAIL(mouse_judge->object_queue_array + prev, po, queue_entry);
		}
		if (fwd < length && fwd < (int)(threshold_50 * last_rate)){
			po = object_init(pos_x, pos_y, type, score_50, id);
			TAILQ_INSERT_TAIL(mouse_judge->object_queue_array + fwd, po, queue_entry);
		}
		prev--, fwd++;
	}
	//insert miss
	for (i = 0; i < threshold_miss; i++)
	{
		if (prev > 0){
			po = object_init(pos_x, pos_y, type, score_miss, id);
			TAILQ_INSERT_TAIL(mouse_judge->object_queue_array + prev, po, queue_entry);
		}
		if (fwd < length && fwd < (int)(threshold_miss * last_rate)){
			po = object_init(pos_x, pos_y, type, score_miss, id);
			TAILQ_INSERT_TAIL(mouse_judge->object_queue_array + fwd, po, queue_entry);
		}
		prev--, fwd++;
	}


}
static inline int 
click_judge(float x1, float y1, float x2, float y2, float redius)
{
	return (x1-x2)*(x1-x2) + (y1-y2)*(y1-y2) < redius * redius;
}

enum object_score 
mouse_judge_click(struct mouse_judge_t *mouse_judge, int pos_x, int pos_y, size_t time, int *id_store, enum object_type *type_store)
{
	struct object_t  *po, *mpo, *tpo;

	struct object_queue_head_t *queue_head = mouse_judge->object_queue_array + time;
	int find = 0, i;
	enum object_score score;

	TAILQ_FOREACH(po, queue_head, queue_entry){
		if (po->type <= object_slide_start &&
				click_judge(po->pos_x, po->pos_y, 
					pos_x, pos_y, 
					mouse_judge->redius_hit)){
			find = 1;
			break;
		}
	}
	if (!find)
		return score_nonhit;
	if (id_store)
		*id_store = po->id;
	if (type_store)
		*type_store = po->type;
	TAILQ_REMOVE(queue_head, po, queue_entry);
	//search all the queue for object equal to po
	//(time +- 2*total_threshold), and delete them!
	for (i = 0; i < mouse_judge->threshold_total && time + i < mouse_judge->length; i++)
	{
		TAILQ_FOREACH_MUTABLE(mpo, queue_head + i, queue_entry, tpo){
			if(OBJECT_EQ(po, mpo)){
				TAILQ_REMOVE(queue_head + i, mpo, queue_entry);
				//free(mpo);
			}
		}
	}
	for (i = 0; i < mouse_judge->threshold_total && time - i >= 0; i++)
	{
		TAILQ_FOREACH_MUTABLE(mpo, queue_head - i, queue_entry, tpo){
			if(OBJECT_EQ(po, mpo)){
				TAILQ_REMOVE(queue_head - i, mpo, queue_entry);
				//free(mpo);
			}
		}

	}
	score = po->score;
	 //free(po);
	return score;
}

enum object_score 
mouse_judge_up(struct mouse_judge_t *mouse_judge, int pos_x, int pos_y, size_t time, int *id_store, enum object_type *type_store)
{
	struct object_t  *po, *mpo, *tpo;

	struct object_queue_head_t *queue_head = mouse_judge->object_queue_array + time;
	int find = 0, i;
	enum object_score score;

	TAILQ_FOREACH(po, queue_head, queue_entry){
		if (po->type == object_slide_end &&
				click_judge(po->pos_x, po->pos_y, 
					pos_x, pos_y, 
					mouse_judge->redius_hit)){
			find = 1;
			break;
		}
	}
	if (!find)
		return score_nonhit;
	if (id_store)
		*id_store = po->id;
	if (type_store)
		*type_store = po->type;
	TAILQ_REMOVE(queue_head, po, queue_entry);
	//search all the queue for object equal to po
	//(time +- 2*total_threshold), and delete them!
	for (i = 0; i < mouse_judge->threshold_total && time + i < mouse_judge->length; i++)
	{
		TAILQ_FOREACH_MUTABLE(mpo, queue_head + i, queue_entry, tpo){
			if(OBJECT_EQ(po, mpo)){
				TAILQ_REMOVE(queue_head + i, mpo, queue_entry);
				//free(mpo);
			}
		}
	}
	for (i = 0; i < mouse_judge->threshold_total && time - i >= 0; i++)
	{
		TAILQ_FOREACH_MUTABLE(mpo, queue_head - i, queue_entry, tpo){
			if(OBJECT_EQ(po, mpo)){
				TAILQ_REMOVE(queue_head - i, mpo, queue_entry);
			//	free(mpo);
			}
		}

	}
	score = po->score;
	 //free(po);
	return score;
}

void 
mouse_judge_insert_slider(struct mouse_judge_t *mouse_judge, struct Dot *p,int dot_num, int repeat, int id)
{
	int i,j, 
		curtime = p[0].time,
		slider_start_time = p[0].time,
		slider_total_time = p[dot_num - 1].time - p[0].time,
		forward = 1;
	struct object_t *po;
	int threshold_miss = mouse_judge->threshold_miss,
		total_length = mouse_judge->length;
	//leading part
/*	for(i = curtime - mouse_judge->threshold_300; i < curtime; i ++){
		po = object_init(p[0].x, p[0].y, object_slide_body, score_nonhit, id);
		TAILQ_INSERT_TAIL(mouse_judge->object_queue_array + i, po, queue_entry);
	}*/
	//repeating part
	while(repeat){
		int now_time, next_time, valid_start, valid_end;
		//forward
		if(forward % 2){
			for(i = 0; i < dot_num - 1; i ++){
				next_time = p[i + 1].time - slider_start_time + curtime;		
				now_time = p[i].time - slider_start_time + curtime;
				if(p[i].is_tick){
					valid_start = now_time - threshold_miss >= 0 ? now_time - threshold_miss:0;
					valid_end = now_time + threshold_miss < total_length ? now_time + threshold_miss : total_length - 1;
					for(j = valid_start; j < valid_end; j++){
						po = object_init(p[i].x, p[i].y, object_slide_tick, score_nonhit, id);
						TAILQ_INSERT_TAIL(mouse_judge->object_queue_array + j, po, queue_entry);
					}
					id++;
				}
	/*			for(j = p[i].time - slider_start_time + curtime;
						j < next_time; j++){
					po = object_init(p[i].x, p[i].y, object_slide_body, score_nonhit, id);
					TAILQ_INSERT_TAIL(mouse_judge->object_queue_array + j, po, queue_entry);
				}*/
			}
			repeat--, forward++;
			curtime += slider_total_time;
		//backward
		}else{
			for(i = dot_num - 1; i > 0; i--){
				next_time = slider_total_time - 
					(p[i - 1].time - slider_start_time) + curtime;
				now_time = slider_total_time - 
					(p[i].time - slider_start_time) + curtime;
				if(p[i].is_tick){
					valid_start = now_time - threshold_miss >= 0 ? now_time - threshold_miss:0;
					valid_end = now_time + threshold_miss < total_length ? now_time + threshold_miss : total_length - 1;
					for(j = valid_start; j < valid_end; j++){
						po = object_init(p[i].x, p[i].y, object_slide_tick, score_nonhit, id);
						TAILQ_INSERT_TAIL(mouse_judge->object_queue_array + j, po, queue_entry);
					}
					id++;
				}
	/*			for(j = slider_total_time - 
						(p[i].time - slider_start_time) + curtime;
						j < next_time; j++){
					po = object_init(p[i].x, p[i].y, object_slide_body, score_nonhit, id);
					TAILQ_INSERT_TAIL(mouse_judge->object_queue_array + j, po, queue_entry);*/
			}
			repeat--, forward++;
			curtime += slider_total_time;
		}
	}
	//finishing part
	/*
	for(j = curtime; j < curtime + mouse_judge->threshold_300; j++){
		po = object_init(p[i].x, p[i].y, object_slide_body, score_nonhit, id);
		TAILQ_INSERT_TAIL(mouse_judge->object_queue_array + j, po, queue_entry);
	}*/
}
int
mouse_judge_drag(struct mouse_judge_t *mouse_judge, int pos_x, int pos_y, size_t time, int *id_store)
{
	struct object_t  *po, *mpo, *tpo;
	struct object_queue_head_t *queue_head = mouse_judge->object_queue_array + time;
	int find = 0, 
		erase_start, 
		erase_end,
		miss_threshold = mouse_judge->threshold_miss,
		total_length = mouse_judge->length;
	int i;
	TAILQ_FOREACH(po, queue_head, queue_entry){
		if (po->type == object_slide_tick && 
				click_judge(po->pos_x, 
					po->pos_y, 
					pos_x, 
					pos_y, 
					mouse_judge->redius_slide)){
			find = 1;
			break;
		}
	}
	if (!find)
		return 0;
	if (id_store)
		*id_store = po->id;
	//erase all others
	erase_start = time - miss_threshold > 0 ? time - miss_threshold : 0;
	erase_end = time + miss_threshold < total_length ? time + miss_threshold : total_length - 1;
	for (i = erase_start; i < erase_end; i ++)
		TAILQ_FOREACH_MUTABLE(mpo, queue_head + i, queue_entry, tpo){
			if(OBJECT_EQ(po, mpo)){
				TAILQ_REMOVE(queue_head + i, mpo, queue_entry);
				//free(mpo);
			}
		}

	return 1;

}
