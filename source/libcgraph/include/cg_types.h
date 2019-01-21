#ifndef CG_TYPES_H
#define CG_TYPES_H

#include <inttypes.h>


#ifdef CG_USE_OPENCL
#include <cf4ocl2.h>
#endif

#include "cg_enums.h"

#define CG_SCALAR_TYPE float
#define CG_CL_SCALAR_TYPE cl_float

/**
 * \brief Double  object.
 */
typedef struct CGDouble {
	CG_SCALAR_TYPE value;
}CGDouble;

/**
 * \brief Vector object metadata and values
 */
typedef struct CGVector {
	CG_SCALAR_TYPE* data;
	uint64_t len;

#ifdef CG_USE_OPENCL
	CCLBuffer* buf;
#endif

	//CG_SCALAR_TYPE* (*getSub)(uint64_t start, uint64_t end);
}CGVector;

/**
 * \brief Matrix object metadata and values
 */
typedef struct CGMatrix {
	CG_SCALAR_TYPE* data;
	uint64_t cols;
	uint64_t rows;

#ifdef CG_USE_OPENCL
	CCLBuffer* buf;
#endif

	//CG_SCALAR_TYPE* (*getRow)(uint64_t row);
	//CG_SCALAR_TYPE* (*getCol)(uint64_t col);
	//CG_SCALAR_TYPE* (*getPatch)(uint64_t row, uint64_t x_size, uint64_t col, uint64_t y_size);
}CGMatrix;


/**
 * \brief 3D Matrix object metadata and values
 */
typedef struct CG3DMatrix {
	CG_SCALAR_TYPE* data;

#ifdef CG_USE_OPENCL
	CCLBuffer* buf;
#endif

	uint64_t cols;
	uint64_t rows;
	uint64_t depth;
}CG3DMatrix;

#endif
