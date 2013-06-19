#include <inc/time_seq.h>
#include <inc/cell.h>
#include <inc/unfinevent.h>
#include <inc/game_status.h>

#include <stdio.h>
#include <GL/glut.h>

int  current_tick_id ;
int current_slider_score;
/**
 * function to add unfinished event into the array;
 */
void 
add_unfin_event(struct UnfinEvent *u_event, int id, float x, float y, int time, int sound_type, int object_type)
{
	u_event[id].px = x;
	u_event[id].py = y;
	u_event[id].finish_time = time;
	u_event[id].finished = 0;
	u_event[id].sound_type = sound_type;
	u_event[id].object_type = object_type;
}

void
finish_event(struct UnfinEvent *u_event, int id, int score, int time, int success , struct time_seq_t *time_seq)
{
	u_event[id].finished = 1;
	if(success)
		sfx_play_by_type(u_event[id].sound_type);
	if(u_event[id].object_type == object_hit){
		create_hitscore_event(u_event[id].px, u_event[id].py, score, time_seq, time); 
	}
	else if(u_event[id].object_type == object_slide_tick){
	//	fprintf(stderr, "abcd\n");
		switch(current_slider_score){
			case HIT_miss:
				current_slider_score = HIT_50;
				break;
			case HIT_50:
				current_slider_score = HIT_100;
				break;
		}

		sfx_tick();
	}
	else if(u_event[id].object_type == object_slide_start){
		if(score == HIT_miss && current_slider_score == HIT_miss){
			sfx_combo_miss();
			create_hitscore_event(u_event[id].px, u_event[id].py, score, time_seq, time);
		}
		else{
			switch(score){
				case HIT_miss:
					current_slider_score = HIT_100;
					break;
				default:
					current_slider_score = HIT_300;
					break;
			}
		}
	}
	else if(u_event[id].object_type == object_slide_end){
		if(score == HIT_miss  && current_slider_score == HIT_miss){
			sfx_combo_miss();
			create_hitscore_event(u_event[id].px, u_event[id].py, score, time_seq, time);
		} 
		else{
			switch(score){
				case HIT_miss:
					if(current_slider_score == HIT_100)
						current_slider_score = HIT_miss;
					else
						current_slider_score = HIT_100;
					break;
				case HIT_50:
					if(current_slider_score == HIT_300)
						current_slider_score = HIT_100;
				case HIT_300:
					if(current_slider_score == HIT_miss)
						current_slider_score = HIT_100;
				default:
					current_slider_score = score;
					break;
			}
			create_hitscore_event(u_event[id].px, u_event[id].py, current_slider_score, time_seq, time); 
			if(current_slider_score == HIT_miss)
				sfx_combo_miss();
		}
	
	
	}
}

void kill_unfin_event_before(struct UnfinEvent *u_event, int event_num, int time, struct time_seq_t *time_seq)
{
	int i;
	int flag = 0;
	for(i = 0 ; i < event_num ; i++)
	{
		if(u_event[i].finish_time > time - 120) return;
		if(u_event[i].finished == 1) continue;
		switch(u_event[i].object_type){
			case object_slide_end:
			//	if( i == current_slide_start + 1){
			//		RESET_GAME_STATUS(GAME_SF_ONSLIDER);
			//	}
			//	u_event[i].finished = 1;
				finish_event(u_event, i, HIT_miss, u_event[i].finish_time, EVENT_FAIL, time_seq);
				break;
			case object_slide_start:
			//	current_slide_start = i;
				//sfx_combo_miss();
				SET_GAME_STATUS(GAME_SF_ONSLIDER);
				finish_event(u_event, i, HIT_miss, u_event[i].finish_time, EVENT_FAIL, time_seq);
				break;
			case object_hit:
				//sfx_combo_miss();
				finish_event(u_event, i, HIT_miss, u_event[i].finish_time,EVENT_FAIL,time_seq);
				break;
			case object_slide_tick:
			//	printf("!!!!!!!!1\n");
				set_current_tick_time(u_event, event_num, u_event[i].finish_time);
			//	printf("7: time: %d, 6: time: %d\n", u_event[7].finish_time, u_event[6].finish_time);
		//		printf("miss, id : %d, x:%f, y:%f\n", i, u_event[i].px, u_event[i].py);
				switch(current_slider_score){
					case HIT_300:
						current_slider_score = HIT_100;
						break;
					case HIT_100:
						current_slider_score = HIT_50;
						break;
				}
				sfx_combo_miss();
				u_event[i].finished = 1;
				break;
			default:;		
		}
	}
}

void set_current_tick_time(struct UnfinEvent *u_event, int event_num, int time)
{
	int i;
	if(time == 0) current_tick_id = 0;
	for(i = 0 ; i < event_num ; i++){
		if(u_event[i].object_type == object_slide_tick && u_event[i].finish_time > time && u_event[i].finished == 0){
			current_tick_id = i;
			break;
		}
	}
	//printf("current tick time: %d, %p\n", u_event[current_tick_id].finish_time, &current_tick_time);
	//printf("current_tick_id; %d\n", i);
}
