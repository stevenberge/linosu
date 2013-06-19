#include <assert.h>
#include <inc/game.h>
#include <inc/res.h>
int color_defined;
int color_num;
struct sound_t *bgm;
GLuint tex_cursor;

//unfinished event num and array
int event_num;
int tick_num;
struct UnfinEvent *u_events;

//about the array of time seq
struct time_seq_t *seq;
int max_len;

//about mouse
int mouse_x, mouse_y;
struct mouse_judge_t *mouse_judge;

void insert_objects(struct ObjectList *o, struct time_seq_t *seq)
{
	struct HitObjects *a;
	int i = 1;
	int temp_tick;
	int current_color = 0;
	event_num = 0;
	float tickrate, slider_multiplier, beat_length; 
	int current_time_offset, next_time_offset;
	struct TimingPointsList *timing_points_list;
	struct TimingPoints *t;
	struct Difficulty *d;
	d = osu_get_difficulty();
	tickrate = d->SliderTickRate;
	slider_multiplier = d->SliderMultiplier;
//	printf("aa:%f\n",slider_multiplier);
	
	timing_points_list = osu_get_timing_points();
//	TAILQ_FOREACH(t, timing_points_list, Tentry)
//	{
//		printf("%f, %f\n", t->beatLength, t->offset);
//	}
	t = TAILQ_FIRST(timing_points_list);
	beat_length = t->beatLength;
	next_time_offset = ((t != TAILQ_LAST(timing_points_list, TimingPointsList))?TAILQ_NEXT(t, Tentry)->offset:max_len);
	current_time_offset = 0 ;
	TAILQ_FOREACH(a, o, Hentry)
	{
		if(current_time_offset > next_time_offset){
			t = TAILQ_NEXT(t, Tentry);
			if(t->notInherited)
				beat_length = t->beatLength;
			next_time_offset = ((t != TAILQ_LAST(timing_points_list, TimingPointsList))?TAILQ_NEXT(t, Tentry)->offset:max_len);
		}
		if(is_HitCircle(a))
		{
			//b 点击 圆圈
			struct HitCircle *b = &a->hitCircle;
			current_time_offset = b->startTimeMs;
			//fprintf(stderr, "circle: %d, %d\n", b->xPos, b->yPos);
			//
			if(b->startTimeMs > HITCIRCLE_EXIST_TIME){
				create_approach_event(b->xPos, b->yPos, current_color, seq, b->startTimeMs/TIME_INTERVAL - CIRCLE_CLOSE_TIME);
				create_circlenumber_event(b->xPos, b->yPos, i, seq, b->startTimeMs/TIME_INTERVAL - HITCIRCLE_EXIST_TIME, HITCIRCLE_EXIST_TIME);
				create_hitcircle_event(b->xPos, b->yPos, current_color, seq, b->startTimeMs/TIME_INTERVAL - HITCIRCLE_EXIST_TIME, HITCIRCLE_EXIST_TIME, 1);
				mouse_judge_insert_hitobject(mouse_judge, b->xPos, b->yPos, b->startTimeMs, object_hit, event_num);
			}
			else{
				create_approach_event(b->xPos, b->yPos, current_color, seq, 0);
				create_circlenumber_event(b->xPos, b->yPos, i, seq, 0, b->startTimeMs);
				create_hitcircle_event(b->xPos, b->yPos, current_color, seq, 0, b->startTimeMs, 1);
				mouse_judge_insert_hitobject(mouse_judge, b->xPos, b->yPos, b->startTimeMs, object_hit, event_num);
			}
			event_num ++;
			if(b->objectType == 1){
				i ++;
			}	
			else if(b->objectType == 5)
			{
				i = 1; 
				current_color = (current_color+1)%color_num;
			}
			else{
				printf("wrong hit circle type !");
				return;
			}
		}else{
			// sliders
			struct Dot *points;
			int circle_num, j, temp_tick;
			current_time_offset = a->slider.startTimeMs;
			if(t->notInherited){
				beat_length = t->beatLength;
				circle_num = curve_get_slider_points(&a->slider, slider_multiplier, tickrate, beat_length, &points);
			}
			else{
				circle_num = curve_get_slider_points(&a->slider, slider_multiplier * 100.0 / ( -t->beatLength), tickrate, beat_length, &points);
			}
		 	create_slider(points, circle_num, seq, current_color,i, a->slider.repeatCount, a->slider.soundType);

			temp_tick = 0;
			for(j = 0 ; j < circle_num ; j++){
				if(points[j].is_tick)
					temp_tick++;
			}
			temp_tick = temp_tick * a->slider.repeatCount;	
			mouse_judge_insert_slider(mouse_judge, points, circle_num, a->slider.repeatCount, event_num);
			event_num += temp_tick;
			mouse_judge_insert_hitobject(mouse_judge, points[0].x, points[0].y, points[0].time, object_slide_start, event_num);
			event_num ++;
			if(a->slider.repeatCount %2 == 1)
			//slider最后滑到的点，该点上有一个hitobject事件(object_slide_end类型）
				mouse_judge_insert_hitobject(mouse_judge, points[circle_num-1].x, points[circle_num-1].y, points[circle_num -1].time, object_slide_end, event_num);
			else 
				mouse_judge_insert_hitobject(mouse_judge, points[0].x, points[0].y, points[0].time + (points[circle_num - 1].time - points[0].time)*a->slider.repeatCount, object_slide_end, event_num);
			event_num ++;
			if(a->slider.objectType == 2){
				i ++;
			}	
			else if(a->slider.objectType == 6)
			{
				i = 1; 
				current_color = (current_color+1)%color_num;
			}
			else{
				printf("wrong hit circle type !");
				return;
			}
		}
	}

	u_events = (struct UnfinEvent *)malloc((event_num) * sizeof(struct UnfinEvent));
	i = 0;
	temp_tick = 0;
	current_time_offset = 0;
	t = TAILQ_FIRST(timing_points_list);
	beat_length = t->beatLength;
	next_time_offset = ((t != TAILQ_LAST(timing_points_list, TimingPointsList))?TAILQ_NEXT(t, Tentry)->offset:max_len);
	TAILQ_FOREACH(a, o, Hentry)
	{
		if(current_time_offset > next_time_offset){
			t = TAILQ_NEXT(t, Tentry);
			if(t->notInherited)
				beat_length = t->beatLength;
			next_time_offset = ((t != TAILQ_LAST(timing_points_list, TimingPointsList))?TAILQ_NEXT(t, Tentry)->offset:max_len);
		}
		if(is_HitCircle(a)){
			current_time_offset = a->hitCircle.startTimeMs;
			add_unfin_event(u_events, i+temp_tick, a->hitCircle.xPos, a->hitCircle.yPos,a->hitCircle.startTimeMs, a->hitCircle.soundType, object_hit);
			i++;
		}
		else{
			struct Dot *points;
			int circle_num, j, k;
			current_time_offset = a->slider.startTimeMs;
			if(t->notInherited){
				beat_length = t->beatLength;
				circle_num = curve_get_slider_points(&a->slider, slider_multiplier, tickrate, beat_length, &points);
			}
			else{
				circle_num = curve_get_slider_points(&a->slider, slider_multiplier * 100.0 / (-t->beatLength), tickrate, beat_length, &points);
			}
			for(k = 0 ; k < a->slider.repeatCount ;k ++){
				if(k%2 == 0){
					for(j = 0 ; j < circle_num ;j ++)
					{
						if(points[j].is_tick){
							add_unfin_event(u_events, i, points[j].x, points[j].y, points[j].time + k*(points[circle_num - 1].time - points[0].time), 0, object_slide_tick);
							i++;
						}	
					}
				}
				else{
					for(j = circle_num -1 ; j>=0 ;j --)
					{
						if(points[j].is_tick){
							add_unfin_event(u_events,i, points[j].x, points[j].y, points[j].time+ k*(points[circle_num - 1].time - points[0].time), 0, object_slide_tick);
							i++;
						}	
					}
				}
			}
			if(a->slider.repeatCount %2 == 1){
				add_unfin_event(u_events, i, points[0].x, points[0].y,points[circle_num - 1].time + (a->slider.repeatCount-1)*(points[circle_num-1].time - points[0].time) , a->slider.soundType, object_slide_start);
				i++;
				add_unfin_event(u_events, i, points[circle_num - 1].x, points[circle_num - 1].y, points[circle_num - 1].time + (a->slider.repeatCount-1)*(points[circle_num-1].time - points[0].time), a->slider.soundType, object_slide_end);
				i++;
			}
			else{
				add_unfin_event(u_events, i, points[0].x, points[0].y,points[circle_num - 1].time + (a->slider.repeatCount-1)*(points[circle_num-1].time - points[0].time), a->slider.soundType, object_slide_start);
				i++;
				add_unfin_event(u_events, i, points[0].x, points[0].y, points[circle_num - 1].time + (a->slider.repeatCount-1)*(points[circle_num-1].time - points[0].time) + 50, a->slider.soundType, object_slide_end);
				i++;
			}
		}
	}
}	

void 
game_init(char *osu, char *sound, char *background_dir)
{    
	int x, y, time;
	int i;
	struct ObjectList *o;
	struct ColorList *c;
	FILE* fin;
	struct HitObjects *last;
	char *background;
	char background_path[200];

	int len;
	float points[100][3];
	int temp_time;
	fin = fopen(osu, "r");
	assert(fin);

	//get the max len
	osu_read(fin);
	fclose(fin);
	background = osu_get_backGround();
	sprintf(background_path, "%s/%s", background_dir, background);
//	printf("background: %s\n",background_path);
	o = osu_get_object_list();
	last = TAILQ_LAST(o, ObjectList);
	if(is_HitCircle(last)){
		max_len = last->hitCircle.startTimeMs + 5000;
	}
	else max_len = last->slider.startTimeMs + 5000;

	// read color from .osu file
	c = osu_get_color_list();
	if(TAILQ_EMPTY(c)){
		color_num = 6;
		color_defined = 0;
	}
	else{
		color_defined = 1;
	}

	mouse_judge = mouse_judge_init(max_len, 64, 120, 100,100,100,5);
	cell_init();
	seq = time_seq_init(max_len);
	insert_objects(o, seq);
	set_current_tick_time(u_events, event_num, 0);
	sfx_init();
	create_background(background_path, seq, max_len);

	bgm = sound_load(sound);
	printf("%s\n",sound);
	assert(bgm);
	tex_cursor = texture_load(STR(MATERIAL_DIR/cursor.png));
	
}

static int not_playing = 1;
gint64 last_time = 0;
const int FPS=200;
void
idle()
{
	gint64 pos;
	static int real_time, last_count_time = 0,count = 0, last_frame_time= 0;
	if(!last_count_time)
		last_count_time = glutGet(GLUT_ELAPSED_TIME);
	if(!last_frame_time)
		last_frame_time = glutGet(GLUT_ELAPSED_TIME);
	real_time = glutGet(GLUT_ELAPSED_TIME);
	if (real_time - last_frame_time < 1000/FPS){
		usleep(1);
		return;
	}
	last_frame_time = real_time;
	if (real_time - last_count_time > 1000){ //1 second
		//printf("FPS: %d, @%d\n", count, real_time);
		count = 0;
		last_count_time = real_time;
	}
	pos = sound_query_position(bgm);
	last_time = pos;
	glClearDepth(1.0f);
	glEnable(GL_TEXTURE_2D);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glAlphaFunc(GL_GREATER, 0.9);
	glEnable(GL_BLEND);
	//  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//printf("%lld, %d\n", pos, pos/1000000);
	kill_unfin_event_before(u_events, event_num, pos/1000000, seq);
	if(not_playing == 1){
		if(pos/1000000 <  max_len && pos/1000000 > 0)
			not_playing = 0;
	}
	else{
		time_seq_trigger_at(seq, pos/1000000);
	}
	draw_cursor(mouse_x, mouse_y);
	//time_seq_destory_at(seq, time);
	count++;
	if(pos >= sound_query_duration(bgm))// || pos/1000000 > max_len)
		exit(0);
	glutPostRedisplay();
}

void
gl_init(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(640, 480);
	glutInitWindowPosition(100, 100);
	glutCreateWindow(argv[0]);
	glutSetCursor(GLUT_CURSOR_NONE);
	glDisable(GL_DEPTH_TEST);
	glClearColor (0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_FLAT);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMotionFunc(mouse_drag);
	glutPassiveMotionFunc(mouse_move);
	glutMouseFunc(mouse_click);
	glutIdleFunc(idle);
}
int 
main(int argc, char** argv)
{
	/* Initialisation */
	gst_init (&argc, &argv);

	/* Check input arguments */
	if (argc != 4) {
		g_printerr ("Usage: %s <Ogg/Vorbis filename>\n", argv[0]);
		return -1;
	}
	/*initial the sound, cursor, mouse judge*/
	gl_init(argc, argv);
	glutMainLoop();
//	game_init(argv[1], argv[2], argv[3]);
//	
//	sound_play(bgm);
//	glutMainLoop();
	return 0; 
}
