#ifndef CG_TYPES_H
#define CG_TYPES_H

#include <inttypes.h>

#include "cg_enums.h"

/**
 * \brief Double  object.
 */
typedef struct CGDouble {
	double value;
}CGDouble;

/**
 * \brief Vector object metadata and values
 */
typedef struct CGVector {
	double* data;
	uint64_t len;
}CGVector;

/**
 * \brief Matrix object metadata and values
 */
typedef struct CGMatrix {
	double* data;
	uint64_t cols;
	uint64_t rows;
	CGMShape shape;
}CGMatrix;

#endif
