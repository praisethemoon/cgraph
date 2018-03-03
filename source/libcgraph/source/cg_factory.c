

#include <inttypes.h>
#include <stdlib.h>
#include <assert.h>

#include "cgraph.h"
#include "cg_operation.h"
#include "cg_types.h"
#include "cg_variables.h"
#include "cg_factory.h"
#include "cg_enums.h"

#include <malloc.h>

double* vcopy(uint64_t len, const double* data){
	double* newdata = calloc(len, sizeof(double));
	memcpy(newdata, data, len*sizeof(double));
	return newdata;
}

CGNode* makeVarNode(char* name){
	CGVariable* var = calloc(1, sizeof(CGVariable));
	var->name = name;
	
	CGNode* node = calloc(1, sizeof(CGNode));
	node->type = CGNT_VARIABLE;
	node->var = var;
	
	return node;
}

CGNode* makeDoubleConstantNode(double value){
	CGDouble* d = calloc(1, sizeof(CGDouble));
	d->value = value;
	
	CGPConstant* c = calloc(1, sizeof(CGPConstant));
	c->type = CGVT_DOUBLE;
	c->value = d;
	
	CGNode* node = calloc(1, sizeof(CGNode));
	node->type = CGNT_CONSTANT;
	node->constant = c;
	
	return node;
}

CGNode* makeVectorConstantNode(uint64_t  len, double* value){
	CGVector* v = calloc(1, sizeof(CGVector));
	v->data = value;
	v->len = len;
	
	CGPConstant* c = calloc(1, sizeof(CGPConstant));
	c->type = CGVT_VECTOR;
	c->value = v;
	
	CGNode* node = calloc(1, sizeof(CGNode));
	node->type = CGNT_CONSTANT;
	node->constant = c;
	
	return node;
}


CGNode* makeMatrixConstantNode(uint64_t  rows, uint64_t cols, double* value){
	CGMatrix* m = calloc(1, sizeof(CGMatrix));
	m->data = value;
	m->rows = rows;
	m->cols = cols;
	m->shape = CGMS_ROW_MAJOR;
	
	CGPConstant* c = calloc(1, sizeof(CGPConstant));
	c->type = CGVT_MATRIX;
	c->value = m;
	
	CGNode* node = calloc(1, sizeof(CGNode));
	node->type = CGNT_CONSTANT;
	node->constant = c;
	
	return node;
}


CGNode* makeZeroDoubleConstantNode(){
	CGDouble* d = calloc(1, sizeof(CGDouble));
	
	CGPConstant* c = calloc(1, sizeof(CGPConstant));
	c->type = CGVT_DOUBLE;
	c->value = d;
	
	CGNode* node = calloc(1, sizeof(CGNode));
	node->type = CGNT_CONSTANT;
	node->constant = c;
	
	return node;
}

CGNode* makeZeroVectorConstantNode(uint64_t  len){
	CGVector* v = calloc(1, sizeof(CGVector));
	v->len = len;
	v->data = calloc(len, sizeof(double));
	
	CGPConstant* c = calloc(1, sizeof(CGPConstant));
	c->type = CGVT_VECTOR;
	c->value = v;
	
	CGNode* node = calloc(1, sizeof(CGNode));
	node->type = CGNT_CONSTANT;
	node->constant = c;
	
	return node;
}


CGNode* makeZeroMatrixConstantNode(uint64_t  rows, uint64_t cols){
	CGMatrix* m = calloc(1, sizeof(CGMatrix));
	m->data = calloc(rows*cols, sizeof(double));
	m->rows = rows;
	m->cols = cols;
	m->shape = CGMS_ROW_MAJOR;
	
	CGPConstant* c = calloc(1, sizeof(CGPConstant));
	c->type = CGVT_MATRIX;
	c->value = m;
	
	CGNode* node = calloc(1, sizeof(CGNode));
	node->type = CGNT_CONSTANT;
	node->constant = c;
	
	return node;
}

CGNode* makeGraphNode(CGraph* graph){
	CGNode* node = calloc(1, sizeof(CGNode));
	node->type = CGNT_GRAPH;
	node->graph = graph;
	
	return node;
}

CGNode* makeBinaryOpNode(CGBinaryOperationType type, CGNode* lhs, CGNode* rhs){
	CGNode* node = calloc(1, sizeof(CGNode));
	node->type = CGNT_BINARY_OPERATION;
	
	node->bop = calloc(1, sizeof(CGBinaryOperation));
	node->bop->type = type;
	node->bop->lhs = lhs;
	node->bop->rhs = rhs;
	
	return node;
}

CGNode* makeUnaryOpNode(CGUnaryOperationType type, CGNode* uhs){
	CGNode* node = calloc(1, sizeof(CGNode));
	node->type = CGNT_UNARY_OPERATION;
	
	node->uop = calloc(1, sizeof(CGUnaryOperation));
	node->uop->type = type;
	node->uop->uhs = uhs;
	
	return node;
}

CGraph* makeGraph(char* name){
	CGraph* graph = calloc(1, sizeof(CGraph));
	graph->name = name;
	map_init(&graph->vars);
	
	return graph;
}

void graphSetVar(CGraph* graph, char* name, CGNode* value){
	map_set(&graph->vars, name, value);
}

CGNode* graphGetVar(CGraph* graph, char* name){
	return *map_get(&graph->vars, name);
}
