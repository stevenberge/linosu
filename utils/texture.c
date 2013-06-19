/**
 * Project Linosu!
 * texture.c: functions encapsuled texture loading,
 * creating and mapping.
 * Author: Brills (brillsp@gmail.com)
 * History: 4/4/2011: first version
 */

#include <stdio.h>
#include <assert.h>
#include <jpeglib.h>

#include <inc/texture.h>
static int
is_png(const char *f)
{
	int len = strlen(f);
	return !strcasecmp(f + len - 3, "png");
}

static int
is_jpeg(const char *f)
{
	int len = strlen(f);
	return !strcasecmp(f + len - 3, "jpg") || !strcasecmp(f + len - 4, "jpeg");
}
/**
 * png_load function:
 * given png file name, load the image into
 * a matrix pointed by the return value,
 * the width and height will store into w_store and 
 * h_store, if they are not null.
 *
 *@param filename: png filename (path included)
 *@param w_store: width
 *@param h_store: height
 *
 * returns:
 * NULL on error, and the error message will be printed
 * into stderr.
 *
 * void* points to the loaded image.
 */
static void *
png_load(const char *filename, size_t *w_store, size_t *h_store)
{
	unsigned char *buffer;
	unsigned char *image;
	size_t buffersize, imagesize ;
	LodePNG_Decoder decoder;
	LodePNG_loadFile(&buffer, &buffersize, filename);
	LodePNG_Decoder_init(&decoder);
	//buffer[buffersize]经过decoder输出image[imagesize].
	//注意输出型参数都是额外增加一层指针(&),输入型参数则不需要
	LodePNG_Decoder_decode(&decoder, &image, &imagesize, buffer, buffersize);
	if(decoder.error) //错误码非0，有异常
	{
		fprintf(stderr,"error %u: %s\n", decoder.error, LodePNG_error_text(decoder.error));
		return NULL;
	}
	//if(w_store!=null) *w_store=...
	w_store ? *w_store = decoder.infoPng.width : 0;
	h_store ? *h_store = decoder.infoPng.height : 0; 
	free(buffer);
	return image;
}

static void *
jpeg_load(const char *filename, size_t *w_store, size_t *h_store)
{
	/* these are standard libjpeg structures for reading(decompression) */
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
	unsigned char *raw_image;
	/* libjpeg data structure for storing one row, that is, scanline of an image */
	JSAMPROW row_pointer[1];
	
	FILE *infile = fopen(filename, "rb");
	unsigned long location = 0;
	int i = 0;
	
	if ( !infile )
	{
		printf("Error opening jpeg file %s\n!", filename );
		return NULL;
	}
	/* here we set up the standard libjpeg error handler */
	cinfo.err = jpeg_std_error( &jerr );
	/* setup decompression process and source, then read JPEG header */
	jpeg_create_decompress( &cinfo );
	/* this makes the library read from infile */
	jpeg_stdio_src( &cinfo, infile );
	/* reading the image header which contains image information */
	jpeg_read_header( &cinfo, TRUE );
	/* Uncomment the following to output image information, if needed. */
	printf( "JPEG File Information: \n" );
	printf( "Image width and height: %d pixels and %d pixels.\n", cinfo.image_width, cinfo.image_height );
	printf( "Color components per pixel: %d.\n", cinfo.num_components );
	printf( "Color space: %d.\n", cinfo.jpeg_color_space );
	/* Start decompression jpeg here */
	jpeg_start_decompress( &cinfo );

	/* allocate memory to hold the uncompressed image */
	raw_image = (unsigned char*)malloc( cinfo.output_width*cinfo.output_height*4 );
	/* now actually read the jpeg into the raw buffer */
	row_pointer[0] = (unsigned char *)malloc( cinfo.output_width*cinfo.num_components );
	/* read one scan line at a time */
	while( cinfo.output_scanline < cinfo.image_height )
	{
		jpeg_read_scanlines( &cinfo, row_pointer, 1 );
		for( i=0; i<cinfo.image_width*cinfo.num_components;){ 
			if(location % 4 == 3){
				raw_image[location++] = 255;
				continue;
			}
			raw_image[location++] = row_pointer[0][i];
			i++;
		}
	}
	*w_store = cinfo.image_width;
	*h_store = cinfo.image_height;
	/* wrap up decompression, destroy objects, free pointers and close open files */
	jpeg_finish_decompress( &cinfo );
	jpeg_destroy_decompress( &cinfo );
	free( row_pointer[0] );
	fclose( infile );
	/* yup, we succeeded! */
	return raw_image;
}
/**
 * texture_bind_image function
 * given a pointer to an image and its size,
 * allocate a texture and bind it to the image
 *
 * @param *img: pointer to image
 * @param width
 * @param height
 * 
 * returns:
 * the texture handle
 */
GLuint
texture_bind_image(void *img, size_t width, size_t height)
{
	GLuint texname;
	glGenTextures(1, &texname);
	glBindTexture(GL_TEXTURE_2D, texname);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width
			, height, 0, GL_RGBA, 
			GL_UNSIGNED_BYTE, img);
	return texname;
}
static void 
png_fill(void *img, size_t w, size_t h, unsigned char r, unsigned char g, unsigned char b)
{
	unsigned char *p = img, *ptr;
	int i,j;
	float alpha;
	for (i = 0; i < h; i++)
		for(j = 0; j < w; j++){
				
				ptr = p + i * w * 4 + j * 4;
				alpha = ptr[3]/255.0;//第四个字节ptr[3]保存的是灰度
				ptr[0] = r*alpha, ptr[1] = g*alpha, ptr[2] = b*alpha;
			}
}
static void
png_fill_alpha(void *img, size_t w, size_t h, float alpha)
{
	unsigned char *p = img, *ptr;
	int i,j;
	for (i = 0; i < h; i++)
		for(j = 0; j < w; j++){
				ptr = p + i * w * 4 + j * 4;
				ptr[0] *= alpha, ptr[1] *= alpha, ptr[2] *= alpha;
	//			ptr[3] = 255*alpha;
			}
}
/**
 * texture_load function
 * given a png file name,
 * load the image file and bind it to a texture
 * 
 * @param *filename: png filename (path included)
 * 
 * returns:
 * the texture handle
 */
GLuint
texture_load(const char *filename)
{
	void *img = NULL;
	GLuint texname;
	size_t w,h;
	if (is_png(filename))
		img = png_load(filename, &w, &h);
	else if (is_jpeg(filename))
		img = jpeg_load(filename, &w, &h);
	
	assert(img);
	return texture_bind_image(img, w, h);
}
/**
 * texture_load_with_color function
 * given a png file name,
 * load the image file, fill the image's non-transprent
 * pixels with given color, and bind it to a texture
 * 
 * @param *filename: png filename (path included)
 * @param r, g, b: channels of given color
 * returns:
 * the texture handle
 */
GLuint
texture_load_with_color(const char *filename, unsigned char r, unsigned char g, unsigned char b)
{
	void *img = NULL;
	GLuint texname;
	size_t w,h;
	if (is_png(filename))
		img = png_load(filename, &w, &h);
	else if (is_jpeg(filename))
		img = jpeg_load(filename, &w, &h);
	assert(img);
	png_fill(img, w, h, r, g, b);
	return texture_bind_image(img, w, h);
}

GLuint
texture_load_with_alpha(const char *filename, float alpha)
{
	void *img = NULL;
	GLuint texname;
	size_t w,h;
	if (is_png(filename))
		img = png_load(filename, &w, &h);
	else if (is_jpeg(filename))
		img = jpeg_load(filename, &w, &h);
	assert(img);
	png_fill_alpha(img, w, h, alpha);
	
	return texture_bind_image(img, w, h);
}

