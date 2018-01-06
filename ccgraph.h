#ifndef CCGRAPH_H
#define CCGRAPH_H

#include "cg_operation.h"
#include "cg_types.h"
#include "cg_variables.h"

typedef enum CGNodeType {
	CGNT_VARIABLE = 0,
	CGNT_CONSTANT,
	CGNT_BINARY_OPERATION,
	CGNT_UNARY_OPERATION
}CGNodeType;

typedef struct CGNode {
	CGNodeType type;
	union {
		CGPVariable* var;
		CGPConstant* constant;
		CGBinaryOperation* bop;
		CGUnaryOperation* uop;
	};
}CGNode;


typedef struct  {
	CGVarType type;
	void* value;
}CGResultNode;

CGResultNode* computeCGNode(CGNode* node);

#endif
