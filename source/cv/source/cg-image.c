#include <stdlib.h>
#include <assert.h>
#include <stddef.h>
#include <time.h>

#define STB_IMAGE_IMPLEMENTATION
#include "private/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "private/stb_image_write.h"

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "private/stb_image_resize.h"

#include "cg-color.h"
#include "cg-image.h"

CGImage* CGImage_new()
{
  CGImage *img = calloc(1, sizeof(CGImage));
  return img;
}


CGImage* CGImage_copy(CGImage *src)
{
  CGImage *dest = calloc(1, sizeof(CGImage));
  dest->cpp = src->cpp;
  dest->height = src->height;
  dest->width = src->width;

  long size = src->height*src->width*src->cpp;
  dest->data = (unsigned char*)calloc(size, sizeof(unsigned char));
  
  memcpy(dest->data, src->data, size*sizeof(unsigned char));

  return dest;
}

CGImage* CGImage_randomImage(int width, int height, int cpp)
{
    ASSERT((cpp == 1) || (cpp == 3) || (cpp == 4), "Image channels can only be 1, 3 and 4. %d was given instead.", cpp);
    CGImage* img = CGImage_new();
    
    img->width = width;
    img->height = height;
    img->cpp = cpp;
    
    img->data = calloc(width*height*cpp, sizeof(unsigned char));
    
    srand ( time(NULL) );
    
    for(size_t i = 0; i < width*height*cpp; i++)
    {
        img->data[i] = (unsigned char)((rand()/(float)RAND_MAX)*255.0f);
    }
    
    return img;
}

void CGImage_delete(CGImage* img)
{
  free(img->data);
  free(img);
}


int CGImage_loadFromSource(CGImage* img, const char *source)
{
  assert(img != NULL);
  
  if(img->data = stbi_load(source, &img->width, &img->height, &img->cpp, 0)){
    return 0;
  }

  return 1;
}

int CGImage_saveToFile(CGImage* img, const char* url)
{
  stbi_write_png(url, img->width, img->height, img->cpp, img->data, 0);
  /* TODO: check if creation went succesfull */
  return 0;
}

/* 
 * TODO: this might cause segfault if #c < source->cpp
 */
void CGImage_pixelAt(CGImage* source, int i, int j, unsigned char *c)
{
  assert(source != NULL);
  assert(c != NULL);
  
  size_t k = 0;
  
  for(; k < source->cpp; k++)
  {
    c[k] = source->data[i*source->width*source->cpp + j*source->cpp + k];
  }
}

void CGImage_debug(CGImage* img)
{
  assert(img != NULL);
  
  printf("Image raw pointer: %p\n\tWidth: %d\n\tHeight: %d\n\tComponent Per Pixel(8-bit): %d\n\tMemory Size: %zu bytes\n", img, img->width, img->height, img->cpp, (img->width*img->height*img->cpp*sizeof(unsigned char)));
}

void CGImage_colorToGrayscale(CGImage* source, CGImage* target)
{
  assert(source != NULL);
  assert(target != NULL);
  assert((source->cpp == 3) || (source->cpp == 4));
  
  target->cpp = 1;
  
  int height = (target->height = source->height);
  int width = (target->width = source->width);
  int cpp = source->cpp;

  target->data = calloc(width*height, sizeof(unsigned char));
  
  const unsigned char *sdata = source->data;
  
  size_t i = 0;
  
  for(; i < height*width*cpp; i+=cpp)
  {
    target->data[i/cpp] = ((sdata[i]+sdata[i+1]+sdata[i+2])/3);
  }
  
}

void CGImage_colorToBW(CGImage* source, CGImage* target, unsigned char threshold)
{
  assert(source != NULL);
  assert(target != NULL);
  assert((source->cpp == 3) || (source->cpp == 4));
  
  target->cpp = 1;
  
  int height = (target->height = source->height);
  int width = (target->width = source->width);
  int cpp = source->cpp;
  
  target->data = calloc(width*height, sizeof(unsigned char));
  
  const unsigned char *sdata = source->data;
  
  size_t i = 0;
  
  for(; i < height*width*cpp; i+=cpp)
  {
    target->data[i/cpp] = ((sdata[i]+sdata[i+1]+sdata[i+2])/3) >= threshold ? 255 : 0;
  }
  
}

void CGImage_grayscaleToBW(CGImage* source, CGImage* target, unsigned char threshold)
{
  assert(source != NULL);
  assert(target != NULL);
  assert(source->cpp == 1);
  
  target->cpp = 1;
  
  int height = (target->height = source->height);
  int width = (target->width = source->width);
  
  target->data = calloc(width*height, sizeof(unsigned char));
  
  const unsigned char *sdata = source->data;
  
  size_t i = 0;
  
  for(; i < height*width; i++)
  {
    target->data[i] = sdata[i] >= threshold ? 255 : 0;
  }  
}

/* 
 * TODO: More testing .. 
 */
void CGImage_toRGB(CGImage* source, CGImage *target)
{
  assert(source != NULL);
  assert(target != NULL);
  assert((target->cpp == 1) || (target->cpp == 4) || (target->cpp == 3));
  
  
  target->cpp = 3;
  
  int height = (target->height = source->height);
  int width = (target->width = source->width);
  
  target->data = calloc(width*height*3, sizeof(unsigned char));

  const unsigned char *sdata = source->data;

  size_t i = 0;
  size_t j = 0;
  
  if(target->cpp == 1) {
    for(; i < height*width; i+=3)
    {
      target->data[i]   = sdata[i];
      target->data[i+1] = sdata[i];
      target->data[i+2] = sdata[i];
    }  
  }
  else {
    for(; i < height*width*3; i+=3, j+=4)
    {
      target->data[i]   = sdata[j];
      target->data[i+1] = sdata[j+1];
      target->data[i+2] = sdata[j+2];
    } 
  }
}

/*
 * TODO: More testing needed to demonstrate the pertinance of the function
 */
void CGImage_histogram(CGImage* source, unsigned long* hist)
{
  assert(source != NULL);
  assert(hist == NULL);
  
  hist = calloc(256*source->cpp, sizeof(long));
    
  size_t i = 0;
  for(; i < source->width * source->height; i++)
  {
    size_t j = 0;
    for(; j < source->cpp; j++)
    {
      hist[ 256*j + source->data[i*source->cpp + j] ]++;
    }
  }
}

/*
 * TODO: color might cause seg faut
 */
void CGImage_drawPixel(CGImage* source, int x, int y, unsigned char *color, int wd)
{
  assert(source != NULL);
  assert(x < source->height);
  assert(y < source->width);
  assert(color != NULL);
  
  size_t k = 0;
  
  for(; k < source->cpp; k++)
  {
    source->data[x*source->width*source->cpp + y*source->cpp + k] = color[k];
  }
}

void CGImage_drawLine(CGImage* source, int x0, int y0, int x1, int y1, unsigned char *color, int wd)
{
  assert(source != NULL);
  assert(x0 < source->height);
  assert(x1 < source->height);
  assert(y0 < source->width);
  assert(y1 < source->width);
  int dx =  abs(x1-x0), sx = x0<x1 ? 1 : -1;
   int dy = -abs(y1-y0), sy = y0<y1 ? 1 : -1; 
   int err = dx+dy, e2; /* error value e_xy */
 
   for(;;){  /* loop */
     CGImage_drawPixel(source, x0, y0, color, wd);
      if (x0==x1 && y0==y1) break;
      e2 = 2*err;
      if (e2 >= dy) { err += dy; x0 += sx; } /* e_xy+e_x > 0 */
      if (e2 <= dx) { err += dx; y0 += sy; } /* e_xy+e_y < 0 */
   }
}

void CGImage_drawCircle(CGImage* source, int xm, int ym, int r,  unsigned char * color, int wd)
{
    int x = -r, y = 0, err = 2-2*r; /* II. Quadrant */ 
    do {
        CGImage_drawPixel(source, xm-x, ym+y, color, wd); /*   I. Quadrant */
        CGImage_drawPixel(source, xm-y, ym-x, color, wd); /*  II. Quadrant */
        CGImage_drawPixel(source, xm+x, ym-y, color, wd); /* III. Quadrant */
        CGImage_drawPixel(source, xm+y, ym+x, color, wd); /*  IV. Quadrant */
        r = err;
        if (r <= y) 
            err += ++y*2+1;           /* e_xy+e_y < 0 */
            if (r > x || err > y)
                err += ++x*2+1; /* e_xy+e_x > 0 or no 2nd y-step */
    } while (x < 0);
}

CGImage* CGImage_extractColorFromRGB(CGImage* source, int c)
{
    ASSERT(source != NULL, "Source Image can't be null");
    ASSERT(source->cpp == 3, "Image given is not RGB");
    ASSERT(c < source->cpp, "Wrong channel specified. Expected < %d but got %d", source->cpp, c);
    
    CGImage* dest = CGImage_new();
    
    dest->height = source->height;
    dest->width = source->width;
    dest->cpp = 3;
    
    size_t i = c;
    
    dest->data = calloc(source->width*source->height*source->cpp, sizeof(unsigned char));
    
    for(; i < source->width*source->height*source->cpp; i+=3)
    {
        dest->data[i] = source->data[i];
    }
    
    return dest;
}

CGImage** CGImage_extractAllColorsFromRGB(CGImage* source)
{
    ASSERT(source != NULL, "Source Image can't be null");
    ASSERT(source->cpp == 3, "Image given is not RGB");
    
    CGImage** dest = calloc(3, sizeof(CGImage*));
    
    dest[0] = CGImage_new();
    dest[1] = CGImage_new();
    dest[2] = CGImage_new();
    
    dest[0]->height = source->height;
    dest[0]->width = source->width;
    dest[0]->cpp = 3;
    dest[0]->data = calloc(source->width*source->height*source->cpp, sizeof(unsigned char));
    
    dest[1]->height = source->height;
    dest[1]->width = source->width;
    dest[1]->cpp = 3;
    dest[1]->data = calloc(source->width*source->height*source->cpp, sizeof(unsigned char));
    
    dest[2]->height = source->height;
    dest[2]->width = source->width;
    dest[2]->cpp = 3;
    dest[2]->data = calloc(source->width*source->height*source->cpp, sizeof(unsigned char));
    
    size_t i = 0;
        
    for(; i < source->width*source->height*3; i+=3)
    {
        dest[0]->data[i] = source->data[i];
        dest[1]->data[i+1] = source->data[i+1];
        dest[2]->data[i+2] = source->data[i+2];
    }
    
    return dest;
}




