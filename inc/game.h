#ifndef __GAME_H__
#define __GAME_H__
#include <inc/texture.h>
#include <inc/audio.h>
#include <inc/mouse_judge.h>
#include <inc/parser.h>
#include <inc/unfinevent.h>

extern struct mouse_judge_t *mouse_judge;
extern int mouse_x,mouse_y;
extern GLuint tex_cursor;
extern struct UnfinEvent *u_events;
extern int event_num;
extern struct time_seq_t *seq;
extern struct sound_t *bgm;

void display(void);
void reshape(int w, int h);
void draw_cursor(int x, int y);
void mouse_drag(int x, int y);
void mouse_move(int x, int y);
void mouse_click(int button, int state, int x, int y);
void keyboard(unsigned char key, int x, int y);

#endif
