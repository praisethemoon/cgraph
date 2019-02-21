#include <stdlib.h>

#include "cg-color.h"


#define ASSERT(test, msg, ...)\
{\
 if(!test){\
  fprintf(stderr, msg);\
  exit(-1);\
 }\
}

/*
 * Image API & Data Structure 
 * 
 */

typedef struct CGImage {
  int width, height, cpp;
  unsigned char *data;
}CGImage;

/* 
 * Image constructor 
 * Creates an empty 0-initialized image
 */
CGImage* Image_new();

/*
 * Create a copy from another image
 * @param img [in]source image
 */
 CGImage* Image_copy(CGImage* src);

 /*
  * Creates a random Image
  * @param width image width
  * @param height image height
  * @param cpp Channels
  */
 CGImage* Image_randomImage(int width, int height, int cpp);
 
/*
 * Destructor
 * Deletes & frees the image's both raw pointer and content
 */
void Image_delete(CGImage*);


/* 
 * Load an image from file, 
 * expect a new instance of Image
 */
int Image_loadFromSource(CGImage* img, const char* source);

/*
 * Save an image as PNG to a file
 */
int Image_saveToFile(CGImage* img, const char* url);

/*
 * Prints Image info to stdout
 */
void Image_debug(CGImage* img);

/*
 * Get RGB Pixel at given poisition
 * @param source [in] Image source 3 bits
 * @param x [in] pixel x raw
 * @param y [in] pixel y column
 * @param c [out] color, must be same size as image's CPP
 */
void Image_pixelAt(CGImage* source, int x, int y, unsigned char *c);

/*
 * Transforms a 3/4 color Space to Grayscale image
 */
void Image_colorToGrayscale(CGImage* source, CGImage* target);

/*
 * Transforms a 3/4 color space to black & white image
 */
void Image_colorToBW(CGImage* source, CGImage* target, unsigned char threshold);

/*
 * Transforms a Grayscale image to black & white
 */
void Image_grayscaleToBW(CGImage* source, CGImage* target, unsigned char threshold);

/* 
 * transforms any channel image to 3 channel image (Red, Green, Blue)
 */
void Image_toRGB(CGImage* source, CGImage *target);


/* 
 * transforms any channel image to 4 channel image (Red, Green, Blue, Alpha)
 */
void Image_toRGBA(CGImage* source, CGImage *target);

/*
 * Calculates Image histogram. Size of the histogram is the image's CPP * 255
 * @param source [in] Input image
 * @param hist [out] output histogram. Must be NULL.
 */
void Image_histogram(CGImage* source, unsigned long* hist);

/*
 * Draws a pixel on an image
 * @param image [out] input image
 * @param x [in] pixel col
 * @param y [in] pixel row
 * @param color [in] pixel color
 * @param wd [in] pixel width
 */
void Image_drawPixel(CGImage* source, int x, int y, unsigned char * color,  int wd);

/*
 * Draws a line on an image using Bresenham's algorithm.
 * Implementation taken from: http://members.chello.at/~easyfilter/bresenham.html
 * @param source[out] Input image
 * @param x0 line starting col
 * @param y0 line starting row
 * @param x1 line ending col
 * @param y1 line ending row
 * @param color [in] pixel color
 * @param wd [in] pixel width
 */
void Image_drawLine(CGImage* source, int x0, int y0, int x1, int y1, unsigned char * color, int wd);

/*
 * Draws a circle on an image using Bresenham's algorithm.
 * Implementation taken from: http://members.chello.at/~easyfilter/bresenham.html
 * @param source[out] Input image
 * @param xm circle center col
 * @param ym circle center row
 * @param r circle radius
 * @param y1 line ending row
 * @param color [in] pixel color
 * @param wd [in] pixel width
 */
void Image_drawCircle(CGImage* source, int xm, int ym, int r,  unsigned char * color, int wd);

/*
 * Extracts entire color-image from an image
 * @param source source
 * @param c channel
 * returns an RGB Image.
 */
CGImage* Image_extractColorFromRGB(CGImage* source, int c);

/*
 * Extracts all color-image from an image
 * @param source source
 * returns an 3-sized array of RGB Image.
 */
CGImage** Image_extractAllColorsFromRGB(CGImage* source);















