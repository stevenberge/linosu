/*
 * Project Linosu!
 * texture.h: functions encapsuled texture loading, 
 * creating and mapping.
 *
 * Author: Brills (brillsp@gmail.com)
 * History: 4/4/2011: first version.
 */
#ifndef __TEXTURE_H__
#define __TEXTURE_H__
#include <GL/glut.h>
#include <inc/lodepng.h>
#define TEXTURE_DRAW_BEGIN(__tex) \
		glBindTexture(GL_TEXTURE_2D, (__tex));	\
		glBegin(GL_QUADS);	
#define TEXTURE_DRAW_END()	\
		glEnd();
#define TEXTURE_PIN(__tx, __ty, __vx, __vy)	\
	glTexCoord2f((__tx), (__ty));	\
	glVertex2f((__vx), (__vy));

GLuint texture_load(const char *filename);
GLuint texture_bind_image(void *img, size_t width, size_t height);
GLuint texture_load_with_color(const char *filename, unsigned char r, unsigned char g, unsigned char b);
GLuint texture_load_with_alpha(const char *filename, float alpha);
#endif


