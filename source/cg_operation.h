#ifndef CG_OPERATION_H
#define CG_OPERATION_H

#include "ccgraph.h"

typedef enum CGBinaryOperationType {
	CGBOT_ADD=0,
	CGBOT_SUB,
	CGBOT_MULT,
	CGBOT_DIV,
} CGBinaryOperationType;


typedef enum CGUnaryOperationType {
	CGUOT_MINUS=0,
	CGUOT_PLUS,
	CGBOT_INV,
	CGBOT_TRANSPOSE,
	CGBOT_EXP,
	CGBOT_LOG,
} CGUnaryOperationType;

typedef struct CGBinaryOperation {
	struct CGNode* lhs;
	struct CGNode* rhs;
	
	CGBinaryOperationType type;
}CGBinaryOperation;

typedef struct CGUnaryOperation {
	struct CGUnaryOperation* uhs;
	CGUnaryOperationType type;
}CGUnaryOperation;


#endif
