
#include <GL/glut.h>
#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include <math.h>

#include <inc/cell.h>
#include <inc/time_seq.h>
#include <inc/texture.h>
#include <inc/res.h>
#include <inc/game.h>
#include <inc/game_status.h>

/**
 *	definition of the functions for cells.
 *
 */
int hitcircle_color[10][3]={{255,255,0}, {255,0,255}, {0,255,255}, {0,0,255}, {0,255,0},{255,0,0}};
GLuint hitcircle_tex[10];
GLuint number_tex[10];
GLuint approach_tex[10];
GLuint hitscore_tex[8];
GLuint overlay_tex;
GLuint sliderball_tex[10];
GLuint background_tex;
GLuint sliderfollow_tex;
//use -1 to indicate this
GLuint reverse_tex;

int circle_size = 80;
int number_size = 24;
int score_size = 160;
int sliderball_size = 80;
int reverse_size = 50;
//各种用struct实现的c语言“类”的构造函数
//如何实现的多态？首先判断类型，然后进行类型转换，再调用多态函数。。
//Approach构造函数 实现trigger函数（event接口）
struct Approach *
new_approach(float x, float y, GLuint *text)
{
	struct Approach *rt;
	rt = (struct Approach *) malloc(sizeof(struct Approach));
	rt->px = x;
	rt->py = y;
	rt->current_zoomrate = APPROACH_INIT_ZOOMRATE;
	rt->tex_graph = text;
	rt->event_header.trigger = approach_trigger;
	return rt;
}

//Hitcircle构造函数 实现trigger函数（event接口）
struct Hitcircle *
new_hitcircle(float x, float y, GLuint *text, int with_overlay)
{
	struct Hitcircle *rt;
	rt = (struct Hitcircle *) malloc(sizeof(struct Hitcircle));
	rt->px = x;
	rt->py = y;
	rt->tex_graph = text;
	if(with_overlay)
		rt->event_header.trigger = hitcircle_trigger;
	else 
		rt->event_header.trigger = hitcircle_trigger_no_overlay;
	return rt;
}

struct Circlenumber *
new_circlenumber(float x, float y, GLuint *text)
{
	struct Circlenumber *rt;
	rt = (struct Circlenumber *) malloc(sizeof(struct Circlenumber));
	rt->px = x;
	rt->py = y;
	rt->tex_graph = text;
	rt->event_header.trigger = circlenumber_trigger;
	return rt;
}

struct Hitscore * 
new_hitscore(float x, float y, GLuint *text)
{
	struct Hitscore *rt;
	rt = (struct Hitscore *)malloc(sizeof(struct Hitscore));
	rt->px = x;
	rt->py = y;
	rt->tex_graph = text;
	rt->current_zoomrate = HITSCORE_INIT_ZOOMRATE;
	rt->event_header.trigger = hitscore_trigger;
	return rt;
}

struct Sliderball *
new_sliderball(float x, float y, GLuint *text, float dx, float dy, int sound_type)
{
	struct Sliderball *rt;
	rt = (struct Sliderball *)malloc(sizeof(struct Sliderball));
	rt->px = x;
	rt->py = y;
	rt->dx = dx;
	rt->dy = dy;
	rt->tex_graph = text;
	rt->sound_type = sound_type;
	rt->ever_played = 0;
	rt->event_header.trigger = sliderball_trigger;
	return rt;
}

struct Slider *
new_slider(struct Dot *points, int len, int color_index, int reverse_time)
{
	struct Slider *rt;
	rt = (struct Slider *)malloc(sizeof(struct Slider));
	rt->points = points;
	rt->max_len = len;
	rt->reverse_time = reverse_time;
	rt->color_index = color_index;
	rt->event_header.trigger = slider_trigger;
	return rt;
}
struct Background *
new_background()
{
	struct Background *rt;
	rt = (struct Background *)malloc(sizeof(struct Background));
	rt->event_header.trigger = background_trigger;
}


void 
approach_trigger(void *self, struct time_seq_t *time_seq, size_t time)
{
	struct Approach *p = (struct Approach *)self;
	float trx, try, blx, bly;
	trx = p->px + (circle_size/2 * p->current_zoomrate);
	try = p->py + (circle_size/2 * p->current_zoomrate);
	blx = p->px - (circle_size/2 * p->current_zoomrate);
	bly = p->py - (circle_size/2 * p->current_zoomrate);

	TEXTURE_DRAW_BEGIN(*p->tex_graph);
	TEXTURE_PIN(0.0,0.0, blx, bly);
	TEXTURE_PIN(0.0,1.0, blx, try);
	TEXTURE_PIN(1.0,1.0, trx, try);
	TEXTURE_PIN(1.0,0.0, trx, bly);
	TEXTURE_DRAW_END();
}

void
hitcircle_trigger(void *self, struct time_seq_t *time_seq, size_t time)
{
	struct Hitcircle *p = (struct Hitcircle *)self;
	float trx, try, blx, bly;
	trx = p->px + circle_size/2.0;
	try = p->py + circle_size/2.0;
	blx = p->px - circle_size/2.0;
	bly = p->py - circle_size/2.0;

	TEXTURE_DRAW_BEGIN(*p->tex_graph);
	TEXTURE_PIN(0.0,0.0, blx, bly);
	TEXTURE_PIN(0.0,1.0, blx, try);
	TEXTURE_PIN(1.0,1.0, trx, try);
	TEXTURE_PIN(1.0,0.0, trx, bly);
	TEXTURE_DRAW_END();

	TEXTURE_DRAW_BEGIN(overlay_tex);
	TEXTURE_PIN(0.0,0.0, blx, bly);
	TEXTURE_PIN(0.0,1.0, blx, try);
	TEXTURE_PIN(1.0,1.0, trx, try);
	TEXTURE_PIN(1.0,0.0, trx, bly);
	TEXTURE_DRAW_END();
}

void
hitcircle_trigger_no_overlay(void *self, struct time_seq_t *time_seq, size_t time)
{
	struct Hitcircle *p = (struct Hitcircle *)self;
	float trx, try, blx, bly;
	trx = p->px + circle_size/2.0;
	try = p->py + circle_size/2.0;
	blx = p->px - circle_size/2.0;
	bly = p->py - circle_size/2.0;

	TEXTURE_DRAW_BEGIN(*p->tex_graph);
	TEXTURE_PIN(0.0,0.0, blx, bly);
	TEXTURE_PIN(0.0,1.0, blx, try);
	TEXTURE_PIN(1.0,1.0, trx, try);
	TEXTURE_PIN(1.0,0.0, trx, bly);
	TEXTURE_DRAW_END();
}

void 
circlenumber_trigger(void *self, struct time_seq_t *time_seq, size_t time)
{
	struct Circlenumber *p = (struct Circlenumber *)self;
	float trx, try, blx, bly;
	if(p->tex_graph != &reverse_tex){
		trx = p->px + number_size/2.0;
		try = p->py + number_size/2.0;
		blx = p->px - number_size/2.0;
		bly = p->py - number_size/2.0;
	}
	else{
		trx = p->px + reverse_size/2.0;
		try = p->py + reverse_size/2.0;
		blx = p->px - reverse_size/2.0;
		bly = p->py - reverse_size/2.0;
	}

	TEXTURE_DRAW_BEGIN(*p->tex_graph);
	TEXTURE_PIN(0.0,0.0, blx, bly);
	TEXTURE_PIN(0.0,1.0, blx, try);
	TEXTURE_PIN(1.0,1.0, trx, try);
	TEXTURE_PIN(1.0,0.0, trx, bly);
	TEXTURE_DRAW_END();
}

void
hitscore_trigger(void *self, struct time_seq_t *time_seq, size_t time)
{
	struct Hitscore *p = (struct Hitscore *)self;
	float trx, try, blx, bly;
	if(p->tex_graph == &hitscore_tex[HIT_30])
	{
		trx = p->px + 40/2.0 * p->current_zoomrate;
		try = p->py + 25/2.0 * p->current_zoomrate;
		blx = p->px - 40/2.0 * p->current_zoomrate;
		bly = p->py - 25/2.0 * p->current_zoomrate;
	}
	else{
		trx = p->px + score_size/2.0 * p->current_zoomrate;
		try = p->py + score_size/2.0 * p->current_zoomrate;
		blx = p->px - score_size/2.0 * p->current_zoomrate;
		bly = p->py - score_size/2.0 * p->current_zoomrate;
	}
	TEXTURE_DRAW_BEGIN(*p->tex_graph);
	TEXTURE_PIN(0.0,0.0, blx, bly);
	TEXTURE_PIN(0.0,1.0, blx, try);
	TEXTURE_PIN(1.0,1.0, trx, try);
	TEXTURE_PIN(1.0,0.0, trx, bly);
	TEXTURE_DRAW_END();
}
int ever_played = 0;
float sound_x, sound_y;
void 
sliderball_trigger(void *self, struct time_seq_t *time_seq, size_t time)
{
	struct Sliderball *p = (struct Sliderball *)self;
	float ltx, lty, rtx, rty, rbx, rby, lbx, lby;
	float trx, try, blx, bly;
	float temp_x, temp_y;
	float cosa, sina;
	cosa = p->dx;
	sina = p->dy;
	temp_x = sliderball_size/2;
	temp_y = sliderball_size/2;
	rtx = temp_x*cosa - temp_y*sina + p->px;
	rty = temp_x*sina + temp_y*cosa + p->py;
	temp_x = -sliderball_size/2;
	temp_y = sliderball_size/2;
	ltx = temp_x*cosa - temp_y*sina + p->px;
	lty = temp_x*sina + temp_y*cosa + p->py;
	temp_x = sliderball_size/2;
	temp_y = -sliderball_size/2;
	rbx = temp_x*cosa - temp_y*sina + p->px;
	rby = temp_x*sina + temp_y*cosa + p->py;
	temp_x = -sliderball_size/2;
	temp_y = -sliderball_size/2;
	lbx = temp_x*cosa - temp_y*sina + p->px;
	lby = temp_x*sina + temp_y*cosa + p->py;
	
	TEXTURE_DRAW_BEGIN(*p->tex_graph);
	TEXTURE_PIN(0.0,0.0, lbx, lby);
	TEXTURE_PIN(0.0,1.0, ltx, lty);
	TEXTURE_PIN(1.0,1.0, rtx, rty);
	TEXTURE_PIN(1.0,0.0, rbx, rby);
	TEXTURE_DRAW_END();
	if(game_status & GAME_SF_MOUSE_DOWN){
		trx = p->px + circle_size;
		try = p->py + circle_size;
		blx = p->px - circle_size;
		bly = p->py - circle_size;
		TEXTURE_DRAW_BEGIN(sliderfollow_tex);
		TEXTURE_PIN(0.0,0.0, blx, bly);
		TEXTURE_PIN(0.0,1.0, blx, try);
		TEXTURE_PIN(1.0,1.0, trx, try);
		TEXTURE_PIN(1.0,0.0, trx, bly);
		TEXTURE_DRAW_END();
		if(p->sound_type != -1 && ever_played == 0 && !(fabs(sound_x - p->px) < 0.01) && !(fabs(sound_y - p->py)<0.01)){
			sfx_play_by_type(p->sound_type);
			ever_played = 1;
			sound_x = p->px;
			sound_y = p->py;
		}
		else if(p->sound_type == -1 &&ever_played == 1)
		{
			ever_played = 0;
		}
	}
}

void slider_trigger(void *self, struct time_seq_t *time_seq, size_t time)
{
	struct Slider *p = (struct Slider *)self;
	int len = p->max_len;
	int exist_time = p->points[len-1].time - p->points[0].time + HITCIRCLE_EXIST_TIME;
	int interval_time = p->points[len-1].time - p->points[0].time;
	int scan_time = (time - (p->points[len-1].time -exist_time))/interval_time;
	int i, j;
	float trx, try, blx, bly;
	int event_id;
	int flag = 0;
	for(i = 0 ; i < len ; i++){
		if(fabs(p->points[i].x - u_events[current_tick_id].px) < 0.01 && fabs(p->points[i].y - u_events[current_tick_id].py) < 0.01){
			flag = 1;
			break;
		}
	}
	if(flag){
		if(time < u_events[current_tick_id].finish_time + 100 && time > u_events[current_tick_id].finish_time - 500){
			if(game_status & GAME_SF_MOUSE_DOWN){
				if((mouse_x - u_events[current_tick_id].px)*(mouse_x - u_events[current_tick_id].px) +  (mouse_y - u_events[current_tick_id].py)*(mouse_y - u_events[current_tick_id].py) < 4900)
				{
		//			printf("hit: %d,  x: %f, y: %f, px: %d, py: %d\n",current_tick_id, u_events[current_tick_id].px, u_events[current_tick_id].py, mouse_x, mouse_y);
					create_hitscore_event(u_events[current_tick_id].px, u_events[current_tick_id].py, HIT_30, time_seq, u_events[current_tick_id].finish_time);
					finish_event(u_events, current_tick_id, 0, time, 0, time_seq);
					set_current_tick_time(u_events, event_num, u_events[current_tick_id].finish_time);
				}
				else{
		//			printf("x: %f, y: %f, px: %d, py: %d\n", u_events[current_tick_id].px, u_events[current_tick_id].py, mouse_x, mouse_y);
				}
			}
		}
	}
	for(i = 0 ; i < p->max_len ; i++)
	{
		//draw an empty hitcircle
		trx = p->points[i].x + circle_size/2.0;
		try = p->points[i].y + circle_size/2.0;
		blx = p->points[i].x - circle_size/2.0;
		bly = p->points[i].y - circle_size/2.0;
		TEXTURE_DRAW_BEGIN(overlay_tex);
		TEXTURE_PIN(0.0,0.0, blx, bly);
		TEXTURE_PIN(0.0,1.0, blx, try);
		TEXTURE_PIN(1.0,1.0, trx, try);
		TEXTURE_PIN(1.0,0.0, trx, bly);
		TEXTURE_DRAW_END();
	}
	for(i = 0 ; i < p->max_len ; i++)
	{
		//draw a hitcircle
		trx = p->points[i].x + circle_size/2.0;
		try = p->points[i].y + circle_size/2.0;
		blx = p->points[i].x - circle_size/2.0;
		bly = p->points[i].y - circle_size/2.0;
		TEXTURE_DRAW_BEGIN(hitcircle_tex[p->color_index]);
		TEXTURE_PIN(0.0,0.0, blx, bly);
		TEXTURE_PIN(0.0,1.0, blx, try);
		TEXTURE_PIN(1.0,1.0, trx, try);
		TEXTURE_PIN(1.0,0.0, trx, bly);
		TEXTURE_DRAW_END();
	}
	trx = p->points[0].x + circle_size/2.0;
	try = p->points[0].y + circle_size/2.0;
	blx = p->points[0].x - circle_size/2.0;
	bly = p->points[0].y - circle_size/2.0;
	TEXTURE_DRAW_BEGIN(hitcircle_tex[p->color_index]);
	TEXTURE_PIN(0.0,0.0, blx, bly);
	TEXTURE_PIN(0.0,1.0, blx, try);
	TEXTURE_PIN(1.0,1.0, trx, try);
	TEXTURE_PIN(1.0,0.0, trx, bly);
	TEXTURE_DRAW_END();
	TEXTURE_DRAW_BEGIN(overlay_tex);
	TEXTURE_PIN(0.0,0.0, blx, bly);
	TEXTURE_PIN(0.0,1.0, blx, try);
	TEXTURE_PIN(1.0,1.0, trx, try);
	TEXTURE_PIN(1.0,0.0, trx, bly);
	TEXTURE_DRAW_END();

	trx = p->points[len-1].x + circle_size/2.0;
	try = p->points[len-1].y + circle_size/2.0;
	blx = p->points[len-1].x - circle_size/2.0;
	bly = p->points[len-1].y - circle_size/2.0;
	TEXTURE_DRAW_BEGIN(hitcircle_tex[p->color_index]);
	TEXTURE_PIN(0.0,0.0, blx, bly);
	TEXTURE_PIN(0.0,1.0, blx, try);
	TEXTURE_PIN(1.0,1.0, trx, try);
	TEXTURE_PIN(1.0,0.0, trx, bly);
	TEXTURE_DRAW_END();
	TEXTURE_DRAW_BEGIN(overlay_tex);
	TEXTURE_PIN(0.0,0.0, blx, bly);
	TEXTURE_PIN(0.0,1.0, blx, try);
	TEXTURE_PIN(1.0,1.0, trx, try);
	TEXTURE_PIN(1.0,0.0, trx, bly);
	TEXTURE_DRAW_END();
}
void
background_trigger(void *self, struct time_seq_t *time_seq, size_t time)
{
	TEXTURE_DRAW_BEGIN(background_tex);
	TEXTURE_PIN(0.0,0.0, 0, 0);
	TEXTURE_PIN(0.0,1.0, 0, 480);
	TEXTURE_PIN(1.0,1.0, 640, 480);
	TEXTURE_PIN(1.0,0.0, 640, 0);
	TEXTURE_DRAW_END();
}

/**
 * ensure that color array and hitcircle_tex array has been initialized
 *
 */
void
create_hitcircle_event(float x, float y, int color_index, struct time_seq_t *time_seq, size_t time, size_t last_time, int with_overlay)
{
	int i, j;
	struct Hitcircle* p;
	for(i = 0 ; i < last_time ; i++){
		p = new_hitcircle(x, y, &hitcircle_tex[color_index], with_overlay);
		time_seq_insert_head(time_seq, time + i, (struct event_t*)p);
	}
}
/**
 *ensure that approach_tex array has been initialized
 *
 */

//一个一个的圆
void 
create_approach_event(float x, float y, int color_index, struct time_seq_t *time_seq, size_t time)
{
	int i;
	struct Approach* p;
	for(i = 0 ; i < CIRCLE_CLOSE_TIME ; i++){
		p = new_approach(x, y, &approach_tex[color_index]);
		p->current_zoomrate = (float)APPROACH_INIT_ZOOMRATE - i * (float)(APPROACH_INIT_ZOOMRATE - 1)/CIRCLE_CLOSE_TIME;
		time_seq_insert_tail(time_seq, time + i, (struct event_t*)p);
	}
}

/**
 * ensure that number_tex array has been initialized
 *
 */
//事件:
//从time开始,在(x,y)点
//持续显示last_time时间的数字number
//材质：reverse_tex
void 
create_circlenumber_event(float x, float y, int number, struct time_seq_t *time_seq, size_t time, size_t last_time)
{
	int i;
	float x1, x2;
	int  number1, number2;
	struct Circlenumber* p;
	if(number == -1)
	{
		for(i = 0 ; i < last_time; i++){
			p = new_circlenumber(x, y, &reverse_tex);
			time_seq_insert_head(time_seq, time+i, (struct event_t*)p);
		}
	}
	else if(number < 10){
		for(i = 0 ; i < last_time ; i++){
			p = new_circlenumber(x, y, &number_tex[number]);
			time_seq_insert_head(time_seq, time + i, (struct event_t*)p);
		}
	}
	else {
		//显示两位数
		x1 = x - number_size/2;
		x2 = x + number_size/2;
		number1 = number/10;
		number2 = number%10;
		for(i = 0 ; i < last_time; i++){
			p = new_circlenumber(x1, y, &number_tex[number1]);
			time_seq_insert_head(time_seq, time + i, (struct event_t*)p);
			p = new_circlenumber(x2, y, &number_tex[number2]);
			time_seq_insert_head(time_seq, time + i, (struct event_t*)p);
		}
	}
}

//事件：
//从time开始，在(x,y)点
//显示点击得分score
//材质：hitscore_tex[score]
//特效：放大
void
create_hitscore_event(float x, float y, int score, struct time_seq_t *time_seq, size_t time)
{
	int i;
	struct Hitscore *p;
	//出现的时候首先放大,zoomrate>1
	for(i = 0 ; i < HITSCORE_APPEAR_TIME ; i++){
		p = new_hitscore(x, y, &hitscore_tex[score]);
		p->current_zoomrate = (float)HITSCORE_INIT_ZOOMRATE + i * (float)(1 - HITSCORE_INIT_ZOOMRATE)/(float)HITSCORE_APPEAR_TIME;
		time_seq_insert_tail(time_seq, time + i, (struct event_t*)p);
	}
	//退出的过程不再放大,zoomrate=1
	for(; i < HITSCORE_EXIST_TIME ; i++){
		p = new_hitscore(x, y, &hitscore_tex[score]);
		p->current_zoomrate = 1;
		time_seq_insert_tail(time_seq, time + i, (struct event_t*)p);
	}
}

//事件：
//从time开始，在(x,y)点
//显示滑动的球
//材质：sliderball_tex[index]
//声音: sound_type
void
create_sliderball_event(float x, float y, int index, struct time_seq_t *time_seq, size_t time, size_t last_time, float dx, float dy, int sound_type)
{
	int i;
	struct Sliderball *p;
	for(i = 0 ; i < last_time; i++)
	{
		p = new_sliderball(x, y, &sliderball_tex[index], dx, dy, sound_type);
		time_seq_insert_head(time_seq, time+i, (struct event_t*)p);
	}
}


//事件：
//显示max_time的背景
//材质: texture_load_with_alpha(background,0.6)
void
create_background(char *background, struct time_seq_t *time_seq, int max_time)
{
	struct Background *p;
	int i;
	printf("%s\n", background);
	background_tex = texture_load_with_alpha(background, 0.6);
	for(i = 0 ; i < max_time ;i++){
		p = new_background();
		time_seq_insert_head(time_seq, i, (struct event_t*)p);
	}

}

/**
 * initial the global vars here.
 * hitcircle_color[], hitcircle_tex[], approach_tex[], number_tex[].
 *
 */
extern int color_defined;
extern int color_num;
void 
cell_init()
{
	int i;
	char temp[50];
	struct ColorList *c;
	struct Color_r *pc;
	if(color_defined){
		// read color from the file; else use the default color
		c = osu_get_color_list();
		i = 0;
		TAILQ_FOREACH(pc,c, Centry)//foreach pc in list(c)
		{
			hitcircle_color[i][0] = pc->r;
			hitcircle_color[i][1] = pc->g;
			hitcircle_color[i][2] = pc->b;
			printf("a:%d, %d, %d\n", pc->r, pc->g, pc->b);
		 	i ++;	
		}
		color_num = i;
	}
	for(i = 0 ; i < color_num ; i++){
		hitcircle_tex[i] = texture_load_with_color(STR(MATERIAL_DIR/hitcircle.png), hitcircle_color[i][0], hitcircle_color[i][1], hitcircle_color[i][2]);
		approach_tex[i] = texture_load_with_color(STR(MATERIAL_DIR/approachcircle.png), hitcircle_color[i][0], hitcircle_color[i][1], hitcircle_color[i][2]);
	}
	//load an empty texture. use to draw an empty overlay
	hitcircle_tex[9] = texture_load(STR(MATERIAL_DIR/hitcircle.png));

	for(i = 0 ; i < 10 ; i++){
		sprintf(temp, STR(MATERIAL_DIR/default-%d.png), i);
		number_tex[i] = texture_load(temp);
	}
	//load and slider ball texture
	for(i = 0 ; i < 10 ; i++){
		sprintf(temp, STR(MATERIAL_DIR/sliderb%d.png), i);
		sliderball_tex[i] = texture_load(temp);
	}

	//read a reverse tex;
	reverse_tex = texture_load(STR(MATERIAL_DIR/reversearrow.png));
	overlay_tex = texture_load(STR(MATERIAL_DIR/hitcircleoverlay.png));
	sliderfollow_tex = texture_load(STR(MATERIAL_DIR/sliderfollowcircle.png));
	hitscore_tex[HIT_50] = texture_load(STR(MATERIAL_DIR/hit50.png));
	hitscore_tex[HIT_100] = texture_load(STR(MATERIAL_DIR/hit100.png));
	hitscore_tex[HIT_100k] = texture_load(STR(MATERIAL_DIR/hit100k.png));
	hitscore_tex[HIT_300] = texture_load(STR(MATERIAL_DIR/hit300.png));
	hitscore_tex[HIT_300k] = texture_load(STR(MATERIAL_DIR/hit300k.png));
	hitscore_tex[HIT_300g] = texture_load(STR(MATERIAL_DIR/hit300g.png));
	hitscore_tex[HIT_miss] = texture_load(STR(MATERIAL_DIR/hit0.png));
	hitscore_tex[HIT_30] = texture_load(STR(MATERIAL_DIR/sliderpoint30.png));
}

/*
void
create_slider(struct Dot *points, int len, struct time_seq_t *time_seq, int color_index, int number, int reverse_time)
{
	int i, j;
	int exist_time, interval_time;
	struct Hitcircle *p;
	struct Circlenumber *q;
	exist_time = points[len-1].time - points[0].time + HITCIRCLE_EXIST_TIME;
	interval_time = points[len -1].time -points[0].time;
	for(j = 0 ; j < reverse_time ;j++){
		if(j == 0){
			create_approach_event(points[0].x, points[0].y, color_index, time_seq, points[len-1].time-exist_time);
			create_circlenumber_event(points[0].x, points[0].y, number, time_seq, points[len-1].time-exist_time + j * interval_time, exist_time);
			if(reverse_time != 1){
				create_circlenumber_event(points[len-1].x, points[len-1].y, -1, time_seq, points[len-1].time-exist_time + j * interval_time, exist_time);
			}
		}
		else if(j == reverse_time -1)
		{
			if(j % 2 ==1)
			{
				create_circlenumber_event(points[len-1].x, points[len-1].y, -1, time_seq, points[len-1].time-exist_time + j * interval_time, exist_time);
			}
			else
				create_circlenumber_event(points[0].x, points[0].y, -1, time_seq, points[len-1].time-exist_time + j * interval_time, exist_time);
		}
		else{
			create_circlenumber_event(points[len-1].x, points[len-1].y, -1, time_seq, points[len-1].time-exist_time + j * interval_time, exist_time);
			create_circlenumber_event(points[0].x, points[0].y, -1, time_seq, points[len-1].time-exist_time + j * interval_time, exist_time);
		}
		create_hitcircle_event(points[0].x, points[0].y, color_index, time_seq, points[len-1].time-exist_time + j*interval_time, exist_time, 1);
		create_hitcircle_event(points[len-1].x, points[len-1].y, color_index, time_seq, points[len-1].time-exist_time + j*interval_time, exist_time, 1);
		for(i = 0 ; i< len ; i++){
			create_hitcircle_event(points[i].x,points[i].y,color_index, time_seq, points[len-1].time-exist_time + j*interval_time, exist_time, 0);
		}
		for(i = 0 ;i  < len ; i ++)
			create_hitcircle_event(points[i].x, points[i].y, 9, time_seq, points[len-1].time - exist_time + j*interval_time, exist_time, 1);
		create_slider_ball(points, len, time_seq, j);
	}
}*/
//事件：
//在points个点上,
//显示滑动的球points[len]
//材质: 		颜色：color_index
//反向重放：reverse_time
//音效:sound_type
void create_slider(struct Dot *points, int len, struct time_seq_t *time_seq, int color_index, int number, int reverse_time, int sound_type)
{
//	int total_time = (points[len-1].time - points[0].time) * reverse_time + HITCIRCLE_EXIST_TIME;
//	int exist_time = points[len-1].time - points[0].time + HITCIRCLE_EXIST_TIME;
//hitcircle 存在一秒钟（HITCIRCLE_EXIST_TIME=1000)
	int total_time = (points[len-1].time - points[0].time) * reverse_time + ((points[0].time > HITCIRCLE_EXIST_TIME)?HITCIRCLE_EXIST_TIME:points[0].time);
	int exist_time = points[len-1].time - points[0].time +((HITCIRCLE_EXIST_TIME > points[0].time)?points[0].time:HITCIRCLE_EXIST_TIME);
	int interval_time = points[len-1].time - points[0].time;
	int i;
	struct Slider *p;
/*	for(i = 0 ; i < len ; i++){
		if(points[i].is_tick){
			create_hitscore_event(points[i].x, points[i].y, 0, time_seq, points[0].time);
		}
	}*/
	for(i = 0 ; i < reverse_time ; i++){
		create_slider_ball(points, len, time_seq, i, sound_type);
		//最开始添加一个approach
		if(i == 0){
			create_approach_event(points[0].x, points[0].y, color_index, time_seq, (points[len-1].time>exist_time)?(points[len-1].time-exist_time):0);
			if(reverse_time != 2){
				create_circlenumber_event(points[0].x, points[0].y, number, time_seq, points[len-1].time-exist_time, exist_time);
			}
			else
				create_circlenumber_event(points[0].x, points[0].y, number, time_seq, points[len-1].time-exist_time, exist_time + interval_time);
				
			if(reverse_time != 1){
				create_circlenumber_event(points[len-1].x, points[len-1].y, -1, time_seq, points[len-1].time-exist_time + i*interval_time, exist_time);
			}
		}
		else if(i == reverse_time -1)
		{
			if(i % 2 ==1)
			{
				create_circlenumber_event(points[len-1].x, points[len-1].y, -1, time_seq, points[len-1].time-exist_time + i * interval_time, interval_time);
			}
			else
				create_circlenumber_event(points[0].x, points[0].y, -1, time_seq, points[len-1].time-exist_time + i * interval_time, interval_time);
		}
		else{
			create_circlenumber_event(points[len-1].x, points[len-1].y, -1, time_seq, points[len-1].time-exist_time + i * interval_time, interval_time);
			create_circlenumber_event(points[0].x, points[0].y, -1, time_seq, points[len-1].time-exist_time + i * interval_time, interval_time);
		}
	}
	//每一个时间点i,同时显示i左右多个球
	for(i = 0 ; i < total_time ; i++ ){
		p = new_slider(points, len, color_index, reverse_time);
		time_seq_insert_head(time_seq, points[len-1].time -exist_time + i, (struct event_t*)p);
	}
}

//事件：
//添加sliderball
//starttime:points[i].time+前scantime次时延
//fortime:points[i+1].time-points[i].time
//倒数第二个sliderball有音效sound_type
void
create_slider_ball(struct Dot *points, int len, struct time_seq_t *time_seq, int scan_time, int sound_type)
{
	int i, j;
	j = 0;
	if(scan_time % 2 == 0){
		for(i = 0 ; i < len-1 ; i++)
		{
			j++;
			if(j == 10) j = 1;
			create_sliderball_event(points[i].x, points[i].y, j, time_seq, points[i].time + scan_time*(points[len-1].time-points[0].time), points[i+1].time - points[i].time, points[i].dx, points[i].dy, ((i == len-2)?sound_type:-1));
		}
	}
	else{
		for(i = len-1 ; i >=1 ; i--)
		{
			j++;
			if(j == 10) j = 1;
			create_sliderball_event(points[i].x, points[i].y, j, time_seq, points[len-i].time + scan_time*(points[len-1].time-points[0].time), points[len-i].time - points[len-i-1].time, -points[i].dx, -points[i].dy, ((i == len-2)?sound_type:-1));
		}
	}
}
