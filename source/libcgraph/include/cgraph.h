#ifndef CCGRAPH_H
#define CCGRAPH_H

#include "map.h"
#include "vec.h"
#include "cg_operation.h"
#include "cg_types.h"
#include "cg_variables.h"
#include "cg_enums.h"

/* Creates the type cgnode_vec_t for storing all nodes in a graph*/
typedef vec_t(void*) CGNode_Vec;

typedef struct CGNode {
	CGNodeType type;
	union {
		CGVariable* var;
		CGPConstant* constant;
		CGBinaryOperation* bop;
		CGUnaryOperation* uop;
		CGAxisBoundOperation* axop;
		CGCrossEntropyLoss* crossEntropyLoss;
		struct CGraph* graph;
	};
	struct CGResultNode* result;
	struct CGNode* diff;
}CGNode;

typedef struct CGraph {
	char* name;
	CGNode* root;
	map_t(CGNode*) vars;
	CGNode_Vec nodes;
}CGraph;


typedef struct CGError {
	CGErrorType errorType;
	CGNode* faultyNode;
	char* message;
}CGError;

typedef struct CGResultNode {
	CGVarType type;
	void* value;
	CGError* error;
}CGResultNode;

/*
 * Reduces the dimension of a result if necessary, i.e vector of length 1 to double
 */
CGResultNode* reduceDim(CGResultNode* result);

CGResultNode* computeRawNode(CGNode* node);
CGResultNode* computeCGNode(CGraph* graph, CGNode* node);
CGResultNode* computeGraph(CGraph* graph);
void storeNodesInGraph(CGraph* graph, CGNode* node);

void freeDoubleValue(CGDouble* v);
void freeVectorValue(CGVector* data);
void freeMatrixValue(CGMatrix* data);
void freeResultNode(CGResultNode* node);
void freeNode(CGraph* graph, CGNode* node);
void freeGraph(CGraph* graph);

CGNode* copyNode(CGNode* node);
void* copyNodeValue(CGNode* node);
void* copyRNodeValue(CGResultNode* node);
CGResultNode* copyResultNode(CGResultNode* node);
#endif
