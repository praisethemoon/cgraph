#ifndef CGRAPH_API_H
#define CGRAPH_API_H

/** \file cg_api.h
 *  \brief External API exposed to users
 *         which provides opaque data types.
 */


#include <inttypes.h>


#define CGRAPH_API

struct CGImage;

CGRAPH_API uint32_t cgv_getImageHeight(struct CGImage* img);
CGRAPH_API uint32_t cgv_getImageWidth(struct CGImage* img);
CGRAPH_API uint32_t cgv_getImageDepth(struct CGImage* img);
CGRAPH_API uint8_t* cgv_getImagePixels(struct CGImage* img);


/* 
 * Image constructor 
 * Creates an empty 0-initialized image
 */
CGRAPH_API struct CGImage* cgv_newImage();

/*
 * Create a copy from another image
 * @param img [in]source image
 */
 CGRAPH_API struct CGImage* cgv_imageCopy(struct CGImage* src);

 /*
  * Creates a random Image
  * @param width image width
  * @param height image height
  * @param cpp Channels
  */
CGRAPH_API struct CGImage* cgv_newRandomImage(uint32_t width, uint32_t height, uint32_t cpp);
 
/*
 * Destructor
 * Deletes & frees the image's both raw pointer and content
 */
CGRAPH_API void cgv_freeImage(struct CGImage*);


/* 
 * Load an image from file, 
 * expect a new instance of Image
 */
CGRAPH_API int cgv_loadImageFromSource(struct CGImage* img, const char* source);

/*
 * Save an image as PNG to a file
 */
CGRAPH_API int cgv_saveImage(struct CGImage* img, const char* url);

/*
 * Prints Image info to stdout
 */
CGRAPH_API void cgv_debugImage(struct CGImage* img);

/*
 * Get RGB Pixel at given poisition
 * @param source [in] Image source 3 bits
 * @param x [in] pixel x raw
 * @param y [in] pixel y column
 * @param c [out] color, must be same size as image's CPP
 */
CGRAPH_API void cgv_ImagePixelAt(struct CGImage* source, uint32_t x, uint32_t y, uint8_t *c);

/*
 * Transforms a 3/4 color Space to Grayscale image
 */
CGRAPH_API void cgv_imageToGrayscale(struct CGImage* source, struct CGImage* target);

/*
 * Transforms a 3/4 color space to black & white image
 */
CGRAPH_API void cgv_ImageToBW(struct CGImage* source, struct CGImage* target, uint8_t threshold);

/*
 * Transforms a Grayscale image to black & white
 */
CGRAPH_API void cgv_grayscaleImageToBW(struct CGImage* source, struct CGImage* target, uint8_t threshold);

/* 
 * transforms any channel image to 3 channel image (Red, Green, Blue)
 */
CGRAPH_API void cgv_imageToRGB(struct CGImage* source, struct CGImage *target);


/* 
 * transforms any channel image to 4 channel image (Red, Green, Blue, Alpha)
 */
CGRAPH_API void cgv_imageToRGBA(struct CGImage* source, struct CGImage *target);

/*
 * Calculates Image histogram. Size of the histogram is the image's CPP * 255
 * @param source [in] Input image
 * @param hist [out] output histogram. Must be NULL.
 */
CGRAPH_API void cgv_imageHistogram(struct CGImage* source, unsigned long* hist);

/**
 * \brief Draws a pixel on an image
 * \param image [out] input image
 * \param x [in] pixel col
 * \param y [in] pixel row
 * \param color [in] pixel color
 * \param wd [in] pixel width
 */
CGRAPH_API void cgv_imageDrawPixel(struct CGImage* source, uint32_t x, uint32_t y, uint8_t * color,  int wd);

/**
 * \brief Draws a line on an image using Bresenham's algorithm. Implementation taken from: http://members.chello.at/~easyfilter/bresenham.html
 * \param source[out] Input image
 * \param x0 line starting col
 * \param y0 line starting row
 * \param x1 line ending col
 * \param y1 line ending row
 * \param color [in] pixel color
 * \param wd [in] pixel width
 */
CGRAPH_API void cgv_imageDrawLine(struct CGImage* source, uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1, uint8_t * color, int wd);

/**
 * \brief Draws a circle on an image using Bresenham's algorithm. Implementation taken from: http://members.chello.at/~easyfilter/bresenham.html
 * \param source[out] Input image
 * \param xm circle center col
 * \param ym circle center row
 * \param r circle radius
 * \param y1 line ending row
 * \param color [in] pixel color
 * \param wd [in] pixel width
 */
CGRAPH_API void cgv_imageDrawCircle(struct CGImage* source, uint32_t xm, uint32_t ym, uint32_t r,  uint8_t* color, int wd);

/**
 * \brief Extracts entire color-image from an image
 * \param source source
 * \param c channel
 * \return an RGB Image.
 */
CGRAPH_API struct CGImage* cgv_imageGetChannel(struct CGImage* source, uint8_t c);

/**
 * \brief Extracts all color-image from an image
 * \param source source
 * \return a 3-sized array of RGB Image.
 */
CGRAPH_API struct CGImage** cgv_imageSplitByChannel(struct CGImage* source);

#endif
