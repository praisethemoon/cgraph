#ifndef CG_OPERATION_H
#define CG_OPERATION_H

#include <stdint.h>

#include "cgraph.h"
#include "cg_enums.h"

typedef struct CGBinaryOperation {
	struct CGNode* lhs;
	struct CGNode* rhs;
	
	CGBinaryOperationType type;
}CGBinaryOperation;

typedef struct CGAxisBoundOperation{
	struct CGNode* uhs;
	uint8_t axis;
	
	CGAxisBoundOperationType type;
}CGAxisBoundOperation;

typedef struct CGUnaryOperation {
	struct CGNode* uhs;
	CGUnaryOperationType type;
}CGUnaryOperation;

typedef struct CGCrossEntropyLoss {
	struct CGNode* x;
	struct CGNode* y;
	uint64_t num_classes;
}CGCrossEntropyLoss;

#endif
