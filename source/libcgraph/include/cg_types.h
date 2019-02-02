#ifndef CG_TYPES_H
#define CG_TYPES_H

#include <inttypes.h>

//#define CG_USE_OPENCL

#ifdef CG_USE_OPENCL
#include <cf4ocl2.h>

typedef enum CGDataLocation {
    CG_DATALOC_HOST_MEM=0,
    CG_DATALOC_DEVICE_MEM
}CGDataLocation;

#endif

#include "cg_enums.h"

#define cg_float float
#define cgcl_float cl_float

/**
 * \brief Double  object.
 */
typedef struct CGDouble {
	cg_float value;
#ifdef CG_USE_OPENCL
	CCLBuffer* buf;
	CGDataLocation loc;
#endif
}CGDouble;

/**
 * \brief Vector object metadata and values
 */
typedef struct CGVector {
	cg_float* data;
	uint64_t len;

#ifdef CG_USE_OPENCL
	CCLBuffer* buf;
	CGDataLocation loc;
#endif

	//cg_float* (*getSub)(uint64_t start, uint64_t end);
}CGVector;

/**
 * \brief Matrix object metadata and values
 */
typedef struct CGMatrix {
	cg_float* data;
	uint64_t cols;
	uint64_t rows;

#ifdef CG_USE_OPENCL
	CCLBuffer* buf;
	CGDataLocation loc;
#endif

	//cg_float* (*getRow)(uint64_t row);
	//cg_float* (*getCol)(uint64_t col);
	//cg_float* (*getPatch)(uint64_t row, uint64_t x_size, uint64_t col, uint64_t y_size);
}CGMatrix;


/**
 * \brief 3D Matrix object metadata and values
 */
typedef struct CG3DMatrix {
	cg_float* data;

#ifdef CG_USE_OPENCL
	CCLBuffer* buf;
#endif

	uint64_t cols;
	uint64_t rows;
	uint64_t depth;
}CG3DMatrix;

#endif
