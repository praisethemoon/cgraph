#ifndef CCGRAPH_H
#define CCGRAPH_H

#include "map.h"
#include "cg_operation.h"
#include "cg_types.h"
#include "cg_variables.h"

typedef enum CGNodeType {
	CGNT_VARIABLE = 0,
	CGNT_CONSTANT,
	CGNT_GRAPH,
	CGNT_BINARY_OPERATION,
	CGNT_UNARY_OPERATION
}CGNodeType;

#define MAX_NODE_TYPE CGNT_UNARY_OPERATION

typedef struct CGNode {
	CGNodeType type;
	union {
		CGPVariable* var;
		CGPConstant* constant;
		CGBinaryOperation* bop;
		CGUnaryOperation* uop;
	};
}CGNode;

typedef struct CGraph {
	char* name;
	CGNode* root;
	map_t(CGNode*) vars;
}CGraph;

/*
 * List of errors that can occure during computations
 */
typedef enum CGErrorType {
	/*
	 * incompatible arguments given i.e device a vector by a matrix, 
	 * like seriously .. what do you think you are doing?
	 */
	CGET_INCOMPATIBLE_ARGS_EXCEPTION = 0,
	
	/*
	 * arguments' types are fine, yet their dimentions are not compatible.
	 */
	CGET_INCOMPATIBLE_DIMENTIONS_EXCEPTION,
	
	/*
	 * double overflow exception
	 */
	CGET_OVERFLOW_EXCEPTION,
	
	/*
	 * Variable not present in the current graph
	 */
	CGET_VARIABLE_DOES_NOT_EXIST,
	
	/*
	 * Operation is not yet supported.
	 */
	CGET_OPERATION_NOT_IMPLEMENTED,
	
	/*
	 * Matrix is not invertible
	 */
	CGET_MATRIX_NOT_INVERTIBLE
	
}CGErrorType;

#define MAX_ERROR_TYPE

typedef struct CGError {
	CGErrorType errorType;
	CGNode* faultyNode;
}CGError;

typedef struct  {
	CGVarType type;
	void* value;
	CGError* error;
}CGResultNode;

CGResultNode* computeRawNode(CGNode* node);
CGResultNode* computeCGNode(CGraph* graph, CGNode* node);
CGResultNode* computeGraph(CGraph* graph);

void freeNode(CGNode* node);
void freeGraph(CGraph* graph);

#endif
