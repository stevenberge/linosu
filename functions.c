#include <GL/glut.h>
#include <gst/gst.h>
#include <glib.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include <inc/game.h>
#include <inc/cell.h>
#include <inc/sfx.h>
#include <inc/game_status.h>
unsigned long game_status;

int current_slide_start;
//global current tick time

void 
display(void)
{
	glFlush();
	glDisable(GL_TEXTURE_2D);
	glutSwapBuffers();
}

void 
reshape(int w, int h)
{
//	glViewport(0, 0, (GLsizei) w, (GLsizei) h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho (0, w, h, 0, 0, 1);
	glMatrixMode(GL_MODELVIEW);
}

void draw_cursor(int x, int y)
{
	const int cursor_size = 50;
	int half = (cursor_size + 1) / 2 - 1;
	TEXTURE_DRAW_BEGIN(tex_cursor);
	TEXTURE_PIN(0.0, 0.0, x - half, y - half);
	TEXTURE_PIN(0.0, 1.0, x + half, y - half);
	TEXTURE_PIN(1.0, 1.0, x + half, y + half);
	TEXTURE_PIN(1.0, 0.0, x - half, y + half);
	TEXTURE_DRAW_END();
}
/**
 * add your mouse dragging event handler here:
 */

void mouse_drag(int x, int y)
{
	static long last_stray = 0;
	gint64 pos;
	mouse_x = x, mouse_y = y;
	/*
	if(!(game_status & GAME_SF_ONSLIDER))
		return;
	pos = sound_query_position(bgm) / 1000000;
	printf("!!%d, %d, %p\n", pos, current_tick_time, bgm);
	if(pos < current_tick_time - 500 || pos > current_tick_time + 500)
		return;

	if (mouse_judge_drag(mouse_judge, x, y, pos, NULL)){
		sfx_tick();
	}
	else
	{
		printf("miss: %d\n", current_tick_time);
		sfx_combo_miss();
	}*/
}
/**
 * add your mouse moving event handler here:
 */
void mouse_move(int x, int y)
{
	mouse_x = x, mouse_y = y;
}

/**
 * add your mouse clicking event handler here:
 */
void mouse_click(int button, int state, int x, int y)
{
	char *b, *s;
	gint64 pos;
	int event_id;
	enum object_type event_type;
	enum object_score score;
	switch (button){
		case GLUT_LEFT_BUTTON:
			pos = sound_query_position(bgm) / 1000000;
			if (state == GLUT_DOWN){
				SET_GAME_STATUS(GAME_SF_MOUSE_DOWN);
				score = mouse_judge_click(mouse_judge, x, 
					y, pos, &event_id, &event_type);
				//slider entrance: only me
			//	if(event_type == object_slide_start && 
			//			!(game_status & GAME_SF_ONSLIDER)){
			//		fprintf(stderr, "@%d: on slider now!\n", pos);
			//		current_slide_start = event_id;
			//		SET_GAME_STATUS(GAME_SF_ONSLIDER);
			//		RESET_GAME_STATUS(GAME_SF_STRAY);
				}
		//	}
			else if (state == GLUT_UP){
				RESET_GAME_STATUS(GAME_SF_MOUSE_DOWN);
			//	if (game_status & GAME_SF_ONSLIDER){
					score = mouse_judge_up(mouse_judge, x, 
							y, pos, &event_id, &event_type);
					//slider exit: this is the normal exit, 
					//if slider fails, the unfinevent of the 
					//end of slider will exit slidering status
			//		if (event_type == object_slide_end){
			//			fprintf(stderr, "@%d: off slider now!\n", pos);
			//			RESET_GAME_STATUS(GAME_SF_ONSLIDER);
			//			RESET_GAME_STATUS(GAME_SF_STRAY);
			//		}
			//	}else return;
			}
			switch(score){
				case score_300:
					finish_event(u_events, event_id, HIT_300, pos, EVENT_SUCCESS, seq);
					break;
				case score_100:
					//printf("score 100!\n");
					finish_event(u_events, event_id, HIT_100, pos, EVENT_SUCCESS, seq);
					break;
				case score_50:
					//printf("score 50!\n");
					finish_event(u_events, event_id, HIT_50, pos, EVENT_SUCCESS,seq);
					break;
				case score_miss:
				//	printf("score miss!\n");
					finish_event(u_events, event_id, HIT_miss, pos, EVENT_SUCCESS,seq);
					break;
				default:
				//	printf("hit nothing!\n");
					;
			}
		default:
			;
	}
		//case GLUT_MIDDLE_BUTTON:
	//		break;
	//	case GLUT_RIGHT_BUTTON:
	//		break;
	//		b = "no button";
}

void 
keyboard (unsigned char key, int x, int y)
{
	int t;
	switch (key) {
		case 27:
			//destroyer here!
			sfx_destroy();
			exit(0);
			break;
		case 'c':
		//	timer_callback(0);
			break;
		default:
			break;
	}
}

