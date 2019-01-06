#ifndef CCGRAPH_FACTORY_H
#define CCGRAPH_FACTORY_H

#include "cgraph.h"
#include "cg_operation.h"
#include "cg_types.h"
#include "cg_variables.h"
#include "cg_enums.h"

#include <malloc.h>

// utilities
double* vcopy(uint64_t len, const double* data);



CGNode* makeVarNode(char* name);
CGNode* makeDoubleConstantNode(double value);
CGNode* makeVectorConstantNode(uint64_t  len, double* value);
CGNode* makeMatrixConstantNode(uint64_t  rows, uint64_t cols, double* value);
CGNode* makeVectorConstantNodeCopy(uint64_t  len, double* value);
CGNode* makeMatrixConstantNodeCopy(uint64_t  rows, uint64_t cols, double* value);
CGNode* makeZeroDoubleConstantNode();
CGNode* makeZeroVectorConstantNode(uint64_t  len);
CGNode* makeZeroMatrixConstantNode(uint64_t  rows, uint64_t cols);

CGNode* makeOnesDoubleConstantNode();
CGNode* makeOnesVectorConstantNode(uint64_t  len);
CGNode* makeOnesMatrixConstantNode(uint64_t  rows, uint64_t cols);

CGNode* makeGraphNode(CGraph* graph);
CGNode* makeBinaryOpNode(CGBinaryOperationType type, CGNode* lhs, CGNode* rhs);
CGNode* makeUnaryOpNode(CGUnaryOperationType type, CGNode* uhs);
CGNode* makeAxisBoundNode(CGAxisBoundOperationType type, CGNode* uhs, uint8_t axis);
CGNode* makeCrossEntropyLossFunc(CGNode* x, CGNode* y, uint64_t num_classes);

CGNode* resultNodeToConstantNode(CGResultNode* result);
CGResultNode* constantNodeToResultNode(CGNode* node);
CGResultNode* constantNodeToResultNodeCopy(CGNode* node);
CGNode* resultNodeToConstantNodeCopy(CGResultNode* result);

CGResultNode* makeDoubleResultNode(double val);
CGResultNode* makeVectorResultNode(uint64_t len, double* val);
CGResultNode* makeMatrixResultNode(uint64_t rows, uint64_t cols, double* val);

double* cg_raw_copy(double* src, uint64_t len);

CGraph* makeGraph(char* name);
void graphSetVar(CGraph* graph, const char* name, CGNode* value);
void graphUnsetVar(CGraph* graph, const char* name);
CGNode* graphGetVar(CGraph* graph, const char* name);


#endif
