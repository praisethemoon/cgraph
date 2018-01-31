#ifndef CG_TYPES_H
#define CG_TYPES_H

#include <stdint.h>

#include "cg_enums.h"

typedef struct CGDouble {
	double value;
}CGDouble;

typedef struct CGVector {
	double* data;
	uint64_t len;
}CGVector;

typedef struct CGMatrix {
	double* data;
	uint64_t cols;
	uint64_t rows;
	CGMShape shape;
}CGMatrix;

#endif
