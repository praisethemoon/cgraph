#ifndef CCGRAPH_FACTORY_H
#define CCGRAPH_FACTORY_H

#include "cgraph.h"
#include "cg_operation.h"
#include "cg_types.h"
#include "cg_variables.h"
#include "cg_enums.h"



// utilities
cg_float* vcopy(uint64_t len, const cg_float* data);

CGNode* makeVarNode(char* name);
CGNode* makeDoubleConstantNode(cg_float value);
CGNode* makeVectorConstantNode(uint64_t  len, cg_float* value);
CGNode* makeMatrixConstantNode(uint64_t  rows, uint64_t cols, cg_float* value);
CGNode* makeVectorConstantNodeCopy(uint64_t  len, cg_float* value);
CGNode* makeMatrixConstantNodeCopy(uint64_t  rows, uint64_t cols, cg_float* value);
CGNode* makeZeroDoubleConstantNode();
CGNode* makeZeroVectorConstantNode(uint64_t  len);
CGNode* makeZeroMatrixConstantNode(uint64_t  rows, uint64_t cols);

CGNode* makeRandomDoubleConstantNode();
CGNode* makeRandomVectorConstantNode(uint64_t  len);
CGNode* makeRandomMatrixConstantNode(uint64_t  rows, uint64_t cols);

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

CGResultNode* makeDoubleResultNode(cg_float val);
CGResultNode* makeVectorResultNode(uint64_t len, cg_float* val);
CGResultNode* makeMatrixResultNode(uint64_t rows, uint64_t cols, cg_float* val);

cg_float* cg_raw_copy(cg_float* src, uint64_t len);

CGraph* makeGraph(char* name);
void graphSetVar(CGraph* graph, const char* name, CGNode* value);
void graphUnsetVar(CGraph* graph, const char* name);
CGNode* graphGetVar(CGraph* graph, const char* name);


#endif
