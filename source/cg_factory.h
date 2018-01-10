#ifndef CCGRAPH_FACTORY_H
#define CCGRAPH_FACTORY_H

#include "ccgraph.h"
#include "cg_operation.h"
#include "cg_types.h"
#include "cg_variables.h"

#include "memory.h"

CGraph* makeGraph(CGNode* node);
CGNode* makeDoubleConstantNode(double value);
CGNode* makeVectorConstantNode(uint64_t  len, double* value);
CGNode* makeMatrixConstantNode(uint64_t  rows, uint64_t cols, double* value);
CGNode* makeBinaryOpNode(CGBinaryOperationType type, CGNode* lhs, CGNode* rhs);

#endif
