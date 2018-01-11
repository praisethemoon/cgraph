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
#include <math.h>

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
	
	exit(EXIT_FAILURE);
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
 *  V/d
 */
CGResultNode* divVD(CGVector* V, CGDouble* D){
	double* res = dmt_calloc(V->len, sizeof(double));
	double value = D->value;
	
	CGVector* Y = dmt_calloc(1, sizeof(CGVector));
	Y->data = res;
	
	uint64_t i = 0;
	
	for(;i<V->len;i++){
		res[i] = V->data[i] / value;
	}
	
	CGResultNode* result = dmt_calloc(1, sizeof(CGResultNode));
	result->type = CGVT_VECTOR;
	result->value = Y;
	
	return result;
}

/*
 *  M/d
 */
CGResultNode* divMD(CGMatrix* M, CGDouble* D){
	uint64_t size = M->rows*M->cols;
	double* res = dmt_calloc(size, sizeof(double));
	double value = D->value;
	
	CGMatrix* Y = dmt_calloc(1, sizeof(CGMatrix));
	Y->rows = M->rows;
	Y->cols = M->cols;
	Y->data = res;
	
	uint64_t i = 0;
	
	for(;i<size;i++){
		res[i] = M->data[i] / value;
	}
	
	CGResultNode* result = dmt_calloc(1, sizeof(CGResultNode));
	result->type = CGVT_MATRIX;
	result->value = Y;
	
	return result;
}

/*
 * *********************
 * Addition
 * *********************
 */

/*
 * d+d
 */
CGResultNode* addDD(CGDouble* D1, CGDouble* D2){
	double res = D1->value + D2->value;
	
	CGDouble* Y = dmt_calloc(1, sizeof(CGDouble));
	Y->value = res;
	
	CGResultNode* result = dmt_calloc(1, sizeof(CGResultNode));
	result->type = CGVT_DOUBLE;
	result->value = Y;
	
	return result;
}

/*
 * V+d
 */
CGResultNode* addVD(CGVector* V, CGDouble* D){
	double* res = dmt_calloc(V->len, sizeof(double));
	double value = D->value;
	
	CGVector* Y = dmt_calloc(1, sizeof(CGVector));
	Y->data = res;
	Y->len = V->len;
	
	uint64_t i = 0;
	
	for(;i<V->len;i++){
		res[i] = V->data[i] + value;
	}
	
	CGResultNode* result = dmt_calloc(1, sizeof(CGResultNode));
	result->type = CGVT_VECTOR;
	result->value = Y;
	
	return result;
}

/*
 * M+d
 */
CGResultNode* addMD(CGMatrix* M, CGDouble* D){
	uint64_t size = M->rows*M->cols;
	double* res = dmt_calloc(size, sizeof(double));
	double value = D->value;
	
	CGMatrix* Y = dmt_calloc(1, sizeof(CGMatrix));
	Y->rows = M->rows;
	Y->cols = M->cols;
	Y->data = res;
	
	uint64_t i = 0;
	
	for(;i<size;i++){
		res[i] = M->data[i] + value;
	}
	
	CGResultNode* result = dmt_calloc(1, sizeof(CGResultNode));
	result->type = CGVT_MATRIX;
	result->value = Y;
	
	return result;
}

/*
 * V+V
 */
CGResultNode* addVV(CGVector* V1, CGVector* V2){
	if(V1->len != V2->len){
		// TODO: throw error
	}
	
	uint64_t size = V1->len;
	double* res = dmt_calloc(size, sizeof(double));
	
	CGVector* Y = dmt_calloc(1, sizeof(CGVector));
	Y->len = V1->len;
	Y->data = res;
	
	uint64_t i = 0;
	
	for(;i<size;i++){
		res[i] = V1->data[i] + V2->data[i];
	}
	
	CGResultNode* result = dmt_calloc(1, sizeof(CGResultNode));
	result->type = CGVT_VECTOR;
	result->value = Y;
	
	return result;
}

/*
 * M+M
 */
CGResultNode* addMM(CGMatrix* M1, CGMatrix* M2){
	if((M1->rows != M2->rows) || (M1->cols != M2->cols)){
		// TODO: throw error
	}
	
	uint64_t size = M1->cols*M1->rows;
	double* res = dmt_calloc(size, sizeof(double));
	
	CGMatrix* Y = dmt_calloc(1, sizeof(CGMatrix));
	Y->rows = M1->rows;
	Y->cols = M1->cols;
	Y->data = res;
	
	uint64_t i = 0;
	
	for(;i<size;i++){
		res[i] = M1->data[i] + M2->data[i];
	}
	
	CGResultNode* result = dmt_calloc(1, sizeof(CGResultNode));
	result->type = CGVT_MATRIX;
	result->value = Y;
	
	return result;
}


/*
 * *********************
 * Substraction
 * *********************
 */

/*
 * d-d
 */
CGResultNode* subDD(CGDouble* D1, CGDouble* D2){
	double res = D1->value - D2->value;
	
	CGDouble* Y = dmt_calloc(1, sizeof(CGDouble));
	Y->value = res;
	
	CGResultNode* result = dmt_calloc(1, sizeof(CGResultNode));
	result->type = CGVT_DOUBLE;
	result->value = Y;
	
	return result;
}

/*
 * V-d
 */
CGResultNode* subVD(CGVector* V, CGDouble* D){
	double* res = dmt_calloc(V->len, sizeof(double));
	double value = D->value;
	
	CGVector* Y = dmt_calloc(1, sizeof(CGVector));
	Y->data = res;
	Y->len = V->len;
	
	uint64_t i = 0;
	
	for(;i<V->len;i++){
		res[i] = V->data[i] - value;
	}
	
	CGResultNode* result = dmt_calloc(1, sizeof(CGResultNode));
	result->type = CGVT_VECTOR;
	result->value = Y;
	
	return result;
}

/*
 * M-d
 */
CGResultNode* subMD(CGMatrix* M, CGDouble* D){
	uint64_t size = M->rows*M->cols;
	double* res = dmt_calloc(size, sizeof(double));
	double value = D->value;
	
	CGMatrix* Y = dmt_calloc(1, sizeof(CGMatrix));
	Y->rows = M->rows;
	Y->cols = M->cols;
	Y->data = res;
	
	uint64_t i = 0;
	
	for(;i<size;i++){
		res[i] = M->data[i] - value;
	}
	
	CGResultNode* result = dmt_calloc(1, sizeof(CGResultNode));
	result->type = CGVT_MATRIX;
	result->value = Y;
	
	return result;
}

/*
 * V-V
 */
CGResultNode* subVV(CGVector* V1, CGVector* V2){
	if(V1->len != V2->len){
		// TODO: throw error
	}
	
	uint64_t size = V1->len;
	double* res = dmt_calloc(size, sizeof(double));
	
	CGVector* Y = dmt_calloc(1, sizeof(CGVector));
	Y->len = V1->len;
	Y->data = res;
	
	uint64_t i = 0;
	
	for(;i<size;i++){
		res[i] = V1->data[i] - V2->data[i];
	}
	
	CGResultNode* result = dmt_calloc(1, sizeof(CGResultNode));
	result->type = CGVT_VECTOR;
	result->value = Y;
	
	return result;
}

/*
 * M-M
 */
CGResultNode* subMM(CGMatrix* M1, CGMatrix* M2){
	if((M1->rows != M2->rows) || (M1->cols != M2->cols)){
		// TODO: throw error
	}
	
	uint64_t size = M1->cols*M1->rows;
	double* res = dmt_calloc(size, sizeof(double));
	
	CGMatrix* Y = dmt_calloc(1, sizeof(CGMatrix));
	Y->rows = M1->rows;
	Y->cols = M1->cols;
	Y->data = res;
	
	uint64_t i = 0;
	
	for(;i<size;i++){
		res[i] = M1->data[i] - M2->data[i];
	}
	
	CGResultNode* result = dmt_calloc(1, sizeof(CGResultNode));
	result->type = CGVT_MATRIX;
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
CGResultNode* mulMtV(CGMatrix* M, CGVector* V){
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


/*
 * exp(d)
 */
CGResultNode* expD(CGDouble* D){
	double y = exp(D->value);
	CGDouble* Y = dmt_calloc(1, sizeof(CGDouble));
	Y->value = y;
	
	CGResultNode* result = dmt_calloc(1, sizeof(CGResultNode));
	result->type = CGVT_DOUBLE;
	result->value = Y;
	
	return result;
}

/*
 * exp(v)
 */
CGResultNode* expV(CGVector* V){
	double* y = dmt_calloc(V->len, sizeof(double));
	CGVector* Y = dmt_calloc(1, sizeof(CGVector));
	Y->data = y;
	Y->len = V->len;
	
	uint64_t i = 0;
	
	for(;i<V->len;i++){
		y[i] = exp(V->data[i]);
	}
	
	CGResultNode* result = dmt_calloc(1, sizeof(CGResultNode));
	result->type = CGVT_VECTOR;
	result->value = Y;
	
	return result;
}

/*
 * exp(M)
 */
CGResultNode* expM(CGMatrix* M){
	uint64_t size = M->rows*M->cols;
	
	double* y = dmt_calloc(size, sizeof(double));
	CGMatrix* Y = dmt_calloc(1, sizeof(CGMatrix));
	Y->data = y;
	Y->rows = M->rows;
	Y->cols = M->cols;
	
	uint64_t i = 0;
	
	for(;i<size;i++){
		y[i] = exp(M->data[i]);
	}
	
	CGResultNode* result = dmt_calloc(1, sizeof(CGResultNode));
	result->type = CGVT_MATRIX;
	result->value = Y;
	
	return result;
}


/*
 * log(d)
 */
CGResultNode* logD(CGDouble* D){
	double y = log(D->value);
	CGDouble* Y = dmt_calloc(1, sizeof(CGDouble));
	Y->value = y;
	
	CGResultNode* result = dmt_calloc(1, sizeof(CGResultNode));
	result->type = CGVT_DOUBLE;
	result->value = Y;
	
	return result;
}

/*
 * log(V)
 */
CGResultNode* logV(CGVector* V){
	double* y = dmt_calloc(V->len, sizeof(double));
	CGVector* Y = dmt_calloc(1, sizeof(CGVector));
	Y->data = y;
	Y->len = V->len;
	
	uint64_t i = 0;
	
	for(;i<V->len;i++){
		y[i] = log(V->data[i]);
	}
	
	CGResultNode* result = dmt_calloc(1, sizeof(CGResultNode));
	result->type = CGVT_VECTOR;
	result->value = Y;
	
	return result;
}

/*
 * exp(M)
 */
CGResultNode* logM(CGMatrix* M){
	uint64_t size = M->rows*M->cols;
	
	double* y = dmt_calloc(size, sizeof(double));
	CGMatrix* Y = dmt_calloc(1, sizeof(CGMatrix));
	Y->data = y;
	Y->rows = M->rows;
	Y->cols = M->cols;
	
	uint64_t i = 0;
	
	for(;i<size;i++){
		y[i] = log(M->data[i]);
	}
	
	CGResultNode* result = dmt_calloc(1, sizeof(CGResultNode));
	result->type = CGVT_MATRIX;
	result->value = Y;
	
	return result;
}



CGResultNode* processUnaryOperation(CGUnaryOperationType type, CGNode* uhs){
	CGVarType uhsType = CGVT_DOUBLE;
	void* uhsValue = NULL;
	
	if(uhs->type == CGNT_CONSTANT){
		uhsType = uhs->constant->type;
		uhsValue = uhs->constant->value;
	}
	else if (uhs->type ==  CGNT_VARIABLE) {
		uhsType = uhs->var->type;
		uhsValue= uhs->var->value;
	}
	else
	{
		CGResultNode* lhsResult = computeCGNode(uhs);
		uhsType = lhsResult->type;
		uhsValue = lhsResult->value;
	}
	
	switch(type){
		case CGBOT_EXP:{
			if(uhsType == CGVT_DOUBLE){
				return expD((CGDouble*)uhsValue);
			}
			
			if(uhsType == CGVT_VECTOR){
				return expV((CGVector*)uhsValue);
			}
			
			if(uhsType == CGVT_MATRIX){
				return expM((CGMatrix*)uhsValue);
			}
		}
		
		
		case CGBOT_LOG:{
			if(uhsType == CGVT_DOUBLE){
				return logD((CGDouble*)uhsValue);
			}
			
			if(uhsType == CGVT_VECTOR){
				return logV((CGVector*)uhsValue);
			}
			
			if(uhsType == CGVT_MATRIX){
				return logM((CGMatrix*)uhsValue);
			}
		}
	
		case CGBOT_INV:
		case CGBOT_TRANSPOSE:
			return NULL;
	}
}

CGResultNode* processBinaryOperation(CGBinaryOperationType type, CGNode* lhs, CGNode* rhs, CGNode* parentNode){
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
		case CGBOT_ADD:{
			if((lhsType == CGVT_DOUBLE) && (rhsType == CGVT_DOUBLE)){
				return addDD((CGDouble*)lhsValue, (CGDouble*)rhsValue);
			}
			
			if((lhsType == CGVT_VECTOR) && (rhsType == CGVT_DOUBLE)){
				return addVD((CGVector*)lhsValue, (CGDouble*)rhsValue);
			}
			
			
			if((lhsType == CGVT_DOUBLE) && (rhsType == CGVT_VECTOR)){
				return addVD((CGVector*)rhsValue, (CGDouble*)lhsValue);
			}
			
			if((lhsType == CGVT_MATRIX) && (rhsType == CGVT_DOUBLE)){
				return addMD((CGMatrix*)lhsValue, (CGDouble*)rhsValue);
			}
			
			if((lhsType == CGVT_DOUBLE) && (rhsType == CGVT_MATRIX)){
				return addMD((CGMatrix*)rhsValue, (CGDouble*)lhsValue);
			}
			
			if((lhsType == CGVT_VECTOR) && (rhsType == CGVT_VECTOR)){
				return addVV((CGVector*)lhsValue, (CGVector*)rhsValue);
			}
			
			if((lhsType == CGVT_MATRIX) && (rhsType == CGVT_MATRIX)){
				return addMM((CGMatrix*)lhsValue, (CGMatrix*)rhsValue);
			}
			
			// TODO: Handle error in a better way
			throwUnsupportedBinaryOperationException(type, lhs, rhs);
			CGResultNode* rhsResult = dmt_calloc(1, sizeof(CGResultNode));
			rhsResult->error = dmt_calloc(1, sizeof(CGError));
			rhsResult->error->errorType = CGET_INCOMPATIBLE_ARGS_EXCEPTION;
			rhsResult->error->faultyNode = parentNode;
		}
		
		case CGBOT_SUB:{
			if((lhsType == CGVT_DOUBLE) && (rhsType == CGVT_DOUBLE)){
				return subDD((CGDouble*)lhsValue, (CGDouble*)rhsValue);
			}
			
			if((lhsType == CGVT_VECTOR) && (rhsType == CGVT_DOUBLE)){
				return subVD((CGVector*)lhsValue, (CGDouble*)rhsValue);
			}
			
			if((lhsType == CGVT_MATRIX) && (rhsType == CGVT_DOUBLE)){
				return subMD((CGMatrix*)lhsValue, (CGDouble*)rhsValue);
			}
			
			if((lhsType == CGVT_VECTOR) && (rhsType == CGVT_VECTOR)){
				return subVV((CGVector*)lhsValue, (CGVector*)rhsValue);
			}
			
			if((lhsType == CGVT_MATRIX) && (rhsType == CGVT_MATRIX)){
				return subMM((CGMatrix*)lhsValue, (CGMatrix*)rhsValue);
			}
			
			throwUnsupportedBinaryOperationException(type, lhs, rhs);
		}
		
		case CGBOT_DIV:{
			if((lhsType == CGVT_DOUBLE) && (rhsType == CGVT_DOUBLE)){
				return divDD((CGDouble*)lhsValue, (CGDouble*)rhsValue);
			}
			
			if((lhsType == CGVT_VECTOR) && (rhsType == CGVT_DOUBLE)){
				return divVD((CGVector*)lhsValue, (CGDouble*)rhsValue);
			}
			
			if((lhsType == CGVT_MATRIX) && (rhsType == CGVT_DOUBLE)){
				return divMD((CGMatrix*)lhsValue, (CGDouble*)rhsValue);
			}
			
			throwUnsupportedBinaryOperationException(type, lhs, rhs);
		}
		
		case CGBOT_MULT:{
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
			result = processBinaryOperation(node->bop->type, node->bop->lhs, node->bop->rhs, node);
			break;
		case CGNT_UNARY_OPERATION:
			result = processUnaryOperation(node->uop->type, node->uop->uhs, node);
	}
	
	return result;
}
