#ifndef CG_TYPES_H
#define CG_TYPES_H

#include <stdint.h>

typedef enum CGVarType {
	CGVT_DOUBLE=0,
	CGVT_VECTOR,
	CGVT_MATRIX,
}CGVarType;

typedef struct CGDouble {
	double value;
}CGDouble;

typedef struct CGVector {
	double* data;
	uint64_t len;
}CGVector;

typedef enum CGMShape {
	CGMS_ROW_MAJOR=0,
	CGMS_COL_MAJOR
}CGMShape;

typedef struct CGMatrix {
	double* data;
	uint64_t cols;
	uint64_t rows;
	CGMShape shape;
}CGMatrix;

#endif
