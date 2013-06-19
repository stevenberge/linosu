#ifndef __CURVE_H__
#define __CURVE_H__
#define CIRCLE_DENSITY 10
#define DIV_POINT_NUM 50
#include<inc/parser.h>
struct Dot {
	double x;
	double y;
	int time;
	double dx;
	double dy;
	int is_tick;
};
int curve_get_slider_points(struct Slider_r * slider, double slider_multiplier, double tick_rate, double beat_length, struct Dot **);
#endif

