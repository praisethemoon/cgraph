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

#define MAX_BINARY_OPERATION CGBOT_TMULT


typedef enum CGUnaryOperationType {
	CGUOT_MINUS=0,
	CGUOT_INV,
	CGUOT_TRANSPOSE,
	CGUOT_EXP,
	CGUOT_LOG,
	CGUOT_SIN,
	CGUOT_COS,
	CGUOT_TAN,
	CGUOT_TANH,
	CGUOT_SUM,
} CGUnaryOperationType;

#define MAX_UNARY_OPERATION CGUOT_LOG

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
