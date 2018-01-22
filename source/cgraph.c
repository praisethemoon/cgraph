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

#include "cgraph.h"
#include "cg_operation.h"
#include "cg_types.h"
#include "cg_variables.h"
#include "cg_errors.h"

#include "memory.h"

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
	
	return result;
}

/*
 * d.V == V.d
 */
CGResultNode* mulDV(CGDouble* a, CGVector* V){
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
CGResultNode* mulDM(CGDouble* a, CGMatrix* M){
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
 * V.V
 */
CGResultNode* crossVV(CGVector* V1, CGVector* V2){
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
		res[i] = V1->data[i] * V2->data[i];
	}
	
	CGResultNode* result = dmt_calloc(1, sizeof(CGResultNode));
	result->type = CGVT_VECTOR;
	result->value = Y;
	
	return result;
}


/*
 * *********************
 * Dot Product
 * *********************
 */

/*
 * V.V
 */
CGResultNode* dotVV(CGVector* V1, CGVector* V2){
	if(V1->len != V2->len){
		// TODO: throw error
	}
	
	uint64_t size = V1->len;
	double res = 0;
	
	CGDouble* Y = dmt_calloc(1, sizeof(CGDouble));
	
	Y->value = cblas_ddot(V1->len, V1->data, 1, V2->data, 1);
	
	CGResultNode* result = dmt_calloc(1, sizeof(CGResultNode));
	result->type = CGVT_DOUBLE;
	result->value = Y;
	
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
 * TODO: optimize as Multiplication of inverse of `d`
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
 * M+V
 */
CGResultNode* addMV(CGMatrix* M, CGVector* V){
	if(M->rows != V->len){
		// TODO: throw error
	}
	
	uint64_t size = M->cols*M->rows;
	double* res = dmt_calloc(size, sizeof(double));
	
	CGMatrix* Y = dmt_calloc(1, sizeof(CGMatrix));
	Y->rows = M->rows;
	Y->cols = M->cols;
	Y->data = res;
	
	uint64_t i = 0;
	
	for(;i<size;i++){
		res[i] = M->data[i] + V->data[(i)/M->cols];
	}
	
	CGResultNode* result = dmt_calloc(1, sizeof(CGResultNode));
	result->type = CGVT_MATRIX;
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
 * d-V
 */
CGResultNode* subDV(CGDouble* D, CGVector* V){
	double* res = dmt_calloc(V->len, sizeof(double));
	double value = D->value;
	
	CGVector* Y = dmt_calloc(1, sizeof(CGVector));
	Y->data = res;
	Y->len = V->len;
	
	uint64_t i = 0;
	
	for(;i<V->len;i++){
		res[i] = value - V->data[i];
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
 * D-M
 */
CGResultNode* subDM(CGDouble* D, CGMatrix* M){
	uint64_t size = M->rows*M->cols;
	double* res = dmt_calloc(size, sizeof(double));
	double value = D->value;
	
	CGMatrix* Y = dmt_calloc(1, sizeof(CGMatrix));
	Y->rows = M->rows;
	Y->cols = M->cols;
	Y->data = res;
	
	uint64_t i = 0;
	
	for(;i<size;i++){
		res[i] = value - M->data[i];
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
 * M-V
 */
CGResultNode* subMV(CGMatrix* M, CGVector* V){
	if(M->rows != V->len){
		// TODO: throw error
	}
	
	uint64_t size = M->cols*M->rows;
	double* res = dmt_calloc(size, sizeof(double));
	
	CGMatrix* Y = dmt_calloc(1, sizeof(CGMatrix));
	Y->rows = M->rows;
	Y->cols = M->cols;
	Y->data = res;
	
	uint64_t i = 0;
	
	for(;i<size;i++){
		res[i] = M->data[i] - V->data[(i)/M->cols];
	}
	
	CGResultNode* result = dmt_calloc(1, sizeof(CGResultNode));
	result->type = CGVT_MATRIX;
	result->value = Y;
	
	return result;
}

/*
 * V-M
 */
CGResultNode* subVM(CGVector* V, CGMatrix* M){
	if(M->rows != V->len){
		// TODO: throw error
	}
	
	uint64_t size = M->cols*M->rows;
	double* res = dmt_calloc(size, sizeof(double));
	
	CGMatrix* Y = dmt_calloc(1, sizeof(CGMatrix));
	Y->rows = M->rows;
	Y->cols = M->cols;
	Y->data = res;
	
	uint64_t i = 0;
	
	for(;i<size;i++){
		res[i] = V->data[(i)/M->cols] - M->data[i];
	}
	
	CGResultNode* result = dmt_calloc(1, sizeof(CGResultNode));
	result->type = CGVT_MATRIX;
	result->value = Y;
	
	return result;
}


/*
 * *********************
 * Power
 * *********************
 */

/*
 * d^d
 */
CGResultNode* powDD(CGDouble* D1, CGDouble* D2){
	double res = pow(D1->value, D2->value);
	
	CGDouble* Y = dmt_calloc(1, sizeof(CGDouble));
	Y->value = res;
	
	CGResultNode* result = dmt_calloc(1, sizeof(CGResultNode));
	result->type = CGVT_DOUBLE;
	result->value = Y;
	
	return result;
}

/*
 * V^d
 */
CGResultNode* powVD(CGVector* V, CGDouble* D){
	double* res = dmt_calloc(V->len, sizeof(double));
	double value = D->value;
	
	CGVector* Y = dmt_calloc(1, sizeof(CGVector));
	Y->data = res;
	Y->len = V->len;
	
	uint64_t i = 0;
	
	for(;i<V->len;i++){
		res[i] = pow(V->data[i], value);
	}
	
	CGResultNode* result = dmt_calloc(1, sizeof(CGResultNode));
	result->type = CGVT_VECTOR;
	result->value = Y;
	
	return result;
}

/*
 * M^d
 */
CGResultNode* powMD(CGMatrix* M, CGDouble* D){
	uint64_t size = M->rows*M->cols;
	double* res = dmt_calloc(size, sizeof(double));
	double value = D->value;
	
	CGMatrix* Y = dmt_calloc(1, sizeof(CGMatrix));
	Y->rows = M->rows;
	Y->cols = M->cols;
	Y->data = res;
	
	uint64_t i = 0;
	
	for(;i<size;i++){
		res[i] = pow(M->data[i], value);
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


/*
 * *********************
 * Invert
 * *********************
 */

/*
 * *********************
 * Transpose
 * *********************
 */

/*
 * M^t
 */
CGResultNode* transposeM(CGMatrix* M){
	uint64_t size = M->rows*M->cols;
	
	double* y = dmt_calloc(size, sizeof(double));
	CGMatrix* Y = dmt_calloc(1, sizeof(CGMatrix));
	Y->data = y;
	Y->rows = M->cols;
	Y->cols = M->rows;
	Y->data = y;
	
	uint64_t i = 0;
	uint64_t j = 0;
	
	for(;i<M->rows;i++){
		for(j = 0;j<M->cols;j++){
			y[j*Y->cols+i] = M->data[i*M->cols +j];
		}
	}
	
	CGResultNode* result = dmt_calloc(1, sizeof(CGResultNode));
	result->type = CGVT_MATRIX;
	result->value = Y;
	
	return result;
}
CGResultNode* processUnaryOperation(CGraph* graph, CGUnaryOperationType type, CGNode* uhs, CGNode* parentNode){
	CGVarType uhsType = CGVT_DOUBLE;
	void* uhsValue = NULL;
	
	if(uhs->type == CGNT_CONSTANT){
		uhsType = uhs->constant->type;
		uhsValue = uhs->constant->value;
	}
	else
	{
		CGResultNode* lhsResult = computeCGNode(graph, uhs);
		uhsType = lhsResult->type;
		uhsValue = lhsResult->value;
	}
	
	switch(type){
		case CGUOT_EXP:{
			if(uhsType == CGVT_DOUBLE){
				return expD((CGDouble*)uhsValue);
			}
			
			if(uhsType == CGVT_VECTOR){
				return expV((CGVector*)uhsValue);
			}
			
			if(uhsType == CGVT_MATRIX){
				return expM((CGMatrix*)uhsValue);
			}
			break;
		}
		
		
		case CGUOT_LOG:{
			if(uhsType == CGVT_DOUBLE){
				return logD((CGDouble*)uhsValue);
			}
			
			if(uhsType == CGVT_VECTOR){
				return logV((CGVector*)uhsValue);
			}
			
			if(uhsType == CGVT_MATRIX){
				return logM((CGMatrix*)uhsValue);
			}
			break;
		}
	
		case CGUOT_MINUS:{
			if(uhsType == CGVT_DOUBLE){
				CGDouble* rhs = dmt_calloc(1, sizeof(CGDouble));
				rhs->value = -1;
				
				CGResultNode* res = mulDD((CGDouble*)uhsValue, rhs);
				dmt_free(rhs);
				return res;
			}
			
			if(uhsType == CGVT_VECTOR){
				CGDouble* lhs = dmt_calloc(1, sizeof(CGDouble));
				lhs->value = -1;
				
				CGResultNode* res = mulDV(lhs, (CGVector*)uhsValue);
				dmt_free(lhs);
				return res;
			}
			
			if(uhsType == CGVT_MATRIX){
				CGDouble* lhs = dmt_calloc(1, sizeof(CGDouble));
				lhs->value = -1;
				CGResultNode* res = mulDM(lhs, (CGMatrix*)uhsValue);
				dmt_free(lhs);
				return res;
			}
			break;
		}
		case CGUOT_INV:
			return returnResultError(graph, CGET_OPERATION_NOT_IMPLEMENTED, parentNode);
			
		case CGUOT_TRANSPOSE:{
			if(uhsType == CGVT_MATRIX){
				return transposeM((CGMatrix*)uhsValue);
			}
			break;
		}
	}
	printf("FUCK2! %d\n", type);
	
	return returnResultError(graph, CGET_INCOMPATIBLE_ARGS_EXCEPTION, parentNode);
}

CGResultNode* processBinaryOperation(CGraph* graph, CGBinaryOperationType type, CGNode* lhs, CGNode* rhs, CGNode* parentNode){
	CGVarType lhsType = CGVT_DOUBLE;
	CGVarType rhsType = CGVT_DOUBLE;
	
	void* lhsValue = NULL;
	void* rhsValue = NULL;
	
	// LHS
	if(lhs->type == CGNT_CONSTANT){
		lhsType = lhs->constant->type;
		lhsValue = lhs->constant->value;
	}
	else
	{
		CGResultNode* lhsResult = computeCGNode(graph, lhs);
		lhsType = lhsResult->type;
		lhsValue = lhsResult->value;
	}
	// RHS
	if(rhs->type == CGNT_CONSTANT){
		rhsType = rhs->constant->type;
		rhsValue = rhs->constant->value;
	}
	else
	{
		CGResultNode* rhsResult = computeCGNode(graph, rhs);
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
			
			if((lhsType == CGVT_MATRIX) && (rhsType == CGVT_VECTOR)){
				return addMV((CGMatrix*)lhsValue, (CGVector*)rhsValue);
			}
			
			if((lhsType == CGVT_VECTOR) && (rhsType == CGVT_MATRIX)){
				return addMV((CGMatrix*)rhsValue, (CGVector*)lhsValue);
			}
			
			if((lhsType == CGVT_MATRIX) && (rhsType == CGVT_MATRIX)){
				return addMM((CGMatrix*)lhsValue, (CGMatrix*)rhsValue);
			}
			break;
		}
		
		case CGBOT_SUB:{
			if((lhsType == CGVT_DOUBLE) && (rhsType == CGVT_DOUBLE)){
				return subDD((CGDouble*)lhsValue, (CGDouble*)rhsValue);
			}
			
			if((lhsType == CGVT_VECTOR) && (rhsType == CGVT_DOUBLE)){
				return subVD((CGVector*)lhsValue, (CGDouble*)rhsValue);
			}
			
			if((lhsType == CGVT_DOUBLE) && (rhsType == CGVT_VECTOR)){
				return subDV((CGDouble*)lhsValue, (CGVector*)rhsValue);
			}
			
			if((lhsType == CGVT_MATRIX) && (rhsType == CGVT_DOUBLE)){
				return subMD((CGMatrix*)lhsValue, (CGDouble*)rhsValue);
			}
			
			if((lhsType == CGVT_DOUBLE) && (rhsType == CGVT_MATRIX)){
				return subDM((CGDouble*)lhsValue, (CGMatrix*)rhsValue);
			}
			
			if((lhsType == CGVT_VECTOR) && (rhsType == CGVT_VECTOR)){
				return subVV((CGVector*)lhsValue, (CGVector*)rhsValue);
			}
			
			if((lhsType == CGVT_MATRIX) && (rhsType == CGVT_MATRIX)){
				return subMM((CGMatrix*)lhsValue, (CGMatrix*)rhsValue);
			}
			
			if((lhsType == CGVT_MATRIX) && (rhsType == CGVT_VECTOR)){
				return subMV((CGMatrix*)lhsValue, (CGVector*)rhsValue);
			}
			
			if((lhsType == CGVT_VECTOR) && (rhsType == CGVT_MATRIX)){
				return subVM((CGVector*)lhsValue, (CGMatrix*)rhsValue);
			}
			break;
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
			break;
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
				return mulDV((CGDouble*)lhsValue, (CGVector*)rhsValue);
			}
			
			if((lhsType == CGVT_VECTOR) && (rhsType == CGVT_DOUBLE)){
				return mulDV((CGDouble*)rhsValue, (CGVector*)lhsValue);
			}
			
			if((lhsType == CGVT_DOUBLE) && (rhsType == CGVT_MATRIX)){
				return mulDM((CGDouble*)lhsValue, (CGMatrix*)rhsValue);
			}
			
			if((lhsType == CGVT_MATRIX) && (rhsType == CGVT_DOUBLE)){
				return mulDM((CGDouble*)rhsValue, (CGMatrix*)lhsValue);
			}
			
			if((lhsType == CGVT_VECTOR) && (rhsType == CGVT_VECTOR)){
				return crossVV((CGVector*)rhsValue, (CGVector*)lhsValue);
			}
			break;
		}
		
		case CGBOT_POW:{
			if((lhsType == CGVT_DOUBLE) && (rhsType == CGVT_DOUBLE)){
				return powDD((CGDouble*)lhsValue, (CGDouble*)rhsValue);
			}
			
			if((lhsType == CGVT_VECTOR) && (rhsType == CGVT_DOUBLE)){
				return powVD((CGVector*)lhsValue, (CGDouble*)rhsValue);
			}
			
			if((lhsType == CGVT_MATRIX) && (rhsType == CGVT_DOUBLE)){
				return powMD((CGMatrix*)lhsValue, (CGDouble*)rhsValue);
			}
			break;
		}
		
		case CGBOT_DOT: {
			
			if((lhsType == CGVT_VECTOR) && (rhsType == CGVT_VECTOR)){
				return dotVV((CGVector*)lhsValue, (CGVector*)rhsValue);
			}
			break;
		}
		
		case CGBOT_TMULT:
			return returnResultError(graph, CGET_OPERATION_NOT_IMPLEMENTED, parentNode);
	}
	printf("FUCK!\n");
	return returnResultError(graph, CGET_INCOMPATIBLE_ARGS_EXCEPTION, parentNode);
}

CGResultNode* computeRawNode(CGNode* node){
	return computeCGNode(NULL, node);
}


CGResultNode* computeCGNode(CGraph* graph, CGNode* node){
	CGResultNode* result = NULL;
	
	switch(node->type){
		case CGNT_CONSTANT:
			result = dmt_calloc(1, sizeof(CGResultNode));
			result->type = node->constant->type;
			result->value = node->constant->value;
			break;

		case CGNT_VARIABLE:{
			result = dmt_calloc(1, sizeof(CGResultNode));
			CGNode* constantNode = *map_get(&graph->vars, node->var->name);
			CGResultNode* rnode = computeCGNode(graph, constantNode);
			result->type = rnode->type;
			result->value = rnode->value;
			break;
		}
		case CGNT_BINARY_OPERATION:
			result = processBinaryOperation(graph, node->bop->type, node->bop->lhs, node->bop->rhs, node);
			break;
		case CGNT_UNARY_OPERATION:
			result = processUnaryOperation(graph, node->uop->type, node->uop->uhs, node);
			break;
			
		case CGNT_GRAPH:
			return returnResultError(graph, CGET_OPERATION_NOT_IMPLEMENTED, node);
	}
	CGResultNode* res = result;
	/*
	switch(res->type){
		case CGVT_DOUBLE:{
			CGDouble* value = (CGDouble*)res->value;
			printf("value %f\n", value->value);
			break;
		}
		
			
		case CGVT_VECTOR:{
			CGVector* value = (CGVector*)res->value;
			
			
			uint64_t i = 0;
			printf("Vector len: %d\n", value->len);
			for(;i<value->len;i++){
				printf("\t%d\t%lf\n", i, value->data[i]);
			}
			
			break;
		}
			
		case CGVT_MATRIX:{
			CGMatrix* value = (CGMatrix*)res->value;
			printf("Matrix len: %dx%d\n", value->rows, value->cols);
			
			uint64_t i = 0;
			uint64_t j = 0;
			for(;i<value->rows;i++){
				for(j = 0;j<value->cols;j++){
					printf("\t%lf", value->data[i*value->cols +j]);
				}
				printf("\n");
			}
			break;
		}
	}
	*/
	
	return result;
}

CGResultNode* computeGraph(CGraph* graph){
	return computeCGNode(graph, graph->root);
}

void freeNode(CGNode* node){
}

void freeGraph(CGraph* graph){
	
}