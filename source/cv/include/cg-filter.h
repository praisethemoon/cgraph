#ifndef RAWCV_FILTERS_H
#define RAWCV_FILTERS_H

#include "cg-image.h"

#define FILTER_3X3 = {{1, 1, 1}, {1, 1, 1}, {1, 1, 1}}

/* more at: http://docs.opencv.org/2.4/modules/imgproc/doc/filtering.html */

/* in case of multi channel images, each channel in processed independently */
rawcvapi void Filter_apply3x3(Image *src, int[3][3] filter);

/*
 * Median Filter.
 * keeps border values unchanges 
 * @param src [out] Image
 */
//CG_CV_API void Filter_median(Image *src, )

CG_CV_API void Filter_dilate(Image *src, int factor);
CG_CV_API void Filter_erode(Image *src, int factor);
CG_CV_API void Filter_gaussian(Image *src, int factor);
CG_CV_API void Filter_sobel(Image, int factor);

#endif