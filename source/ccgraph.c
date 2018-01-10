/*
 * 
 * 
 * 
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <cblas.h>
#include <string.h> // memcpy

#include "ccgraph.h"
#include "cg_operation.h"
#include "cg_types.h"
#include "cg_variables.h"

#include "memory.h"

extern const char* BinaryOperationTypeString[];
extern const char* NodeTypeString[];
extern const char* VariableTypeString[];

void dumpNode(CGNode* node){
	fprintf(stderr, "\t\t+Node: '%s'\n", NodeTypeString[node->type]);
	if(node->type == CGNT_CONSTANT){
		fprintf(stderr, "\t\t+Type: '%s'\n", VariableTypeString[node->var->type]);
	}
}

void throwUnsupportedBinaryOperationException(CGBinaryOperationType type, CGNode* lhs, CGNode* rhs){
	fprintf(stderr, "Unsupported operation %s\n", BinaryOperationTypeString[type]);
	fprintf(stderr, "\tLeft node:\n");
	dumpNode(lhs);
	fprintf(stderr, "\tRight node:\n");
	dumpNode(rhs);
}

/*
 * *********************
 * Multiplication
 * *********************
 */

/*
 * d.d 
 */
CGResultNode* mulDD(CGDouble* M, CGDouble* V){
	CGDouble* Y = dmt_calloc(1, sizeof(CGDouble));
	Y->value = M->value * V->value;
	
	CGResultNode* result = dmt_calloc(1, sizeof(CGResultNode));
	result->type = CGVT_DOUBLE;
	result->value = Y;
}

/*
 * d.V == V.d
 */
CGResultNode* multDV(CGDouble* a, CGVector* V){
	double* y = dmt_calloc(V->len, sizeof(double));
	CGVector* Y = dmt_calloc(1, sizeof(CGVector));
	Y->len = V->len;
	Y->data = y;
	
	cblas_dcopy(V->len, V->data, 1, y, 1);
	cblas_dscal(V->len, a->value, y, 1);
	
	CGResultNode* result = dmt_calloc(1, sizeof(CGResultNode));
	result->type = CGVT_VECTOR;
	result->value = Y;
	
	return result;
}


/*
 * d.M == M.d
 */
CGResultNode* mulMD(CGDouble* a, CGMatrix* M){
	uint64_t size = M->cols*M->rows;
	double value = a->value;
	
	double* y = dmt_calloc(size, sizeof(double));
	CGMatrix* Y = dmt_calloc(1, sizeof(CGMatrix));
	Y->rows = M->rows;
	Y->cols = M->cols;
	Y->data = y;
	
	memcpy(y, M->data, size*sizeof(double));
	
	uint64_t i = 0;
	
	for(;i<size;i++){
		y[i] *= value;
	}
	
	CGResultNode* result = dmt_calloc(1, sizeof(CGResultNode));
	result->type = CGVT_MATRIX;
	result->value = Y;
	
	return result;
}

/*
 * M.v
 */
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

/*
 * M.N
 */
CGResultNode* mulMM(CGMatrix* M, CGMatrix* N){
	double* z = dmt_calloc(M->rows*N->cols, sizeof(double));
	
	CGMatrix* Z = dmt_calloc(1, sizeof(CGMatrix));
	Z->rows = M->rows;
	Z->cols = N->cols;
	
	cblas_dgemm(M->shape == CGMS_ROW_MAJOR?CblasRowMajor:CblasColMajor,
		    CblasNoTrans, CblasNoTrans, M->rows, N->cols, M->cols,
		    1.0, M->data, M->cols, N->data, N->cols, 0, z, Z->cols);
	
	Z->data = z;
	
	CGResultNode* result = dmt_calloc(1, sizeof(CGResultNode));
	result->type = CGVT_MATRIX;
	result->value = Z;
	
	return result;
}


/*
 * *********************
 * Division 
 * *********************
 */

/*
 *  d/d
 */
CGResultNode* divDD(CGDouble* D1, CGDouble* D2){
	double res = D1->value / D2->value;
	
	CGDouble* Y = dmt_calloc(1, sizeof(CGDouble));
	Y->value = res;
	
	CGResultNode* result = dmt_calloc(1, sizeof(CGResultNode));
	result->type = CGVT_DOUBLE;
	result->value = Y;
	
	return result;
}

/*
 * *********************
 * Transpose Multiplication
 * *********************
 */

/*
 * M^T.v
 */
CGResultNode* mulTMV(CGMatrix* M, CGVector* V){
	double* y = dmt_calloc(V->len, sizeof(double));
	CGVector* Y = dmt_calloc(1, sizeof(CGVector));
	Y->len = V->len;
	Y->data = y;
	
	cblas_dgemv(M->shape == CGMS_ROW_MAJOR?CblasRowMajor:CblasColMajor,
		    CblasTrans, M->rows, M->cols, 1.0, M->data,
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
		
		case CGBOT_DIV:{
			if((lhsType == CGVT_DOUBLE) && (rhsType == CGVT_DOUBLE)){
				return divDD((CGDouble*)lhsValue, (CGDouble*)rhsValue);
			}
			
			/*
			if((lhsType == CGVT_VECTOR) && (rhsType == CGVT_DOUBLE)){
				return divVD((CGVector*)lhsValue, (CGDouble*)rhsValue);
			}
			
			if((lhsType == CGVT_MATRIX) && (rhsType == CGVT_DOUBLE)){
				return divMD((CGVector*)lhsValue, (CGDouble*)rhsValue);
			}
			
			if((lhsType == CGVT_VECTOR) && (rhsType == CGVT_DOUBLE)){
				return divVD((CGVector*)lhsValue, (CGDouble*)rhsValue);
			}
			*/
		}
		
		case CGBOT_MULT:{
			if(1)
				throwUnsupportedBinaryOperationException(type, lhs, rhs);
			
			if((lhsType == CGVT_MATRIX) && (rhsType == CGVT_MATRIX)){
				return mulMM((CGMatrix*)lhsValue, (CGMatrix*)rhsValue);
			}
			
			if((lhsType == CGVT_MATRIX) && (rhsType == CGVT_VECTOR)){
				return mulMV((CGMatrix*)lhsValue, (CGVector*)rhsValue);
			}
			
			if((lhsType == CGVT_DOUBLE) && (rhsType == CGVT_DOUBLE)){
				return mulDD((CGDouble*)lhsValue, (CGDouble*)rhsValue);
			}
			
			if((lhsType == CGVT_DOUBLE) && (rhsType == CGVT_VECTOR)){
				return multDV((CGDouble*)lhsValue, (CGVector*)rhsValue);
			}
			
			if((lhsType == CGVT_VECTOR) && (rhsType == CGVT_DOUBLE)){
				return multDV((CGDouble*)rhsValue, (CGVector*)lhsValue);
			}
			
			if((lhsType == CGVT_DOUBLE) && (rhsType == CGVT_MATRIX)){
				return mulMD((CGDouble*)lhsValue, (CGMatrix*)rhsValue);
			}
			
			if((lhsType == CGVT_MATRIX) && (rhsType == CGVT_DOUBLE)){
				return mulMD((CGDouble*)rhsValue, (CGMatrix*)lhsValue);
			}
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
