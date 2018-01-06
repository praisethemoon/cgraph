/*
 * 
 * 
 * 
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <cblas.h>

#include "ccgraph.h"
#include "cg_operation.h"
#include "cg_types.h"
#include "cg_variables.h"

#include "memory.h"

CGResultNode* mulMV(CGMatrix* M, CGVector* V){
	double* y = dmt_calloc(V->len, sizeof(double));
	CGVector* Y = dmt_calloc(1, sizeof(CGVector));
	Y->len = V->len;
	Y->data = y;
	
	cblas_dgemv(M->shape == CGMS_ROW_MAJOR?CblasRowMajor:CblasColMajor,
		    CblasNoTrans, M->rows, M->cols, 1.0, M->data,
		    V->len, V->data, 1, 0.0, Y->data, 1);
	
	CGResultNode* result = dmt_calloc(1, sizeof(CGResultNode));
	result->type = CGVT_VECTOR;
	result->value = Y;
	
	return result;
}

CGResultNode* processBinaryOperation(CGBinaryOperationType type, CGNode* lhs, CGNode* rhs){
	CGVarType lhsType = CGVT_DOUBLE;
	CGVarType rhsType = CGVT_DOUBLE;
	
	void* lhsValue = NULL;
	void* rhsValue = NULL;
	
	// LHS
	if(lhs->type == CGNT_CONSTANT){
		lhsType = lhs->constant->type;
		lhsValue = lhs->constant->value;
	}
	else if (lhs->type ==  CGNT_VARIABLE) {
		lhsType = lhs->var->type;
		lhsValue= lhs->var->value;
	}
	else
	{
		CGResultNode* lhsResult = computeCGNode(lhs);
		lhsType = lhsResult->type;
		lhsValue = lhsResult->value;
	}
	// RHS
	if(rhs->type == CGNT_CONSTANT){
		rhsType = rhs->constant->type;
		rhsValue = rhs->constant->value;
	}
	else if (rhs->type ==  CGNT_VARIABLE) {
		rhsType = rhs->var->type;
		rhsValue= rhs->var->value;
	}
	else
	{
		CGResultNode* rhsResult = computeCGNode(rhs);
		rhsType = rhsResult->type;
		rhsValue = rhsResult->value;
	}
	
	switch(type){
		case CGBOT_MULT:{
			assert(
				((lhsType == CGVT_DOUBLE) && (rhsType == CGVT_DOUBLE)) ||
				((lhsType == CGVT_DOUBLE) && (rhsType == CGVT_VECTOR)) ||
				((lhsType == CGVT_DOUBLE) && (rhsType == CGVT_MATRIX)) ||
				((lhsType == CGVT_MATRIX) && (rhsType == CGVT_VECTOR)) ||
				((lhsType == CGVT_MATRIX) && (rhsType == CGVT_MATRIX))
			);
			
			if((lhsType == CGVT_MATRIX) && (rhsType == CGVT_VECTOR)){
				return mulMV((CGMatrix*)lhsValue, (CGVector*)rhsValue);
			}
			printf("%d, %d\n", lhsType, rhsType);
		}
	}
}

/* N1 => A + N2
 * N2 => B * C 
 * A+B*C
 * 
 */
CGResultNode* computeCGNode(CGNode* node){
	CGResultNode* result = NULL;
	
	switch(node->type){
		case CGNT_CONSTANT:
			result = dmt_calloc(1, sizeof(CGResultNode));
			result->type = node->constant->type;
			result->value = node->constant->value;
			break;
		case CGNT_VARIABLE:
			result = dmt_calloc(1, sizeof(CGResultNode));
			result->type = node->var->type;
			result->value = node->var->value;
			break;
		case CGNT_BINARY_OPERATION:
			result = processBinaryOperation(node->bop->type, node->bop->lhs, node->bop->rhs);
			break;
	}
	
	return result;
}
