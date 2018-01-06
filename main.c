/*
 * 
 * 
 */

#include <stdlib.h>
#include <stdio.h>

#include "ccgraph.h"
#include "cg_operation.h"
#include "cg_types.h"
#include "cg_variables.h"

#include "memory.h"

int main(int argc, char *argv[]) {
	// Vector X
	CGPConstant* X = dmt_calloc(1, sizeof(CGPConstant));
	X->type = CGVT_VECTOR;
	double value []= {-1.0, -1.0, 1.0};

	CGVector* x = dmt_calloc(1, sizeof(CGVector));
	x->data = value;
	x->len = 3;

	X->value = x;
	
	// Matrix M
	
	CGPConstant* M = dmt_calloc(1, sizeof(CGPConstant));
	M->type = CGVT_MATRIX;
	double value2[] = {
		3, 1, 3,
		1, 5, 9,
		2, 6, 5
	};
	

	CGMatrix* m = dmt_calloc(1, sizeof(CGMatrix));
	m->data = value2;
	m->cols = 3;
	m->rows = 3;
	m->shape = CGMS_ROW_MAJOR;
	
	M->value = m;

	CGNode* node1 = dmt_calloc(1, sizeof(CGNode));
	node1->type = CGNT_CONSTANT;
	node1->constant = M;
	
	CGNode* node2 = dmt_calloc(1, sizeof(CGNode));
	node2->type = CGNT_CONSTANT;
	node2->constant = X;

	CGNode* node = dmt_calloc(1, sizeof(CGNode));
	node->type = CGNT_BINARY_OPERATION;
	
	node->bop = dmt_calloc(1, sizeof(CGBinaryOperation));
	node->bop->type = CGBOT_MULT;
	node->bop->lhs = node1;
	node->bop->rhs = node2;
	
	CGResultNode* result = computeCGNode(node);
	CGVector* Y = (CGVector*)result->value;

	printf("result: %d shape: %d\n", result->type, Y->len);
	uint64_t i = 0;
	
	for(;i<Y->len;i++){
		printf("\t%f\n", Y->data[i]);
	}
}
