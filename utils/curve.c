#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <inc/curve.h>
static double 
bezier_arc_len(double t, int control_points[][2], int start_idx, int degree)
{
	int i, j;
	double b[100][2], db[100][2];
	double result;
	for (i = 0; i < degree; i++)
	{
		b[i][0] = control_points[i+start_idx][0];
		b[i][1] = control_points[i+start_idx][1];
		//fprintf(stderr, "##%lf %lf\n", b[i][0], b[i][1]);
		db[i][0] = 0.0;
		db[i][1] = 0.0;
	}
	for (i = 1; i < degree; i++)
	{
		for (j = 0; j < degree-i; j++)
		{
			db[j][0] += t * (db[j+1][0] - db[j][0]) + (b[j+1][0] - b[j][0]);
			db[j][1] += t * (db[j+1][1] - db[j][1]) + (b[j+1][1] - b[j][1]);
			b[j][0] += t * (b[j+1][0] - b[j][0]);
			b[j][1] += t * (b[j+1][1] - b[j][1]);
		}
	}
	result = sqrt(db[0][0]*db[0][0] + db[0][1]*db[0][1]);
	return result;
}

static double 
simpson_method(double a, double b, double (*f)(double,int[][2],int,int), int control_points[][2], int start_idx, int degree)
{
	int i;
	double h = (b - a) / DIV_POINT_NUM;
	double result = 0.0;
	for (i = 0; i < DIV_POINT_NUM; i++)
		result += (f(a+i*h, control_points, start_idx, degree) + 4 * f(a+(i+0.5)*h, control_points, start_idx, degree) + f(a+(i+1)*h, control_points, start_idx, degree)) * h / 6;
	return result;
}

static double 
binary_search_t(double tar_length, int control_points[][2], int start_idx, int degree)
{
	double min = 0.0, mid = 0.5, max = 1.0;
	double cur_length;
//	printf("!!!!!!!!!!!!\n");
	while (max - min > 0.00001)
	{
		cur_length = simpson_method(0, mid, bezier_arc_len, control_points, start_idx, degree);
//		printf("max and min: %lf, %lf\n", max, min);
		//fprintf(stderr, "\t%lf  ", cur_length);
		if (fabs(cur_length-tar_length) < 0.01)
			break;
		if (cur_length - tar_length >= 0.01)
			max = mid;
		else
			min = mid;
		mid = (min + max) / 2;
	}
	return mid;
}

static void 
create_bezier(int control_points[][2], struct Dot *draw_points, int num, int start_idx, int degree, int start_time, double slider_multiplier, double beat_length)
{
	int i, j, k;
	double t;
	double b[100][2], db[100][2];
	//fprintf(stderr, "---------------------\n");
	for (k = 0; k < num; k++)
	{
		t = binary_search_t(k * CIRCLE_DENSITY, control_points, start_idx, degree);
		//fprintf(stderr, "%d: %lf\n", k, t);
		for (j = 0; j < degree; j++)
		{
			b[j][0] = control_points[j+start_idx][0];
			b[j][1] = control_points[j+start_idx][1];
			db[j][0] = 0.0;
			db[j][1] = 0.0;
		}
		for (i = 1; i < degree; i++)
		{
			for (j = 0; j < degree-i; j++)
			{
				db[j][0] += t * (db[j+1][0] - db[j][0]) + (b[j+1][0] - b[j][0]);
				db[j][1] += t * (db[j+1][1] - db[j][1]) + (b[j+1][1] - b[j][1]);
				b[j][0] += t * (b[j+1][0] - b[j][0]);
				b[j][1] += t * (b[j+1][1] - b[j][1]);
			}
		}
		//fprintf(stderr, "p: %lf %lf\n", b[0][0], b[0][1]);
		draw_points[k].x = b[0][0];
		draw_points[k].y = b[0][1];
		draw_points[k].time = (int)(start_time + (double)k * CIRCLE_DENSITY /(slider_multiplier*100) * beat_length);
		draw_points[k].dx = db[0][0] / sqrt(db[0][0]*db[0][0] + db[0][1]*db[0][1]);
		draw_points[k].dy = db[0][1] / sqrt(db[0][0]*db[0][0] + db[0][1]*db[0][1]);
	}
}

int 
curve_get_slider_points(struct Slider_r * slider, double slider_multiplier, double tick_rate, double beat_length, struct Dot ** dot_store)
{
	int control_points_num = slider->posNum+1;
	int (*control_points)[2]; //= slider->pos;
	int	start_time = slider->startTimeMs;
	double slider_length = slider->sliderLengthPixels, len, dis = (slider_multiplier*100) / tick_rate;
	int i, j, last_index, start_idx, degree;
	int circle_num, total_circle_num, tick_num;
	struct Dot *draw_points;
//	printf("beat length: %f, slider multiplier: %f, tick_rate: %f\n", beat_length, slider_multiplier, tick_rate);
//	fprintf(stderr, "%lf\n", slider_length);
	control_points = (void *)malloc((slider->posNum + 1)*sizeof(int)*2);
	control_points[0][0] = slider->xPos;
	control_points[0][1] = slider->yPos;
	for (i = 0; i < slider->posNum; i++)
		control_points[i+1][0] = slider->pos[i][0],
			control_points[i+1][1] = slider->pos[i][1];
	//for (i = 0; i < slider->posNum + 1; i++)
		//fprintf(stderr,"* %d %d\n", control_points[i][0],
				//control_points[i][1]);
	draw_points = NULL;
	total_circle_num = 0;	
	last_index = 0;
	for (i = 0; i < control_points_num; i++)
	{
		if (i== control_points_num-1 || (control_points[i][0] == control_points[i+1][0] && control_points[i][1] == control_points[i+1][1]))
		{
			start_idx = last_index;
			degree = i - start_idx + 1;
			len = simpson_method(0, 1, bezier_arc_len, control_points, start_idx, degree);
			//fprintf(stderr, "length: %lf\n", len);
			circle_num = (int)len / CIRCLE_DENSITY;
			if (i == control_points_num-1)
				circle_num++;
			//fprintf(stderr, "circle_num: %d\n", circle_num);
			total_circle_num += circle_num;
			draw_points = (struct Dot*)realloc(draw_points, total_circle_num*sizeof(struct Dot));
			//fprintf(stderr, "degree: %d\n", degree);
			create_bezier(control_points, draw_points+total_circle_num-circle_num, circle_num, start_idx, degree, start_time, slider_multiplier, beat_length);
			start_time += len / (slider_multiplier*100) * beat_length;
			last_index = i+1;
			tick_num = (int)(len / dis);
			//fprintf(stderr, "tick_num: %d\n", tick_num);
			for (j = 0; j < circle_num; j++)
				draw_points[total_circle_num-circle_num+j].is_tick = 0;
			for (j = 1; j <= tick_num; j++)
				draw_points[total_circle_num-circle_num+(int)(j*dis/CIRCLE_DENSITY)].is_tick = 1;
		}
	}
	//draw_points[total_circle_num-1].is_tick = 1;
	//for (i = 0; i < total_circle_num; i++)
	//	fprintf(stderr, "%lf %lf %d\n", draw_points[i].x, draw_points[i].y, draw_points[i].is_tick);
	//fprintf(stderr, "------------------------------\n");
	*dot_store = draw_points;
	return total_circle_num;
}
