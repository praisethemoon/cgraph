#ifndef CG_OPERATION_H
#define CG_OPERATION_H

#include "cgraph.h"
#include "cg_enums.h"

typedef struct CGBinaryOperation {
	struct CGNode* lhs;
	struct CGNode* rhs;
	
	CGBinaryOperationType type;
}CGBinaryOperation;

typedef struct CGUnaryOperation {
	struct CGNode* uhs;
	CGUnaryOperationType type;
}CGUnaryOperation;


#endif
