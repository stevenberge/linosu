#ifndef __PARSER_H__
#define __PARSER_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <inc/queue.h>
#include <stddef.h>
/**
 * read the .ous and initial the data struct
 * Author: hzq0630
 */

struct General
{
	char AudioFilename[80];//The audio filename
	int AudioLeadIn;  //The number of ms to allow before mp3 starts.  Basically starts the     internal time counter at a negative position.
	int PreviewTime; //The position in the song (ms) to play the song selection preview fr    om.
	int Countdown;  //Type of countdown {Disabled = 0, Normal = 1, HalfSpeed = 2, DoubleSpeed = 3}
	int SampleSet; //0: normal, 1:soft;
	double StackLeniency;
	int Mode;
	int LetterboxInBreaks;
	int EditorBookmarks[20];	//Locations in the song that editor bookmarks have been placed (ms).
}static general;

struct Metadata
{
	char Title[80]; 
	char Artist[80];
	char Creater[80];
	char Version[80]; // esay, normal, hard, customize
	char Source[80];  //Song Source, used for searching and will be displayed. (spec max 80chars)
	char Tags[80]; //tags for searching;
}metadata;

struct Difficulty
{
	int HPDrainRate; //The level of HP drain, 0 (easy) to 10 (hard).
	int CircleSize;	//Size of hitobjects.  Assuming a gamefield of 512x384, 64 * (1 - 0.7*((CircleSize-5)/5))
	int OverallDifficulty; //Affects timing window, spinner difficulty and approach speeds.
	double SliderMultiplier; 	//Speed of slider movement, where 1.0 is 100pixels per beatLength.
	double SliderTickRate; //How often slider ticks are placed, where 1 is "1 tick per beat" etc.
}static difficulty;


struct TimingPoints
{ 
	TAILQ_ENTRY(TimingPoints) Tentry;	
	double offset; //offsetMs   - the offset in milliseconds for this section
	double beatLength;//beatLength - length of a single beat in milliseconds (double accuracy)
	int timingSignature; //timingSignature - 3 = simple triple, 4 = simple quadruple.  Used in editor.
	int sampleSetId; //sampleSetId - timing point specific samples {None = 0, Normal = 1, Soft = 2}
	int useCustomSampleas; //useCustomSamples - use skin localised samples? {No = 0, Yes = 1}
	int sampleVolume;
	int notInherited;
	int isKiai;
}static timingPoints;

struct HitCircle
{
	int xPos;
	int yPos;
	int startTimeMs;
	int objectType;
	int soundType;
}hitCircle;

struct Slider_r
{
	int xPos;
	int yPos;
	int startTimeMs;
	int objectType;
	int soundType;
	char curveType;
	int posNum;
	int pos[40][2];//0: xpos, 1:ypos
	int repeatCount;
	double sliderLengthPixels;
}slider;

struct HitObjects
{
	TAILQ_ENTRY(HitObjects) Hentry;
	int type; //0:hitcricle ,1: slider
	struct HitCircle hitCircle;
	struct Slider_r slider;
};

struct Color_r
{
	TAILQ_ENTRY(Color_r) Centry;	
	int r;
	int g;
	int b;
}static color;
TAILQ_HEAD(ObjectList, HitObjects); 
TAILQ_HEAD(ColorList, Color_r);
TAILQ_HEAD(TimingPointsList, TimingPoints);
void  osu_read(FILE* osu);
struct ObjectList *osu_get_object_list(void);

struct ColorList *osu_get_color_list(void);

struct General *osu_get_general(void);
struct Difficulty *osu_get_difficulty(void);
struct TimingPointsList *osu_get_timing_points(void);
int is_HitCircle(struct HitObjects *a);

char * osu_get_backGround(void);
char * osu_get_video(void);

#endif
