#include <inc/parser.h>

static char backGround[100];
static char video[100];
static char backGroundOrVidoe[100];
static struct ObjectList HitQueueHead;
static struct ColorList ComboHead;
static struct TimingPointsList TimingPointsHead;
int sstrlen (char* p)
{
	int i;
	for(i = 0; *p != '\0' && *p != '\n'; i++, p++);
	return i;
}
/*
* find position of ":"
*/
int find_pos(char * oneline)
{
	int i=0;
	while ( oneline[i] != ':')
		i++;
	return i;
}
/*
* find the pos of ","
*/
int find_pos2(char * oneline)
{
	int i=0;
	int len;
	len = sstrlen(oneline);
	while ( oneline[i] != ',' && i < len )
		i++;
	if (i == len)
		return -1;
	return i;
}
/*
**find '|'
*/
int find_pos3(char * oneline)
{
	int i=0,len;
	len = sstrlen(oneline);
	while ( oneline[i] != '|' && i < len )
		i++;
	if (i == len)
		return -1;
	return i;
}

/*
* fill the pos of BYS
*/
void fill_curve_pos( char *oneline,  int num, struct Slider_r * slider )
{
	int pos,tmp,len,i;
	len = find_pos2(oneline);
	pos = find_pos3(oneline);
	tmp = find_pos(oneline);
	if ( pos == -1 || pos > len)
	{
		slider->pos[num][0]= getnum(oneline,tmp);
		slider->pos[num][1]= getnum(&oneline[tmp+1],len-tmp-1);
//		//printf("%s\n%d\n",&oneline[tmp+1],len);
		return;
	}
	else
	{
		slider->pos[num][0]= getnum(oneline,tmp);
		slider->pos[num][1]= getnum(&oneline[tmp+1],pos-tmp-1);
	}
	fill_curve_pos(&oneline[pos+1], num+1 , slider);
}



/*
* count the num of the ","
*/
int count(char * oneline)
{
	int i = 0 , num = 0;
	while ( oneline[i] != '\n' )
	{
		i++;
	//	//printf("%c\n",oneline[i]);
		if ( oneline[i] == ',')
			num++;
	}
	return num;
}
/**count the num of "|"
*/
int count2(char * oneline)
{
	int i = 0 , num = 0;
	while ( oneline[i] != '\n' )
	{
		i++;
	//	//printf("%c\n",oneline[i]);
		if ( oneline[i] == '|')
			num++;
		if ( oneline[i] == ',')
			break;
	}
	return num;
}


/*
 *change string to int
 */
int getnum(char * oneline , int len)
{
	int tmp = 0 , i = 0,mark = 1;
	if (oneline[i] == '-')
	{
		mark = -1;
		i++;
	}
	while ( i < len)
	{
		tmp = tmp * 10 + oneline[i] - '0';
		i++;
	}
	return tmp*mark;
}

double getnum2(char * oneline, int len)
{
	double res=0;
	int pos=0,tmp=10;

	while (oneline[pos] != '.' && pos < len)
		pos++;
	res = getnum(oneline , pos);
	if(oneline[pos] == '.')
	{
		pos++;
		if (res >= 0)
		{
			while(pos < len)
			{
		//		//printf("testgetnum2 %c %d\n",oneline[pos],tmp);
				res+= ((double)(oneline[pos]-'0'))/tmp;
				tmp *= 10;
				pos++;
			}
	//		//printf("hzq: %f\n",res);
		}
		else
		{
			while(pos < len)
			{
	//			//printf("testgetnum2 %c %d\n",oneline[pos],tmp);
				res-= ((double)(oneline[pos]-'0'))/tmp;
				tmp *= 10;
				pos++;
			}

		}
	}
	if (res<0)
		return (-1)*res;
	return res;
}

void fill_timingPoints(char * oneline, struct TimingPoints * timingPoints, int num )
{
	int pos,tmp;
	////printf("in fill hit circle");
	if (num == 7)
	{
		pos = find_pos2(oneline);

		//printf("hzq: pos=%d\n", pos);
		if (pos == -1 )
			pos = sstrlen(oneline) - 2;
		//printf("hzq: pos=%d\n", pos);
		timingPoints->isKiai = getnum(oneline,pos);
		return;
	}
	else
	{	
		pos = find_pos2(oneline);
		tmp = getnum(oneline, pos);
		if ( num == 0 )
		{
			timingPoints->offset = getnum2(oneline, pos);
		}
		if ( num == 1 )
		{
		//	timingPoints->beatLength = getnum2(oneline, pos);
			sscanf(oneline, "%lf", &timingPoints->beatLength);
		}

		if ( num == 2 )
		{
			timingPoints-> timingSignature = tmp;
		}

		if ( num == 3 )
		{
			timingPoints->sampleSetId = tmp;
		}
		if ( num == 4 )
		{
			timingPoints->useCustomSampleas = tmp;
		}
		if ( num == 5 )
		{
			timingPoints->sampleVolume = tmp;
		}
		if ( num == 6 )
		{
			timingPoints->notInherited = tmp;
		}

		fill_timingPoints(&oneline[pos+1], timingPoints, num+1 );
	}
}



void fill_hit_circle(char * oneline, struct HitCircle * hitCircle, int num )
{
	int pos,tmp;
	////printf("in fill hit circle");
	if (num == 4)
	{
		hitCircle->soundType = oneline[0]-'0';
		return;
	}
	else
	{

		pos = find_pos2(oneline);
		tmp = getnum(oneline, pos);
		if ( num == 0 )
		{
			hitCircle->xPos = tmp;
		}
		if ( num == 1 )
		{
			hitCircle->yPos = tmp;
		}

		if ( num == 2 )
		{
			hitCircle->startTimeMs = tmp;
		}

		if ( num == 3 )
		{
			hitCircle->objectType = tmp;
		}
		fill_hit_circle(&oneline[pos+1], hitCircle, num+1 );
	}
}


void fill_slider(char * oneline, struct Slider_r * slider, int num )
{
	int pos,tmp;
	if (num == 7)
	{
		pos = sstrlen(oneline) - 1;
	//	slider->sliderLengthPixels = getnum(oneline,pos);

		sscanf(oneline, "%lf", &slider->sliderLengthPixels);
//		printf("hzq: sliderLengthPixels %f\n",slider->sliderLengthPixels);
		return ;
	}
	else
	{
		pos = find_pos2(oneline);
		tmp = getnum(oneline, pos);
		if ( num == 0 )
		{
			slider->xPos = tmp;
		}
		if ( num == 1 )
		{
			slider->yPos = tmp;
		}

		if ( num == 2 )
		{
			slider->startTimeMs = tmp;
		}

		if ( num == 3 )
		{
			slider->objectType = tmp;
		}

		if ( num == 4 )
		{
			slider->soundType = tmp;
		}
		if ( num == 5 )
		{
			//the judge point of BYS
			slider->curveType = oneline[0];
			slider->posNum = count2( oneline);
			fill_curve_pos(oneline + 2, 0 , slider);

		}

		if ( num == 6 )
		{
			slider->repeatCount = tmp;
		}

		fill_slider(&oneline[pos+1], slider, num+1);

	}

}

void fill_slider2(char * oneline, struct Slider_r * slider, int num )
{
	int pos,tmp;
	if (num == 8)
	{
		return ;
	}
	else
	{
		pos = find_pos2(oneline);
		tmp = getnum(oneline, pos);
		if ( num == 0 )
		{
			slider->xPos = tmp;
		}
		if ( num == 1 )
		{
			slider->yPos = tmp;
		}

		if ( num == 2 )
		{
			slider->startTimeMs = tmp;
		}

		if ( num == 3 )
		{
			slider->objectType = tmp;
		}

		if ( num == 4 )
		{
			slider->soundType = tmp;
		}
		if ( num == 5 )
		{
			//the judge point of BYS
			slider->curveType = oneline[0];
			slider->posNum = count2( oneline);
			fill_curve_pos(oneline + 2, 0 , slider);

		}

		if ( num == 6 )
		{
			slider->repeatCount = tmp;
		}
		if ( num == 7 )
		{
			//slider->sliderLengthPixels = tmp;
			sscanf(oneline, "%lf", &slider->sliderLengthPixels);
		//	printf("hzq: sliderLengthPixels %f\n",slider->sliderLengthPixels);
		}
		fill_slider2(&oneline[pos+1], slider, num+1);

	}

}


void fill_color(char * oneline, struct Color_r * color, int num )
{
	int pos,tmp;
	////printf("in fill hit circle");
	if (num == 2)
	{
		pos =sstrlen(oneline) - 1;
		tmp = getnum(oneline, pos);
		color->b = tmp;
		return;
	}
	else
	{

		pos = find_pos2(oneline);
		tmp = getnum(oneline, pos);
		if ( num == 0 )
		{
			color->r = tmp;
		}
		if ( num == 1 )
		{
			color->g = tmp;
		}
		fill_color(&oneline[pos+1], color , num+1 );
		
	}
}



void read_section( FILE * f , char * oneline )
{
	int len,tmp,pos,i,counter;
	if ( oneline[1] == 'G')
	{
		/**
		*deal with_General
		*/
		//printf("G\n");
		while (  fgets( oneline, 1024, f ) != NULL  )
		{
			//printf("%s",oneline);
			len = sstrlen(oneline);
			if ( len == 2 )
			{
				continue;
			}
			if( oneline[0] == '[')
			{
				read_section( f, oneline );
				break;
			}
			i = 0;
			while ( oneline[i] == ' ' )
			{
				i++;
			}
			if ( oneline[i] == '/' || oneline[i] == '\n')
			{
				continue;
			}

			pos = find_pos(oneline);
			//printf("%d+%c\n",pos,oneline[pos]);
			if (pos == 13 && oneline[0] == 'A' )
			{
				pos++;
				while( oneline[pos] == ' ')
					pos++;
				strcpy( general.AudioFilename , &oneline[pos]);
				len = sstrlen( general.AudioFilename);
				 general.AudioFilename[len-1]='\0';
				//printf("%s\n", general.AudioFilename);
			}			
			if (pos == 11 && oneline[0] == 'A' )
			{
				pos++;
				while( oneline[pos] == ' ')
					pos++;
				len = sstrlen(&oneline[pos]);
				general.AudioLeadIn = getnum(&oneline[pos],len-1);
				//printf("%d\n", general.AudioLeadIn);
			}
			if (pos == 11 && oneline[0] == 'P' )
			{
				pos++;
				while ( oneline[pos] == ' ')
					pos++;
				len = sstrlen(&oneline[pos]);
				general.PreviewTime = getnum(&oneline[pos],len-1);
				//printf("%d\n", general.PreviewTime);
			}
			if (pos == 9 && oneline[0] == 'C' )
			{
				pos++;
				while ( oneline[pos] == ' ')
					pos++;
					len = sstrlen(&oneline[pos]);
				general.Countdown = getnum(&oneline[pos],len-1);
				//printf("%d\n", general.Countdown);
			}
			if (pos == 9 && oneline[0] == 'S' )
			{
				pos++;
				while ( oneline[pos] == ' ')
					pos++;
				if (oneline[pos]=='N')
					general.SampleSet=0;
				else
					general.SampleSet=1;
				//printf("%d\n", general.SampleSet);
			}
			if (pos == 13 && oneline[0] == 'S' )
			{
				pos++;
				while( oneline[pos] == ' ')
					pos++;
				len = sstrlen(&oneline[pos]);
				general.StackLeniency = getnum2(&oneline[pos],len-1);
	//			printf("hzq:  %f\n", general.StackLeniency);
			}
			if (pos == 4 && oneline[0] == 'M' )
			{
				pos++;
				while( oneline[pos] == ' ')
					pos++;
				len = sstrlen(&oneline[pos]);
				general.Mode= getnum(&oneline[pos],len-1);
				//printf("%d\n", general.Mode);
			}
			if (pos == 17 && oneline[0] == 'L' )
			{
				pos++;
				while( oneline[pos] == ' ')
					pos++;
				len = sstrlen(&oneline[pos]);
				general.LetterboxInBreaks = getnum(&oneline[pos],len-1);
				//printf("%d\n", general.LetterboxInBreaks);
			}
		}
	}
	else if ( oneline[1] == 'M' )
	{
		/**
		* deal with Metadata
		*/
		//printf("M\n");
		while (  fgets( oneline, 1024, f ) != NULL  )
		{
			//printf("%s",oneline);
			len = sstrlen(oneline);
			if ( len == 2 )
			{
				break;
			}
			if( oneline[0] == '[')
			{
				read_section( f, oneline );
				break;
			}
			i = 0;
			while ( oneline[i] == ' ' )
			{
				i++;
			}
			if ( oneline[i] == '/' || oneline[i] == '\n')
			{
				continue;
			}


			pos = find_pos(oneline);
			//printf("%d+%c\n",pos,oneline[pos]);
		}
	}
	else if ( oneline[1] == 'D' )
	{
		/**
		* deal with Difficult
	*/
		//printf("D\n");
		while (  fgets( oneline, 1024, f ) != NULL  )
		{
			//printf("%s",oneline);

			len = sstrlen(oneline);
			if ( len == 2 )
			{
				break;
			}
			if( oneline[0] == '[')
			{
				read_section( f, oneline );
				break;
			}
			i = 0;
			while ( oneline[i] == ' ' )
			{
				i++;
			}
			if ( oneline[i] == '/' || oneline[i] == '\n')
			{
				continue;
			}

			pos = find_pos(oneline);
			//printf("%d+%c\n",pos,oneline[pos]);
		
			if (pos == 11 && oneline[0] == 'H' )
			{
				pos++;
				while( oneline[pos] == ' ')
					pos++;
				len = sstrlen(&oneline[pos]);
				difficulty.HPDrainRate = getnum(&oneline[pos],len-1);
				//printf("%d\n", difficulty.HPDrainRate);

			}
			if (pos == 10 && oneline[0] == 'C' )
			{
				pos++;
				while( oneline[pos] == ' ')
					pos++;
				len = sstrlen(&oneline[pos]);
				difficulty.CircleSize = getnum(&oneline[pos],len-1);
				//printf("%d\n", difficulty.CircleSize);

			}
			if (pos == 17 && oneline[0] == 'O' )
			{
				pos++;
				while( oneline[pos] == ' ')
					pos++;
				len = sstrlen(&oneline[pos]);
				difficulty.OverallDifficulty = getnum(&oneline[pos],len-1);
				//printf("%d\n", difficulty.OverallDifficulty);

			}
			if (pos == 16 && oneline[0] == 'S' )
			{
				pos++;
				while( oneline[pos] == ' ')
					pos++;
				len = sstrlen(&oneline[pos]);
				difficulty.SliderMultiplier = getnum2(&oneline[pos],len-1);
				//printf("%f\n", difficulty.SliderMultiplier);

			}
			if (pos == 14 && oneline[0] == 'S' )
			{
				pos++;
				while( oneline[pos] == ' ')
					pos++;
				len = sstrlen(&oneline[pos]);
				difficulty.SliderTickRate = getnum2(&oneline[pos],len-1);
				printf("hzq: %f\n", difficulty.SliderTickRate);

			}
		}
	}
	else if ( oneline[1] == 'T' )
	{
	/**
		* deal with Event
		*/
		struct TimingPoints * timingPointsP;
		//printf("T\n");
		while (  fgets( oneline, 1024, f ) != NULL  )
		{
			//printf("%s",oneline);

			len = sstrlen(oneline);
			if ( len == 1 )
			{
				continue;
			}
			if( oneline[0] == '[')
			{
				read_section( f, oneline );
				break;
			}
			i = 0;
			while ( oneline[i] == ' ' )
			{
				i++;
			}
			if ( oneline[i] == '/' || oneline[i] == '\n')
			{
				continue;
			}
			fill_timingPoints(&oneline[pos],&timingPoints,0);
			timingPointsP = malloc(sizeof(struct TimingPoints));
			*timingPointsP = timingPoints;
		//	printf("hzq: %f %.10f  %d %d %d %d %d %d\n",timingPoints.offset, timingPoints.beatLength, timingPoints.timingSignature,timingPoints.sampleSetId,timingPoints.useCustomSampleas,timingPoints.sampleVolume,timingPoints.notInherited,timingPoints.isKiai);
			TAILQ_INSERT_TAIL(&TimingPointsHead, timingPointsP, Tentry);
			printf("HZQ!!!!!\n");

		}
	}
	else if ( oneline[1] == 'E' )
	{
		/**
		* deal with Event
		*/
		//printf("E\n");
		while (  fgets( oneline, 1024, f ) != NULL  )
		{
			//printf("%s",oneline);

			len = sstrlen(oneline);
			if ( len == 2 )
			{
				continue;
			}
			if( oneline[0] == '[')
			{
				read_section( f, oneline );
				break;
			}
			i = 0;
			while ( oneline[i] == ' ' )
			{
				i++;
			}
			if (oneline[i] == '\n')
			{
				continue;
			}
			if ( oneline[i] == '/' && sstrlen(oneline) == 30)
			{
				//printf("hzq");
				//printf("%s %d\n",oneline,strlen(oneline));
 			 	while (fgets( oneline, 1024, f ) != NULL)
				{
					if (oneline[0] == '/')
						break;
					if (oneline[0] == '[')
					{
						read_section( f, oneline );
						break;
					}
					/*
					sscanf(oneline,"%d,%d,\"%s",&tmp,&i,backGroundOrVidoe);
					if (tmp == 0)
					{
						strcpy(backGround,backGroundOrVidoe);
						backGround[strlen(backGround)-1]='\0';
					//	printf("backGround: %s\n",backGround);
					}
					if (tmp == 1)
					{
						strcpy(video,backGroundOrVidoe);
						video[strlen(video)-1]='\0';
					//	printf("video: %s\n",video);
					}
					*/
					if ( oneline[0] == '0' || oneline[0] == 'b' || oneline[0] == 'B')
					{
						tmp = find_pos2(oneline);
						oneline = &oneline[tmp+1];
						tmp = find_pos2(oneline);
						strcpy(backGround,&oneline[tmp+2]);
						backGround[sstrlen(backGround)-2]='\0';
	//					printf("hzq: %s \n",backGround);
					}
					if ( oneline[0] == '0' || oneline[0] == 'v' || oneline[0] == 'V')
					{
						tmp = find_pos2(oneline);
						oneline =&oneline[tmp+1];
						tmp = find_pos2(oneline);
						strcpy(video,&oneline[tmp+2]);
						video[sstrlen(video)-2]='\0';
	//					printf("hzq: %s \n",video);
					}


				}
			}
		}

	}
	else if ( oneline[1] == 'C' )
	{
		/**
		* deal with colour
		*/

		struct Color_r * colorP;
		//printf("C\n");
		while (  fgets( oneline, 1024, f ) != NULL  )
		{
			//printf("%s",oneline);

			len = sstrlen(oneline);
			if ( len == 1 )
			{
				continue;
			}
			if( oneline[0] == '[')
			{
				read_section( f, oneline );
				break;
			}
			i = 0;
			while ( oneline[i] == ' ' )
			{
				i++;
			}
			if ( oneline[i] == '/' || oneline[i] == '\n')
			{
				continue;
			}
			pos = find_pos(oneline);
			//printf("%d+%c\n",pos,oneline[pos]);
			while ( oneline[pos+1] == ' ')
			{
				pos++;
			}
			fill_color(&oneline[pos+1],&color,0);
			colorP = malloc(sizeof(struct Color_r));
			printf("%d %d %d\n",color.r,color.g,color.b);
			*colorP = color;
			TAILQ_INSERT_TAIL(&ComboHead, colorP, Centry);

		}

	}
	else if ( oneline[1] == 'H' )
	{
		/**
		* deal with HitObject
		*/
		struct HitObjects * hitObjectsP;
		//printf("H\n");
		while (  fgets( oneline, 1024, f ) != NULL  )
		{
			//printf("%s",oneline);

			len = sstrlen(oneline);
			if ( len == 2 )
			{
				continue;
			}
			if( oneline[0] == '[')
			{
				read_section( f, oneline );
				break;
			}
			i = 0;
			while ( oneline[i] == ' ' )
			{
				i++;
			}
			if ( oneline[i] == '/' || oneline[i] == '\n')
			{
				continue;
			}
			counter = count(oneline);
		//	//printf("%d\n",counter);
		//	//printf("%s\n",oneline);
			if (counter == 4)
			{
				hitObjectsP = malloc(sizeof(struct HitObjects));
				
				fill_hit_circle( oneline , &hitCircle, 0);
				hitObjectsP->type = 0;
				hitObjectsP->hitCircle = hitCircle;
				//printf("hzq: %d %d %d %d %d\n",hitCircle.xPos,hitCircle.yPos,hitCircle.startTimeMs,hitCircle.objectType,hitCircle.soundType);

				TAILQ_INSERT_TAIL(&HitQueueHead, hitObjectsP, Hentry);
				
			}
			if (counter==7 )
			{
				hitObjectsP = malloc(sizeof(struct HitObjects));
				
				fill_slider( oneline, &slider, 0);
				hitObjectsP->type = 1;
				hitObjectsP->slider = slider;
				TAILQ_INSERT_TAIL(&HitQueueHead, hitObjectsP, Hentry);
				//printf("hzq:%d %d %d %d %d %d %d %c\n", slider.xPos,slider.yPos,slider.startTimeMs,slider.objectType,slider.soundType,slider.repeatCount, slider.sliderLengthPixels, slider.curveType);
				//i=0;
				//while( i < slider.posNum)
				//{
				//	printf("hzq: %d:%d\n",slider.pos[i][0],slider.pos[i][1]);
				//	i++;
				//}
			}
			if (counter > 7 )
			{
				hitObjectsP = malloc(sizeof(struct HitObjects));
				
				fill_slider2( oneline, &slider, 0);
				hitObjectsP->type = 1;
				hitObjectsP->slider = slider;
				TAILQ_INSERT_TAIL(&HitQueueHead, hitObjectsP, Hentry);
				//printf("%d %d %d %d %d %d %d %c\n", slider.xPos,slider.yPos,slider.startTimeMs,slider.objectType,slider.soundType,slider.repeatCount, slider.sliderLengthPixels, slider.curveType);
				//i=0;
				//while( i < slider.posNum)
				//{
				//	printf("%d:%d\n",slider.pos[i][0],slider.pos[i][1]);
				//	i++;
				//}
			}


		}

	}
	
}
// return the message of hitObjects
void
osu_read(FILE *in)
{
	char oneline[1024];
	int len=0,i,j;
	int pos;
	FILE *f = in;
	backGround[0] = '\0';
	video[0] = '\0';
	TAILQ_INIT(&HitQueueHead);
	TAILQ_INIT(&ComboHead);
	TAILQ_INIT(&TimingPointsHead);
	if ( f == NULL )
	{
		perror("Fail to open");
		exit(1);
	}
	while ( fgets( oneline, 200, f ) != NULL )
	{
		len = sstrlen(oneline);
//		//printf("%d\n", len );
		//printf("%s", oneline );
		if ( len == 2 )
		{
			continue;
		}
		i = 0;
		while ( oneline[i] == ' ' )
		{
			i++;
		}
		if ( oneline[i] == '/' || oneline[i] == '\n')
		{
			continue;
		}
//		//printf("useful info\n");
		if ( oneline[0] == '[')
		{
			read_section( f, oneline  );
		}
	}

}
struct ObjectList *
osu_get_object_list(void){
	return &HitQueueHead; 
}

struct ColorList *
osu_get_color_list(void)
{
	return &ComboHead;
}

struct General *
osu_get_general(void)
{
	return &general;
}

struct Difficulty *
osu_get_difficulty(void)
{
	return &difficulty;
}

struct TimingPointsList *
osu_get_timing_points(void)
{
	return &TimingPointsHead;
}
int
is_HitCircle(struct HitObjects *a)
{
	if(a->type == 0)
		return 1;
	return 0;
}

char * 
osu_get_backGround(void)
{
	return backGround;
}

char * 
osu_get_video(void)
{
	return video;
}
/*
int main()
{
	char osu[50];
	int i;
	struct HitObjects *b;
	struct Color_r *d;
	scanf("%s",osu)	;
	read_point_osu(osu);
	//printf("%s %d %d %d %d %f %d %d \n",general.AudioFilename, general.AudioLeadIn, general.PreviewTime, general.Countdown, general.SampleSet , general.StackLeniency, general.Mode, general.LetterboxInBreaks );
	TAILQ_FOREACH(d, &ComboHead, Centry)
	{
		//printf("5D %d %d \n",d->R,d->G,d->B);
	}
	TAILQ_FOREACH(b, &HitQueueHead, Hentry)
	{
		if(b->type == 0)
		{
			struct HitCircle *a = &b->hitCircle;
			//printf("%d, %d, %d, %d %d\n", a->xPos, a->yPos, a->startTimeMs, a->objectType, a->soundType);
		}
		else{
			struct Slider_r *a = &b->slider;
			//printf("%d, %d, %d, %d, %d, %c|", a->xPos, a->yPos, a->startTimeMs, a->objectType, a->soundType, a->curveType);
			for(i = 0 ; i < a->posNum ; i ++){
				//printf("%d:%d|", a->pos[i][0], a->pos[i][1]);
			}
			//printf("%d, %d\n", a->repeatCount, a->sliderLengthPixels);
		}
	}
	return 0;
}*/
