#ifndef CG_OPERATION_H
#define CG_OPERATION_H

#include "cgraph.h"

typedef enum CGBinaryOperationType {
	CGBOT_ADD=0,
	CGBOT_SUB,
	CGBOT_MULT,
	CGBOT_DIV,
	CGBOT_POW,
	CGBOT_DOT,
	CGBOT_TMULT, // transpose + mult
} CGBinaryOperationType;


typedef enum CGUnaryOperationType {
	CGUOT_MINUS=0,
	CGUOT_INV,
	CGUOT_TRANSPOSE,
	CGUOT_EXP,
	CGUOT_LOG,
} CGUnaryOperationType;

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
