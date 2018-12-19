

#include <inttypes.h>
#include <stdlib.h>
#include <assert.h>

#include "cgraph.h"
#include "cg_operation.h"
#include "cg_types.h"
#include "cg_variables.h"
#include "cg_factory.h"
#include "cg_enums.h"

#include "vec.h"

#include <malloc.h>

/*
 * These nodes are only used to store derivative values
 */
CGNode* makeZeroDoubleConstantNodeNoDiff(){
	CGDouble* d = calloc(1, sizeof(CGDouble));
	
	CGPConstant* c = calloc(1, sizeof(CGPConstant));
	c->type = CGVT_DOUBLE;
	c->value = d;
	
	CGNode* node = calloc(1, sizeof(CGNode));
	node->type = CGNT_CONSTANT;
	node->constant = c;
	
	node->result = NULL;
	node->diff = NULL;
	vec_init(&node->consumers);
	
	return node;
}



CGNode* makeZeroVectorConstantNodeNoDiff(uint64_t  len){
	CGVector* v = calloc(1, sizeof(CGVector));
	v->len = len;
	v->data = calloc(len, sizeof(double));
	
	CGPConstant* c = calloc(1, sizeof(CGPConstant));
	c->type = CGVT_VECTOR;
	c->value = v;
	
	CGNode* node = calloc(1, sizeof(CGNode));
	node->type = CGNT_CONSTANT;
	node->constant = c;
	
	node->result = NULL;
	node->diff = NULL;
	vec_init(&node->consumers);
	
	return node;
}


CGNode* makeZeroMatrixConstantNodeNoDiff(uint64_t  rows, uint64_t cols){
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
	
	node->result = NULL;
	node->diff = NULL;
	vec_init(&node->consumers);
	
	return node;
}


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
	
	node->result = NULL;
	node->diff = NULL;
	vec_init(&node->consumers);
	
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
	
	node->result = NULL;
	node->diff = makeZeroDoubleConstantNodeNoDiff();
	vec_init(&node->consumers);
	
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
	
	node->result = NULL;
	node->diff = makeZeroVectorConstantNodeNoDiff(len);
	vec_init(&node->consumers);
	
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
	
	node->result = NULL;
	node->diff = makeZeroMatrixConstantNodeNoDiff(rows, cols);
	vec_init(&node->consumers);
	
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
	
	node->result = NULL;
	node->diff = makeZeroDoubleConstantNodeNoDiff();
	vec_init(&node->consumers);
	
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
	
	node->result = NULL;
	node->diff = makeZeroVectorConstantNodeNoDiff(len);
	vec_init(&node->consumers);
	
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
	
	node->result = NULL;
	node->diff = makeZeroMatrixConstantNodeNoDiff(rows, cols);
	vec_init(&node->consumers);
	
	return node;
}

CGNode* makeGraphNode(CGraph* graph){
	CGNode* node = calloc(1, sizeof(CGNode));
	node->type = CGNT_GRAPH;
	node->graph = graph;
	
	node->result = NULL;
	node->diff = makeZeroDoubleConstantNodeNoDiff();
	vec_init(&node->consumers);
	
	return node;
}

CGNode* makeBinaryOpNode(CGBinaryOperationType type, CGNode* lhs, CGNode* rhs){
	CGNode* node = calloc(1, sizeof(CGNode));
	node->type = CGNT_BINARY_OPERATION;
	
	node->bop = calloc(1, sizeof(CGBinaryOperation));
	node->bop->type = type;
	node->bop->lhs = lhs;
	node->bop->rhs = rhs;
	
	node->result = NULL;
	node->diff = makeZeroDoubleConstantNodeNoDiff();
	vec_init(&node->consumers);
	
	vec_push(&node->consumers, lhs);
	vec_push(&node->consumers, rhs);
	
	return node;
}

CGNode* makeUnaryOpNode(CGUnaryOperationType type, CGNode* uhs){
	CGNode* node = calloc(1, sizeof(CGNode));
	node->type = CGNT_UNARY_OPERATION;
	
	node->uop = calloc(1, sizeof(CGUnaryOperation));
	node->uop->type = type;
	node->uop->uhs = uhs;
	
	node->result = NULL;
	node->diff = makeZeroDoubleConstantNodeNoDiff();
	vec_push(&node->consumers, uhs);
	
	return node;
}

CGNode* makeSumNode(CGNode* uhs, uint8_t axis){
	CGNode* node = calloc(1, sizeof(CGNode));
	node->type = CGNT_SUM_OPERATION;
	
	node->sum = calloc(1, sizeof(CGSumOperation));
	node->sum->axis = axis;
	node->sum->uhs = uhs;
	
	node->result = NULL;
	node->diff = makeZeroDoubleConstantNodeNoDiff();
	vec_push(&node->consumers, uhs);
	
	return node;
}

CGResultNode* makeDoubleResultNode(double val){
	CGDouble* Y = calloc(1, sizeof(CGDouble));
	Y->value = val;
	
	CGResultNode* result = calloc(1, sizeof(CGResultNode));
	result->type = CGVT_DOUBLE;
	result->value = Y;
	
	return result;
}

CGResultNode* makeVectorResultNode(uint64_t len, double* val){
	CGVector* v = calloc(1, sizeof(CGVector));
	v->data = val;
	v->len = len;
	
	CGResultNode* result = calloc(1, sizeof(CGResultNode));
	result->type = CGVT_VECTOR;
	result->value = v;
	
	return result;
}

CGResultNode* makeMatrixResultNode(uint64_t rows, uint64_t cols, double* val){
	CGMatrix* m = calloc(1, sizeof(CGMatrix));
	m->data = val;
	m->rows = rows;
	m->cols = cols;
	m->shape = CGMS_ROW_MAJOR;
	
	CGResultNode* result = calloc(1, sizeof(CGResultNode));
	result->type = CGVT_MATRIX;
	result->value = m;
	
	return result;
}

CGraph* makeGraph(char* name){
	CGraph* graph = calloc(1, sizeof(CGraph));
	graph->name = name;
	map_init(&graph->vars);
	
	return graph;
}

void graphSetVar(CGraph* graph, char* name, CGNode* value){
	int res = map_set(&graph->vars, name, value);
}

CGNode* graphGetVar(CGraph* graph, char* name){
	return *map_get(&graph->vars, name);
}


CGNode* resultNodeToConstantNode(CGResultNode* result){
	switch(result->type){
		case CGVT_DOUBLE:
		{
			CGDouble* d = (CGDouble*)result->value;
			return makeDoubleConstantNode(d->value);
		}
		case CGVT_VECTOR:
		{
			CGVector* v = (CGVector*)result->value;
			return makeVectorConstantNode(v->len, v->data);
		}
		case CGVT_MATRIX:
		{
			CGMatrix* m = (CGMatrix*)result->value;
			return makeMatrixConstantNode(m->rows, m->cols, m->data);
		}
	}
}
