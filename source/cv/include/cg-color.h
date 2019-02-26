#ifndef RAWCV_COLOR_H
#define RAWCV_COLOR_H

#include "cg-cv.h"

typedef struct RGBColor{
  unsigned char r, g, b;
}RGBColor;

typedef struct HSVColor{
  unsigned char h, s, v;
}HSVColor;

CG_CV_API HSVColor Color_rgbToHSV(RGBColor);
CG_CV_API RGBColor Color_hsvToRGB(HSVColor);

#endif
