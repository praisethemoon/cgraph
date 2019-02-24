/** \file cg_api.c
 *  \brief External API implementation
 */

#include <inttypes.h>
#include "cg-image.h"

uint32_t cgv_getImageHeight(struct CGImage *img)
{
	return img->width;
}

uint32_t cgv_getImageWidth(struct CGImage *img)
{
	return img->height;
}

uint32_t cgv_getImageDepth(struct CGImage *img)
{
	return img->cpp;
}

uint8_t *cgv_getImagePixels(struct CGImage *img)
{
	return img->data;
}

struct CGImage *cgv_newImage()
{
	return Image_new();
}

struct CGImage *cgv_imageCopy(struct CGImage *src)
{
	return Image_copy(src);
}

struct CGImage *cgv_newRandomImage(uint32_t width, uint32_t height, uint32_t cpp){
	return Image_randomImage(width, height, cpp);
}

void cgv_freeImage(struct CGImage* img){
	Image_delete(img);
}

int cgv_loadImageFromSource(struct CGImage* img, const char* source){
	return Image_loadFromSource(img, source);
}

int cgv_saveImage(struct CGImage* img, const char* url){
	return Image_saveToFile(img, url);
}

void cgv_debugImage(struct CGImage* img){
	Image_debug(img);
}

void cgv_ImagePixelAt(struct CGImage* source, uint32_t x, uint32_t y, uint8_t *c){
	Image_pixelAt(source, x, y, c);
}

void cgv_imageToGrayscale(struct CGImage* source, struct CGImage* target){
	Image_colorToGrayscale(source, target);
}

void cgv_colorImageToBW(struct CGImage* source, struct CGImage* target, uint8_t threshold){
	Image_colorToBW(source, target, threshold);
}

void cgv_grayscaleImageToBW(struct CGImage* source, struct CGImage* target, uint8_t threshold){
	Image_grayscaleToBW(source, target, threshold);
}

void cgv_imageToRGB(struct CGImage* source, struct CGImage *target){
	Image_toRGB(source, target);
}

void cgv_imageToRGBA(struct CGImage* source, struct CGImage *target){
	Image_toRGBA(source, target);
}

void cgv_imageHistogram(struct CGImage* source, unsigned long* hist){
	Image_histogram(source, hist);
}

void cgv_imageDrawPixel(struct CGImage* source, uint32_t x, uint32_t y, uint8_t * color,  int wd){
	Image_drawPixel(source, x, y, color, wd);
}

void cgv_imageDrawLine(struct CGImage* source, uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1, uint8_t * color, int wd){
	Image_drawLine(source, x0, y0, x1, y1, color, wd);
}

void cgv_imageDrawCircle(struct CGImage* source, uint32_t xm, uint32_t ym, uint32_t r,  uint8_t* color, int wd){
	Image_drawCircle(source, xm, ym, r, color, wd);
}

struct CGImage* cgv_imageGetChannel(struct CGImage* source, uint8_t c){
	return Image_extractColorFromRGB(source, c);
}

struct CGImage** cgv_imageSplitByChannel(struct CGImage* source){
	return Image_extractAllColorsFromRGB(source);
}
