#ifndef CCGRAPH_FACTORY_H
#define CCGRAPH_FACTORY_H

#include "cgraph.h"
#include "cg_operation.h"
#include "cg_types.h"
#include "cg_variables.h"

#include "memory.h"

// utilities
double* vcopy(uint64_t len, const double* data);

CGNode* makeVarNode(char* name);
CGNode* makeDoubleConstantNode(double value);
CGNode* makeVectorConstantNode(uint64_t  len, double* value);
CGNode* makeMatrixConstantNode(uint64_t  rows, uint64_t cols, double* value);
CGNode* makeBinaryOpNode(CGBinaryOperationType type, CGNode* lhs, CGNode* rhs);
CGNode* makeUnaryOpNode(CGUnaryOperationType type, CGNode* uhs);

CGraph* makeGraph(char* name);
void graphSetVar(CGraph* graph, char* name, CGNode* value);
CGNode* graphGetVar(CGraph* graph, char* name);


#endif
