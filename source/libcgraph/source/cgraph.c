/*
 * 
 * 
 * 
 */

#include <inttypes.h>
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
#include "cg_constants.h"
#include "cg_enums.h"
#include "cg_factory.h"
#include "cg_math.h"

#include <malloc.h>

#define CHECK_RESULT(node) \
if(node->error != NULL){\
	return node;\
}

/*
 * Works only with constant types
 */
void* copyNode(CGNode* node){
	CGNode* n = calloc(1, sizeof(CGNode));
	if(node->type != CGNT_CONSTANT){
		fprintf(stderr, "Call to copyNodeValue with a non-constant node.\n... This should not happen, but who knows these days.");
		exit(-1);
	}
	
	n->type = CGNT_CONSTANT;
	n->constant = calloc(1, sizeof(CGPConstant));
	n->constant->type = node->constant->type;
	
	switch(node->constant->type){
		case CGVT_DOUBLE: {
			CGDouble* d = calloc(1, sizeof(CGDouble));
			d->value = ((CGDouble*)node->constant->value)->value;
			
			n->constant->value = d;
			break;
		}
		
		case CGVT_VECTOR: {
			CGVector* V = calloc(1, sizeof(CGVector));
			CGVector* src = (CGVector*)node->constant->value;
			
			V->len = src->len;
			V->data = calloc(V->len, sizeof(double));
			
			memcpy(V->data, src->data, V->len*sizeof(double));
			
			n->constant->value = V;
			break;
		}
		
		case CGVT_MATRIX: {
			CGMatrix* M = calloc(1, sizeof(CGMatrix));
			CGMatrix* src = (CGMatrix*)node->constant->value;
			
			uint64_t size = src->cols * src->rows;
			
			M->rows = src->rows;
			M->cols = src->cols;
			M->data = calloc(size, sizeof(double));
			
			memcpy(M->data, src->data, size*sizeof(double));
			
			n->constant->value = M;
			break;
		}
	}
}

void* copyNodeValue(CGNode* node){
	if(node->type != CGNT_CONSTANT){
		fprintf(stderr, "Call to copyNodeValue with a non-constant node.\n... This should not happen, but who knows these days.");
		exit(-1);
	}
	
	switch(node->constant->type){
		case CGVT_DOUBLE: {
			CGDouble* d = calloc(1, sizeof(CGDouble));
			d->value = ((CGDouble*)node->constant->value)->value;
			
			return d;
		}
		
		case CGVT_VECTOR: {
			CGVector* V = calloc(1, sizeof(CGVector));
			CGVector* src = (CGVector*)node->constant->value;
			
			V->len = src->len;
			V->data = calloc(V->len, sizeof(double));
			
			memcpy(V->data, src->data, V->len*sizeof(double));
			return V;
		}
		
		case CGVT_MATRIX: {
			CGMatrix* M = calloc(1, sizeof(CGMatrix));
			CGMatrix* src = (CGMatrix*)node->constant->value;
			
			uint64_t size = src->cols * src->rows;
			
			M->rows = src->rows;
			M->cols = src->cols;
			M->data = calloc(size, sizeof(double));
			
			memcpy(M->data, src->data, size*sizeof(double));
			return M;
		}
	}
}

/*
 * @Deprecated
 */
void* copyRNodeValue(CGResultNode* node){
	switch(node->type){
		case CGVT_DOUBLE: {
			CGDouble* d = calloc(1, sizeof(CGDouble));
			d->value = ((CGDouble*)node->value)->value;
			
			return d;
		}
		
		case CGVT_VECTOR: {
			CGVector* V = calloc(1, sizeof(CGVector));
			CGVector* src = (CGVector*)node->value;
			
			V->len = src->len;
			V->data = calloc(V->len, sizeof(double));
			
			memcpy(V->data, src->data, V->len*sizeof(double));
			return V;
		}
		
		case CGVT_MATRIX: {
			CGMatrix* M = calloc(1, sizeof(CGMatrix));
			CGMatrix* src = (CGMatrix*)node->value;
			
			uint64_t size = src->cols * src->rows;
			
			M->rows = src->rows;
			M->cols = src->cols;
			M->data = calloc(size, sizeof(double));
			
			memcpy(M->data, src->data, size*sizeof(double));
			return M;
		}
	}
}


CGResultNode* copyResultNode(CGResultNode* node){
	CGResultNode* res = calloc(1, sizeof(CGResultNode));
	
	switch(node->type){
		case CGVT_DOUBLE: {
			CGDouble* d = calloc(1, sizeof(CGDouble));
			CGDouble* d2 = (CGDouble*)node->value;
			d->value = d2->value;
			
			res->value = d;
			break;
		}
		
		case CGVT_VECTOR: {
			CGVector* V = calloc(1, sizeof(CGVector));
			CGVector* src = (CGVector*)node->value;
			
			V->len = src->len;
			V->data = calloc(V->len, sizeof(double));
			
			memcpy(V->data, src->data, V->len*sizeof(double));
			
			res->value = V;
			break;
		}
		
		case CGVT_MATRIX: {
			CGMatrix* M = calloc(1, sizeof(CGMatrix));
			CGMatrix* src = (CGMatrix*)node->value;
			
			uint64_t size = src->cols * src->rows;
			
			M->rows = src->rows;
			M->cols = src->cols;
			M->data = calloc(size, sizeof(double));
			
			memcpy(M->data, src->data, size*sizeof(double));
			
			res->value = M;
			break;
		}
	}
	res->type = node->type;
	
	return res;
}

CGMatrix* vectorToMatrix(CGVector* v){
	CGMatrix* m = calloc(1, sizeof(CGMatrix));
	m->rows = v->len;
	m->cols = 1;
	m->data = v->data;
	
	return m;
}

/*
 * *********************
 * Multiplication
 * *********************
 */

/*
 * d.d 
 */
CGResultNode* mulDD(CGDouble* M, CGDouble* V, CGraph* graph, CGNode* parentNode){
	CGDouble* Y = calloc(1, sizeof(CGDouble));
	Y->value = M->value * V->value;
	
	CGResultNode* result = calloc(1, sizeof(CGResultNode));
	result->type = CGVT_DOUBLE;
	result->value = Y;
	
	return result;
}

/*
 * d.V == V.d
 */
CGResultNode* mulDV(CGDouble* a, CGVector* V, CGraph* graph, CGNode* parentNode){
	double* y = calloc(V->len, sizeof(double));
	CGVector* Y = calloc(1, sizeof(CGVector));
	Y->len = V->len;
	Y->data = y;
	
	if(a->value != 0){
		cblas_dcopy(V->len, V->data, 1, y, 1);
		cblas_dscal(V->len, a->value, y, 1);
	}
	
	CGResultNode* result = calloc(1, sizeof(CGResultNode));
	result->type = CGVT_VECTOR;
	result->value = Y;
	
	return result;
}


/*
 * d.M == M.d
 */
CGResultNode* mulDM(CGDouble* a, CGMatrix* M, CGraph* graph, CGNode* parentNode){
	uint64_t size = M->cols*M->rows;
	double value = a->value;
	
	double* y = calloc(size, sizeof(double));
	CGMatrix* Y = calloc(1, sizeof(CGMatrix));
	Y->rows = M->rows;
	Y->cols = M->cols;
	Y->data = y;
	
	if(a->value != 0){
		cblas_dcopy(size, M->data, 1, y, 1);
		cblas_dscal(size, a->value, y, 1);
	}
	
	CGResultNode* result = calloc(1, sizeof(CGResultNode));
	result->type = CGVT_MATRIX;
	result->value = Y;
	
	return result;
}

/*
 * M*v
 * broadcasting
 */
CGResultNode* mulMV(CGMatrix* M, CGVector* V, CGraph* graph, CGNode* parentNode){
	if(M->cols != V->len){
		char msg[MAX_ERR_FMT_LEN];
		snprintf(msg, MAX_ERR_FMT_LEN, "Cannot Add M(%"PRIu64", %"PRIu64") by V(%"PRIu64")", M->rows, M->cols, V->len);
		return returnResultError(graph, CGET_INCOMPATIBLE_DIMENTIONS_EXCEPTION, parentNode, msg);
	}
	
	uint64_t size = M->cols*M->rows;
	double* res = calloc(size, sizeof(double));
	
	CGMatrix* Y = calloc(1, sizeof(CGMatrix));
	Y->rows = M->rows;
	Y->cols = M->cols;
	Y->data = res;
	
	uint64_t i = 0;
	
	for(;i<size;i++){
		res[i] = M->data[i] * V->data[(i)%V->len];
	}
	
	CGResultNode* result = calloc(1, sizeof(CGResultNode));
	result->type = CGVT_MATRIX;
	result->value = Y;
	
	return result;
}

/*
 * Mv
 */
/*
CGResultNode* mulMV(CGMatrix* M, CGVector* V, CGraph* graph, CGNode* parentNode){
	if(M->cols != V->len){
		char msg[MAX_ERR_FMT_LEN];
		snprintf(msg, MAX_ERR_FMT_LEN, "Cannot Add M(%"PRIu64", %"PRIu64") by V(%"PRIu64")", M->rows, M->cols, V->len);
		return returnResultError(graph, CGET_INCOMPATIBLE_DIMENTIONS_EXCEPTION, parentNode, msg);
	}
	
	uint64_t size = M->cols*M->rows;
	double* res = calloc(size, sizeof(double));
	
	CGMatrix* Y = calloc(1, sizeof(CGMatrix));
	Y->rows = M->rows;
	Y->cols = M->cols;
	Y->data = res;
	
	uint64_t i = 0;
	
	for(;i<size;i++){
		res[i] = M->data[i] * V->data[(i)%V->len];
	}
	
	CGResultNode* result = calloc(1, sizeof(CGResultNode));
	result->type = CGVT_MATRIX;
	result->value = Y;
	
	return result;
}
*/

/*
 * M*N
 * broadcasting
 */
CGResultNode* mulMM(CGMatrix* M1, CGMatrix* M2, CGraph* graph, CGNode* parentNode){
	if((M1->rows != M2->rows) && (M1->cols != M2->cols)){
		char msg[MAX_ERR_FMT_LEN];
		snprintf(msg, MAX_ERR_FMT_LEN, "Cannot multiply M(%"PRIu64", %"PRIu64") by N(%"PRIu64", %"PRIu64")", M1->rows, M1->cols, M2->rows, M2->cols);
		return returnResultError(graph, CGET_INCOMPATIBLE_DIMENTIONS_EXCEPTION, parentNode, msg);
	}
	
	uint64_t size = M1->cols*M1->rows;
	double* res = calloc(size, sizeof(double));
	
	CGMatrix* Y = calloc(1, sizeof(CGMatrix));
	Y->rows = M1->rows;
	Y->cols = M1->cols;
	Y->data = res;
	
	uint64_t i = 0;
	
	for(;i<size;i++){
		res[i] = M1->data[i] * M2->data[i];
	}
	
	CGResultNode* result = calloc(1, sizeof(CGResultNode));
	result->type = CGVT_MATRIX;
	result->value = Y;
	
	return result;
}

/*
 * M.v
 */
CGResultNode* dotMV(CGMatrix* M, CGVector* V, CGraph* graph, CGNode* parentNode){
	if(M->cols != 1){
		char msg[MAX_ERR_FMT_LEN];
		snprintf(msg, MAX_ERR_FMT_LEN, "Cannot calculate M(%"PRIu64", %"PRIu64") DOT V(%"PRIu64")", M->rows, M->cols, V->len);
		return returnResultError(graph, CGET_INCOMPATIBLE_DIMENTIONS_EXCEPTION, parentNode, msg);
	}
	
	double* y = calloc(M->rows*V->len, sizeof(double));
	CGMatrix* Y = calloc(1, sizeof(CGMatrix));
	Y->rows= M->rows;
	Y->cols  = V->len;
	Y->data = y;
	
	/*
	cblas_dgemv(M->shape == CGMS_ROW_MAJOR?CblasRowMajor:CblasColMajor,
		    CblasNoTrans, M->rows, M->cols, 1.0, M->data,
		    M->cols, V->data, 1, 0.0, Y->data, 1);
	*/
	
	
	cblas_dgemm(M->shape == CGMS_ROW_MAJOR?CblasRowMajor:CblasColMajor,
		    CblasNoTrans, CblasNoTrans, M->rows, V->len, M->cols,
		    1.0, M->data, M->cols, V->data, V->len, 0, y, Y->cols);
	
	
	CGResultNode* result = calloc(1, sizeof(CGResultNode));
	result->type = CGVT_MATRIX;
	result->value = Y;
	
	return result;
}


/*
 * v.M
 */
CGResultNode* dotVM(CGVector* V, CGMatrix* M, CGraph* graph, CGNode* parentNode){
	if(M->rows != V->len){
		char msg[MAX_ERR_FMT_LEN];
		snprintf(msg, MAX_ERR_FMT_LEN, "Cannot calculate V(%"PRIu64") DOT M(%"PRIu64", %"PRIu64")", V->len, M->rows, M->cols);
		return returnResultError(graph, CGET_INCOMPATIBLE_DIMENTIONS_EXCEPTION, parentNode, msg);
	}
	
	double* y = calloc(M->cols, sizeof(double));
	CGVector* Y = calloc(1, sizeof(CGVector));
	Y->len = M->rows;
	Y->data = y;
		
	cblas_dgemm(M->shape == CGMS_ROW_MAJOR?CblasRowMajor:CblasColMajor,
		    CblasNoTrans, CblasNoTrans, 1, M->cols, V->len,
		    1.0, V->data, V->len, M->data, M->cols, 0, Y->data, Y->len);
	
	
	CGResultNode* result = calloc(1, sizeof(CGResultNode));
	result->type = CGVT_VECTOR;
	result->value = Y;
	
	return result;
}

/*
 * M.N
 */
CGResultNode* dotMM(CGMatrix* M, CGMatrix* N, CGraph* graph, CGNode* parentNode){
	if(M->cols != N->rows){
		char msg[MAX_ERR_FMT_LEN];
		snprintf(msg, MAX_ERR_FMT_LEN, "Cannot DOT M(%"PRIu64", %"PRIu64") by N(%"PRIu64", %"PRIu64")", M->rows, M->cols, N->rows, N->cols);
		return returnResultError(graph, CGET_INCOMPATIBLE_DIMENTIONS_EXCEPTION, parentNode, msg);
	}
	
	double* z = calloc(M->rows*N->cols, sizeof(double));
	
	CGMatrix* Z = calloc(1, sizeof(CGMatrix));
	Z->rows = M->rows;
	Z->cols = N->cols;
	
	cblas_dgemm(M->shape == CGMS_ROW_MAJOR?CblasRowMajor:CblasColMajor,
		    CblasNoTrans, CblasNoTrans, M->rows, N->cols, M->cols,
		    1.0, M->data, M->cols, N->data, N->cols, 0, z, Z->cols);
	
	Z->data = z;
	
	CGResultNode* result = calloc(1, sizeof(CGResultNode));
	result->type = CGVT_MATRIX;
	result->value = Z;
	
	return result;
}


/*
 * V.V
 */
CGResultNode* crossVV(CGVector* V1, CGVector* V2, CGraph* graph, CGNode* parentNode){
	if(V1->len != V2->len){
		char msg[MAX_ERR_FMT_LEN];
		snprintf(msg, MAX_ERR_FMT_LEN, "Cannot compute Cross Product of V(%"PRIu64") by V(%"PRIu64")", V1->len, V2->len);
		return returnResultError(graph, CGET_INCOMPATIBLE_DIMENTIONS_EXCEPTION, parentNode, msg);
	}
	
	uint64_t size = V1->len;
	double* res = calloc(size, sizeof(double));
	
	CGVector* Y = calloc(1, sizeof(CGVector));
	Y->len = V1->len;
	Y->data = res;
	
	uint64_t i = 0;
	
	for(;i<size;i++){
		res[i] = V1->data[i] * V2->data[i];
	}
	
	CGResultNode* result = calloc(1, sizeof(CGResultNode));
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
CGResultNode* dotVV(CGVector* V1, CGVector* V2, CGraph* graph, CGNode* parentNode){
	if(V1->len != V2->len){
		char msg[MAX_ERR_FMT_LEN];
		snprintf(msg, MAX_ERR_FMT_LEN, "Cannot compute Dot Product of V(%"PRIu64") by V(%"PRIu64")", V1->len, V2->len);
		return returnResultError(graph, CGET_INCOMPATIBLE_DIMENTIONS_EXCEPTION, parentNode, msg);
	}
	
	uint64_t size = V1->len;
	double res = 0;
	
	CGDouble* Y = calloc(1, sizeof(CGDouble));
	
	Y->value = cblas_ddot(V1->len, V1->data, 1, V2->data, 1);
	
	CGResultNode* result = calloc(1, sizeof(CGResultNode));
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
CGResultNode* divDD(CGDouble* D1, CGDouble* D2, CGraph* graph, CGNode* parentNode){
	if(D2->value == 0.0){
		char msg[MAX_ERR_FMT_LEN];
		snprintf(msg, MAX_ERR_FMT_LEN, "Diving by zero D/(D=0)");
		return returnResultError(graph, CGET_DIVIDE_BY_ZERO, parentNode, msg);
	}
	
	double res = D1->value / D2->value;
	
	CGDouble* Y = calloc(1, sizeof(CGDouble));
	Y->value = res;
	
	CGResultNode* result = calloc(1, sizeof(CGResultNode));
	result->type = CGVT_DOUBLE;
	result->value = Y;
	
	return result;
}

/*
 *  V/d
 */
CGResultNode* divVD(CGVector* V, CGDouble* D, CGraph* graph, CGNode* parentNode){
	if(D->value == 0.0){
		char msg[MAX_ERR_FMT_LEN];
		snprintf(msg, MAX_ERR_FMT_LEN, "Diving by zero V/(D=0)");
		return returnResultError(graph, CGET_DIVIDE_BY_ZERO, parentNode, msg);
	}
	
	double* res = calloc(V->len, sizeof(double));
	double value = D->value;
	
	CGVector* Y = calloc(1, sizeof(CGVector));
	Y->data = res;
	Y->len = V->len;
	
	cblas_dcopy(V->len, V->data, 1, res, 1);
	cblas_dscal(V->len, 1.0/D->value, res, 1);
	
	CGResultNode* result = calloc(1, sizeof(CGResultNode));
	result->type = CGVT_VECTOR;
	result->value = Y;
	
	return result;
}

/*
 *  V/V
 */
CGResultNode* divVV(CGVector* V, CGVector* D, CGraph* graph, CGNode* parentNode){
	if(V->len != D->len){
		char msg[MAX_ERR_FMT_LEN];
		snprintf(msg, MAX_ERR_FMT_LEN, "Cannot calculate  V(%"PRIu64") DIV V(%"PRIu64")", V->len, D->len);
		return returnResultError(graph, CGET_INCOMPATIBLE_DIMENTIONS_EXCEPTION, parentNode, msg);
	}
	
	double* res = calloc(V->len, sizeof(double));
	
	CGVector* Y = calloc(1, sizeof(CGVector));
	Y->data = res;
	Y->len = V->len;
	
	uint64_t i = 0;
	for(;i<V->len;i++){
		res[i] = V->data[i]/D->data[i];
	}
	
	CGResultNode* result = calloc(1, sizeof(CGResultNode));
	result->type = CGVT_VECTOR;
	result->value = Y;
	
	return result;
}

/*
 *  d/V element-wise
 */
CGResultNode* divDV(CGDouble* D, CGVector* V, CGraph* graph, CGNode* parentNode){
	double* res = calloc(V->len, sizeof(double));
	double value = D->value;
	
	CGVector* Y = calloc(1, sizeof(CGVector));
	Y->data = res;
	Y->len = V->len;
	
	uint64_t i = 0;
	
	for(;i<V->len;i++){
		res[i] = value / V->data[i];
	}
	
	CGResultNode* result = calloc(1, sizeof(CGResultNode));
	result->type = CGVT_VECTOR;
	result->value = Y;
	
	return result;
}

/*
 *  M/d
 */
CGResultNode* divMD(CGMatrix* M, CGDouble* D, CGraph* graph, CGNode* parentNode){
	if(D->value == 0.0){
		char msg[MAX_ERR_FMT_LEN];
		snprintf(msg, MAX_ERR_FMT_LEN, "Diving by zero M/(D=0)");
		return returnResultError(graph, CGET_DIVIDE_BY_ZERO, parentNode, msg);
	}
	
	uint64_t size = M->rows*M->cols;
	double* res = calloc(size, sizeof(double));
	double value = D->value;
	
	CGMatrix* Y = calloc(1, sizeof(CGMatrix));
	Y->rows = M->rows;
	Y->cols = M->cols;
	Y->data = res;
	
	cblas_dcopy(size, M->data, 1, res, 1);
	cblas_dscal(size, 1.0/D->value, res, 1);
	
	CGResultNode* result = calloc(1, sizeof(CGResultNode));
	result->type = CGVT_MATRIX;
	result->value = Y;
	
	return result;
}

/*
 * M/v
 */
CGResultNode* divMV(CGMatrix* M, CGVector* V, CGraph* graph, CGNode* parentNode){
	if(M->cols != V->len){
		char msg[MAX_ERR_FMT_LEN];
		snprintf(msg, MAX_ERR_FMT_LEN, "Cannot calculate  M(%"PRIu64", %"PRIu64") DIV V(%"PRIu64")", M->rows, M->cols, V->len);
		return returnResultError(graph, CGET_INCOMPATIBLE_DIMENTIONS_EXCEPTION, parentNode, msg);
	}
	
	uint64_t size = M->rows*M->cols;
	double* res = calloc(size, sizeof(double));
	
	CGMatrix* Y = calloc(1, sizeof(CGMatrix));
	Y->rows = M->rows;
	Y->cols = M->cols;
	Y->data = res;
	
	uint64_t i = 0;
	
	for(; i < size;i++)
	{
		res[i] = M->data[i] / V->data[i/M->cols];
	}

	CGResultNode* result = calloc(1, sizeof(CGResultNode));
	result->type = CGVT_MATRIX;
	result->value = Y;
	
	return result;
}

/*
 *  d/M element-wise
 */
CGResultNode* divDM(CGDouble* D, CGMatrix* M, CGraph* graph, CGNode* parentNode){
	uint64_t size = M->rows*M->cols;
	double* res = calloc(size, sizeof(double));
	double value = D->value;
	
	CGMatrix* Y = calloc(1, sizeof(CGMatrix));
	Y->rows = M->rows;
	Y->cols = M->cols;
	Y->data = res;

	uint64_t i = 0;
	
	for(;i<size;i++){
		res[i] = value / M->data[i];
	}
	
	CGResultNode* result = calloc(1, sizeof(CGResultNode));
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
CGResultNode* addDD(CGDouble* D1, CGDouble* D2, CGraph* graph, CGNode* parentNode){
	double res = D1->value + D2->value;
	
	CGDouble* Y = calloc(1, sizeof(CGDouble));
	Y->value = res;
	
	CGResultNode* result = calloc(1, sizeof(CGResultNode));
	result->type = CGVT_DOUBLE;
	result->value = Y;
	
	return result;
}

/*
 * V+d
 */
CGResultNode* addVD(CGVector* V, CGDouble* D, CGraph* graph, CGNode* parentNode){
	double* res = calloc(V->len, sizeof(double));
	double value = D->value;
	
	CGVector* Y = calloc(1, sizeof(CGVector));
	Y->data = res;
	Y->len = V->len;
	
	uint64_t i = 0;
	
	for(;i<V->len;i++){
		res[i] = V->data[i] + value;
	}
	
	CGResultNode* result = calloc(1, sizeof(CGResultNode));
	result->type = CGVT_VECTOR;
	result->value = Y;
	
	return result;
}

/*
 * M+d
 */
CGResultNode* addMD(CGMatrix* M, CGDouble* D, CGraph* graph, CGNode* parentNode){
	uint64_t size = M->rows*M->cols;
	double* res = calloc(size, sizeof(double));
	double value = D->value;
	
	CGMatrix* Y = calloc(1, sizeof(CGMatrix));
	Y->rows = M->rows;
	Y->cols = M->cols;
	Y->data = res;
	
	uint64_t i = 0;
	
	for(;i<size;i++){
		res[i] = M->data[i] + value;
	}
	
	CGResultNode* result = calloc(1, sizeof(CGResultNode));
	result->type = CGVT_MATRIX;
	result->value = Y;
	
	return result;
}

/*
 * V+V
 */
CGResultNode* addVV(CGVector* V1, CGVector* V2, CGraph* graph, CGNode* parentNode){
	if(V1->len != V2->len){
		char msg[MAX_ERR_FMT_LEN];
		snprintf(msg, MAX_ERR_FMT_LEN, "Cannot Add V(%"PRIu64") by V(%"PRIu64")", V1->len, V2->len);
		return returnResultError(graph, CGET_INCOMPATIBLE_DIMENTIONS_EXCEPTION, parentNode, msg);
	}
	
	uint64_t size = V1->len;
	double* res = calloc(size, sizeof(double));
	
	CGVector* Y = calloc(1, sizeof(CGVector));
	Y->len = V1->len;
	Y->data = res;
	
	uint64_t i = 0;
	
	for(;i<size;i++){
		res[i] = V1->data[i] + V2->data[i];
	}
	
	CGResultNode* result = calloc(1, sizeof(CGResultNode));
	result->type = CGVT_VECTOR;
	result->value = Y;
	
	return result;
}



/*
 * M+V
 */
CGResultNode* addMV(CGMatrix* M, CGVector* V, CGraph* graph, CGNode* parentNode){
	if(M->cols != V->len){
		char msg[MAX_ERR_FMT_LEN];
		snprintf(msg, MAX_ERR_FMT_LEN, "Cannot Add M(%"PRIu64", %"PRIu64") by V(%"PRIu64")", M->rows, M->cols, V->len);
		return returnResultError(graph, CGET_INCOMPATIBLE_DIMENTIONS_EXCEPTION, parentNode, msg);
	}
	
	uint64_t size = M->cols*M->rows;
	double* res = calloc(size, sizeof(double));
	
	CGMatrix* Y = calloc(1, sizeof(CGMatrix));
	Y->rows = M->rows;
	Y->cols = M->cols;
	Y->data = res;
	
	uint64_t i = 0;
	
	for(;i<size;i++){
		res[i] = M->data[i] + V->data[(i)%V->len];
	}
	
	CGResultNode* result = calloc(1, sizeof(CGResultNode));
	result->type = CGVT_MATRIX;
	result->value = Y;
	
	return result;
}

/*
 * M+M
 */
CGResultNode* addMM(CGMatrix* M1, CGMatrix* M2, CGraph* graph, CGNode* parentNode){
	if((M1->rows != M2->rows) && (M1->cols != M2->cols)){
		char msg[MAX_ERR_FMT_LEN];
		snprintf(msg, MAX_ERR_FMT_LEN, "Cannot add M(%"PRIu64", %"PRIu64") by N(%"PRIu64", %"PRIu64")", M1->rows, M1->cols, M2->rows, M2->cols);
		return returnResultError(graph, CGET_INCOMPATIBLE_DIMENTIONS_EXCEPTION, parentNode, msg);
	}
	
	uint64_t size = M1->cols*M1->rows;
	double* res = calloc(size, sizeof(double));
	
	CGMatrix* Y = calloc(1, sizeof(CGMatrix));
	Y->rows = M1->rows;
	Y->cols = M1->cols;
	Y->data = res;
	
	uint64_t i = 0;
	
	for(;i<size;i++){
		res[i] = M1->data[i] + M2->data[i];
	}
	
	CGResultNode* result = calloc(1, sizeof(CGResultNode));
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
CGResultNode* subDD(CGDouble* D1, CGDouble* D2, CGraph* graph, CGNode* parentNode){
	double res = D1->value - D2->value;
	
	CGDouble* Y = calloc(1, sizeof(CGDouble));
	Y->value = res;
	
	CGResultNode* result = calloc(1, sizeof(CGResultNode));
	result->type = CGVT_DOUBLE;
	result->value = Y;
	
	return result;
}

/*
 * V-d
 */
CGResultNode* subVD(CGVector* V, CGDouble* D, CGraph* graph, CGNode* parentNode){
	double* res = calloc(V->len, sizeof(double));
	double value = D->value;
	
	CGVector* Y = calloc(1, sizeof(CGVector));
	Y->data = res;
	Y->len = V->len;
	
	uint64_t i = 0;
	
	for(;i<V->len;i++){
		res[i] = V->data[i] - value;
	}
	
	CGResultNode* result = calloc(1, sizeof(CGResultNode));
	result->type = CGVT_VECTOR;
	result->value = Y;
	
	return result;
}

/*
 * d-V
 */
CGResultNode* subDV(CGDouble* D, CGVector* V, CGraph* graph, CGNode* parentNode){
	double* res = calloc(V->len, sizeof(double));
	double value = D->value;
	
	CGVector* Y = calloc(1, sizeof(CGVector));
	Y->data = res;
	Y->len = V->len;
	
	uint64_t i = 0;
	
	for(;i<V->len;i++){
		res[i] = value - V->data[i];
	}
	
	CGResultNode* result = calloc(1, sizeof(CGResultNode));
	result->type = CGVT_VECTOR;
	result->value = Y;
	
	return result;
}


/*
 * M-d
 */
CGResultNode* subMD(CGMatrix* M, CGDouble* D, CGraph* graph, CGNode* parentNode){
	uint64_t size = M->rows*M->cols;
	double* res = calloc(size, sizeof(double));
	double value = D->value;
	
	CGMatrix* Y = calloc(1, sizeof(CGMatrix));
	Y->rows = M->rows;
	Y->cols = M->cols;
	Y->data = res;
	
	uint64_t i = 0;
	
	for(;i<size;i++){
		res[i] = M->data[i] - value;
	}
	
	CGResultNode* result = calloc(1, sizeof(CGResultNode));
	result->type = CGVT_MATRIX;
	result->value = Y;
	
	return result;
}

/*
 * D-M
 */
CGResultNode* subDM(CGDouble* D, CGMatrix* M, CGraph* graph, CGNode* parentNode){
	uint64_t size = M->rows*M->cols;
	double* res = calloc(size, sizeof(double));
	double value = D->value;
	
	CGMatrix* Y = calloc(1, sizeof(CGMatrix));
	Y->rows = M->rows;
	Y->cols = M->cols;
	Y->data = res;
	
	uint64_t i = 0;
	
	for(;i<size;i++){
		res[i] = value - M->data[i];
	}
	
	CGResultNode* result = calloc(1, sizeof(CGResultNode));
	result->type = CGVT_MATRIX;
	result->value = Y;
	
	return result;
}


/*
 * V-V
 */
CGResultNode* subVV(CGVector* V1, CGVector* V2, CGraph* graph, CGNode* parentNode){
	if(V1->len != V2->len){
		char msg[MAX_ERR_FMT_LEN];
		snprintf(msg, MAX_ERR_FMT_LEN, "Cannot Substract V(%"PRIu64") by V(%"PRIu64")", V1->len, V2->len);
		return returnResultError(graph, CGET_INCOMPATIBLE_DIMENTIONS_EXCEPTION, parentNode, msg);
	}
	
	uint64_t size = V1->len;
	double* res = calloc(size, sizeof(double));
	
	CGVector* Y = calloc(1, sizeof(CGVector));
	Y->len = V1->len;
	Y->data = res;
	
	uint64_t i = 0;
	
	for(;i<size;i++){
		res[i] = V1->data[i] - V2->data[i];
	}
	
	CGResultNode* result = calloc(1, sizeof(CGResultNode));
	result->type = CGVT_VECTOR;
	result->value = Y;
	
	return result;
}

/*
 * M-M
 */
CGResultNode* subMM(CGMatrix* M1, CGMatrix* M2, CGraph* graph, CGNode* parentNode){
	if((M1->rows != M2->rows) && (M1->cols != M2->cols)){
		char msg[MAX_ERR_FMT_LEN];
		snprintf(msg, MAX_ERR_FMT_LEN, "Cannot Substract M(%"PRIu64", %"PRIu64") by N(%"PRIu64", %"PRIu64")", M1->rows, M1->cols, M2->rows, M2->cols);
		return returnResultError(graph, CGET_INCOMPATIBLE_DIMENTIONS_EXCEPTION, parentNode, msg);
	}
	
	uint64_t size = M1->cols*M1->rows;
	double* res = calloc(size, sizeof(double));
	
	CGMatrix* Y = calloc(1, sizeof(CGMatrix));
	Y->rows = M1->rows;
	Y->cols = M1->cols;
	Y->data = res;
	
	uint64_t i = 0;
	
	for(;i<size;i++){
		res[i] = M1->data[i] - M2->data[i];
	}
	
	CGResultNode* result = calloc(1, sizeof(CGResultNode));
	result->type = CGVT_MATRIX;
	result->value = Y;
	
	return result;
}

/*
 * M-V
 */
CGResultNode* subMV(CGMatrix* M, CGVector* V, CGraph* graph, CGNode* parentNode){
	if(M->rows != V->len){
		char msg[MAX_ERR_FMT_LEN];
		snprintf(msg, MAX_ERR_FMT_LEN, "Cannot Substract M(%"PRIu64", %"PRIu64") by V(%"PRIu64")", M->rows, M->cols, V->len);
		return returnResultError(graph, CGET_INCOMPATIBLE_DIMENTIONS_EXCEPTION, parentNode, msg);
	}
	
	uint64_t size = M->cols*M->rows;
	double* res = calloc(size, sizeof(double));
	
	CGMatrix* Y = calloc(1, sizeof(CGMatrix));
	Y->rows = M->rows;
	Y->cols = M->cols;
	Y->data = res;
	
	uint64_t i = 0;
	
	for(;i<size;i++){
		res[i] = M->data[i] - V->data[(i)/M->cols];
	}
	
	CGResultNode* result = calloc(1, sizeof(CGResultNode));
	result->type = CGVT_MATRIX;
	result->value = Y;
	
	return result;
}

/*
 * V-M
 */
CGResultNode* subVM(CGVector* V, CGMatrix* M, CGraph* graph, CGNode* parentNode){
	
	if(M->rows != V->len){
		char msg[MAX_ERR_FMT_LEN];
		snprintf(msg, MAX_ERR_FMT_LEN, "Cannot Substract V(%"PRIu64") by M(%"PRIu64", %"PRIu64")", V->len, M->rows, M->cols);
		return returnResultError(graph, CGET_INCOMPATIBLE_DIMENTIONS_EXCEPTION, parentNode, msg);
	}
	
	uint64_t size = M->cols*M->rows;
	double* res = calloc(size, sizeof(double));
	
	CGMatrix* Y = calloc(1, sizeof(CGMatrix));
	Y->rows = M->rows;
	Y->cols = M->cols;
	Y->data = res;
	
	uint64_t i = 0;
	
	for(;i<size;i++){
		res[i] = V->data[(i)/M->cols] - M->data[i];
	}
	
	CGResultNode* result = calloc(1, sizeof(CGResultNode));
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
CGResultNode* powDD(CGDouble* D1, CGDouble* D2, CGraph* graph, CGNode* parentNode){
	double res = pow(D1->value, D2->value);
	
	CGDouble* Y = calloc(1, sizeof(CGDouble));
	Y->value = res;
	
	CGResultNode* result = calloc(1, sizeof(CGResultNode));
	result->type = CGVT_DOUBLE;
	result->value = Y;
	
	return result;
}

/*
 * V^d
 */
CGResultNode* powVD(CGVector* V, CGDouble* D, CGraph* graph, CGNode* parentNode){
	double* res = calloc(V->len, sizeof(double));
	double value = D->value;
	
	CGVector* Y = calloc(1, sizeof(CGVector));
	Y->data = res;
	Y->len = V->len;
	
	uint64_t i = 0;
	
	for(;i<V->len;i++){
		res[i] = pow(V->data[i], value);
	}
	
	CGResultNode* result = calloc(1, sizeof(CGResultNode));
	result->type = CGVT_VECTOR;
	result->value = Y;
	
	return result;
}

/*
 * M^d
 */
CGResultNode* powMD(CGMatrix* M, CGDouble* D, CGraph* graph, CGNode* parentNode){
	uint64_t size = M->rows*M->cols;
	double* res = calloc(size, sizeof(double));
	double value = D->value;
	
	CGMatrix* Y = calloc(1, sizeof(CGMatrix));
	Y->rows = M->rows;
	Y->cols = M->cols;
	Y->data = res;
	
	uint64_t i = 0;
	
	for(;i<size;i++){
		res[i] = pow(M->data[i], value);
	}
	
	CGResultNode* result = calloc(1, sizeof(CGResultNode));
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
CGResultNode* mulMtV(CGMatrix* M, CGVector* V, CGraph* graph, CGNode* parentNode){
	double* y = calloc(V->len, sizeof(double));
	CGVector* Y = calloc(1, sizeof(CGVector));
	Y->len = V->len;
	Y->data = y;
	
	cblas_dgemv(M->shape == CGMS_ROW_MAJOR?CblasRowMajor:CblasColMajor,
		    CblasTrans, M->rows, M->cols, 1.0, M->data,
		    V->len, V->data, 1, 0.0, Y->data, 1);
	
	CGResultNode* result = calloc(1, sizeof(CGResultNode));
	result->type = CGVT_VECTOR;
	result->value = Y;
	
	return result;
}


/*
 * exp(d)
 */
CGResultNode* expD(CGDouble* D, CGraph* graph, CGNode* parentNode){
	double y = exp(D->value);
	CGDouble* Y = calloc(1, sizeof(CGDouble));
	Y->value = y;
	
	CGResultNode* result = calloc(1, sizeof(CGResultNode));
	result->type = CGVT_DOUBLE;
	result->value = Y;
	
	return result;
}

/*
 * exp(v)
 */
CGResultNode* expV(CGVector* V, CGraph* graph, CGNode* parentNode){
	double* y = calloc(V->len, sizeof(double));
	CGVector* Y = calloc(1, sizeof(CGVector));
	Y->data = y;
	Y->len = V->len;
	
	uint64_t i = 0;
	
	for(;i<V->len;i++){
		y[i] = exp(V->data[i]);
	}
	
	CGResultNode* result = calloc(1, sizeof(CGResultNode));
	result->type = CGVT_VECTOR;
	result->value = Y;
	
	return result;
}

/*
 * exp(M)
 */
CGResultNode* expM(CGMatrix* M, CGraph* graph, CGNode* parentNode){
	uint64_t size = M->rows*M->cols;
	
	double* y = calloc(size, sizeof(double));
	CGMatrix* Y = calloc(1, sizeof(CGMatrix));
	Y->data = y;
	Y->rows = M->rows;
	Y->cols = M->cols;
	
	uint64_t i = 0;
	
	for(;i<size;i++){
		y[i] = exp(M->data[i]);
	}
	
	CGResultNode* result = calloc(1, sizeof(CGResultNode));
	result->type = CGVT_MATRIX;
	result->value = Y;
	
	return result;
}


/*
 * log(d)
 */
CGResultNode* logD(CGDouble* D, CGraph* graph, CGNode* parentNode){
	double y = log(D->value);
	CGDouble* Y = calloc(1, sizeof(CGDouble));
	Y->value = y;
	
	CGResultNode* result = calloc(1, sizeof(CGResultNode));
	result->type = CGVT_DOUBLE;
	result->value = Y;
	
	return result;
}

/*
 * log(V)
 */
CGResultNode* logV(CGVector* V, CGraph* graph, CGNode* parentNode){
	double* y = calloc(V->len, sizeof(double));
	CGVector* Y = calloc(1, sizeof(CGVector));
	Y->data = y;
	Y->len = V->len;
	
	uint64_t i = 0;
	
	for(;i<V->len;i++){
		y[i] = log(V->data[i]);
	}
	
	CGResultNode* result = calloc(1, sizeof(CGResultNode));
	result->type = CGVT_VECTOR;
	result->value = Y;
	
	return result;
}

/*
 * exp(M)
 */
CGResultNode* logM(CGMatrix* M, CGraph* graph, CGNode* parentNode){
	uint64_t size = M->rows*M->cols;
	
	double* y = calloc(size, sizeof(double));
	CGMatrix* Y = calloc(1, sizeof(CGMatrix));
	Y->data = y;
	Y->rows = M->rows;
	Y->cols = M->cols;
	
	uint64_t i = 0;
	
	for(;i<size;i++){
		y[i] = log(M->data[i]);
	}
	
	CGResultNode* result = calloc(1, sizeof(CGResultNode));
	result->type = CGVT_MATRIX;
	result->value = Y;
	
	return result;
}

/*
 * *********************
 * Sin
 * *********************
 */

/*
 * sin(d)
 */
CGResultNode* sinD(CGDouble* D, CGraph* graph, CGNode* parentNode){
	double y = sin(D->value);
	CGDouble* Y = calloc(1, sizeof(CGDouble));
	Y->value = y;
	
	CGResultNode* result = calloc(1, sizeof(CGResultNode));
	result->type = CGVT_DOUBLE;
	result->value = Y;
	
	return result;
}

/*
 * sin(V)
 */
CGResultNode* sinV(CGVector* V, CGraph* graph, CGNode* parentNode){
	double* y = calloc(V->len, sizeof(double));
	CGVector* Y = calloc(1, sizeof(CGVector));
	Y->data = y;
	Y->len = V->len;
	
	uint64_t i = 0;
	
	for(;i<V->len;i++){
		y[i] = sin(V->data[i]);
	}
	
	CGResultNode* result = calloc(1, sizeof(CGResultNode));
	result->type = CGVT_VECTOR;
	result->value = Y;
	
	return result;
}

/*
 * sin(M)
 */
CGResultNode* sinM(CGMatrix* M, CGraph* graph, CGNode* parentNode){
	uint64_t size = M->rows*M->cols;
	
	double* y = calloc(size, sizeof(double));
	CGMatrix* Y = calloc(1, sizeof(CGMatrix));
	Y->data = y;
	Y->rows = M->rows;
	Y->cols = M->cols;
	
	uint64_t i = 0;
	
	for(;i<size;i++){
		y[i] = sin(M->data[i]);
	}
	
	CGResultNode* result = calloc(1, sizeof(CGResultNode));
	result->type = CGVT_MATRIX;
	result->value = Y;
	
	return result;
}

/*
 * *********************
 * cos
 * *********************
 */

/*
 * cos(d)
 */
CGResultNode* cosD(CGDouble* D, CGraph* graph, CGNode* parentNode){
	double y = cos(D->value);
	CGDouble* Y = calloc(1, sizeof(CGDouble));
	Y->value = y;
	
	CGResultNode* result = calloc(1, sizeof(CGResultNode));
	result->type = CGVT_DOUBLE;
	result->value = Y;
	
	return result;
}

/*
 * cos(V)
 */
CGResultNode* cosV(CGVector* V, CGraph* graph, CGNode* parentNode){
	double* y = calloc(V->len, sizeof(double));
	CGVector* Y = calloc(1, sizeof(CGVector));
	Y->data = y;
	Y->len = V->len;
	
	uint64_t i = 0;
	
	for(;i<V->len;i++){
		y[i] = cos(V->data[i]);
	}
	
	CGResultNode* result = calloc(1, sizeof(CGResultNode));
	result->type = CGVT_VECTOR;
	result->value = Y;
	
	return result;
}

/*
 * cos(M)
 */
CGResultNode* cosM(CGMatrix* M, CGraph* graph, CGNode* parentNode){
	uint64_t size = M->rows*M->cols;
	
	double* y = calloc(size, sizeof(double));
	CGMatrix* Y = calloc(1, sizeof(CGMatrix));
	Y->data = y;
	Y->rows = M->rows;
	Y->cols = M->cols;
	
	uint64_t i = 0;
	
	for(;i<size;i++){
		y[i] = cos(M->data[i]);
	}
	
	CGResultNode* result = calloc(1, sizeof(CGResultNode));
	result->type = CGVT_MATRIX;
	result->value = Y;
	
	return result;
}


/*
 * *********************
 * tan
 * *********************
 */

/*
 * tan(d)
 */
CGResultNode* tanD(CGDouble* D, CGraph* graph, CGNode* parentNode){
	double y = tan(D->value);
	CGDouble* Y = calloc(1, sizeof(CGDouble));
	Y->value = y;
	
	CGResultNode* result = calloc(1, sizeof(CGResultNode));
	result->type = CGVT_DOUBLE;
	result->value = Y;
	
	return result;
}

/*
 * tan(V)
 */
CGResultNode* tanV(CGVector* V, CGraph* graph, CGNode* parentNode){
	double* y = calloc(V->len, sizeof(double));
	CGVector* Y = calloc(1, sizeof(CGVector));
	Y->data = y;
	Y->len = V->len;
	
	uint64_t i = 0;
	
	for(;i<V->len;i++){
		y[i] = tan(V->data[i]);
	}
	
	CGResultNode* result = calloc(1, sizeof(CGResultNode));
	result->type = CGVT_VECTOR;
	result->value = Y;
	
	return result;
}

/*
 * tan(M)
 */
CGResultNode* tanM(CGMatrix* M, CGraph* graph, CGNode* parentNode){
	uint64_t size = M->rows*M->cols;
	
	double* y = calloc(size, sizeof(double));
	CGMatrix* Y = calloc(1, sizeof(CGMatrix));
	Y->data = y;
	Y->rows = M->rows;
	Y->cols = M->cols;
	
	uint64_t i = 0;
	
	for(;i<size;i++){
		y[i] = tan(M->data[i]);
	}
	
	CGResultNode* result = calloc(1, sizeof(CGResultNode));
	result->type = CGVT_MATRIX;
	result->value = Y;
	
	return result;
}


/*
 * *********************
 * tanh
 * *********************
 */

/*
 * tanh(d)
 */
CGResultNode* tanhD(CGDouble* D, CGraph* graph, CGNode* parentNode){
	double y = tanh(D->value);
	CGDouble* Y = calloc(1, sizeof(CGDouble));
	Y->value = y;
	
	CGResultNode* result = calloc(1, sizeof(CGResultNode));
	result->type = CGVT_DOUBLE;
	result->value = Y;
	
	return result;
}

/*
 * tanh(V)
 */
CGResultNode* tanhV(CGVector* V, CGraph* graph, CGNode* parentNode){
	double* y = calloc(V->len, sizeof(double));
	CGVector* Y = calloc(1, sizeof(CGVector));
	Y->data = y;
	Y->len = V->len;
	
	uint64_t i = 0;
	
	for(;i<V->len;i++){
		y[i] = tanh(V->data[i]);
	}
	
	CGResultNode* result = calloc(1, sizeof(CGResultNode));
	result->type = CGVT_VECTOR;
	result->value = Y;
	
	return result;
}

/*
 * tanh(M)
 */
CGResultNode* tanhM(CGMatrix* M, CGraph* graph, CGNode* parentNode){
	uint64_t size = M->rows*M->cols;
	
	double* y = calloc(size, sizeof(double));
	CGMatrix* Y = calloc(1, sizeof(CGMatrix));
	Y->data = y;
	Y->rows = M->rows;
	Y->cols = M->cols;
	
	uint64_t i = 0;
	
	for(;i<size;i++){
		y[i] = tanh(M->data[i]);
	}
	
	CGResultNode* result = calloc(1, sizeof(CGResultNode));
	result->type = CGVT_MATRIX;
	result->value = Y;
	
	return result;
}

/*
 * *********************
 * Invert
 * *********************
 */

// TODO

/*
 * *********************
 * Transpose
 * *********************
 */

/*
 * D^t
 */
CGResultNode* transposeD(CGDouble* D, CGraph* graph, CGNode* parentNode){
	CGDouble* R = calloc(1, sizeof(CGDouble));
	R->value = D->value;
	
	CGResultNode* result = calloc(1, sizeof(CGResultNode));
	result->type = CGVT_DOUBLE;
	result->value = R;
	
	return result;
}


/*
 * V^t
 */
CGResultNode* transposeV(CGVector* V, CGraph* graph, CGNode* parentNode){
	uint64_t size = V->len;
	
	double* y = calloc(size, sizeof(double));
	CGMatrix* Y = calloc(1, sizeof(CGMatrix));
	Y->data = y;
	Y->rows = V->len;
	Y->cols = 1;
	Y->data = y;
	
	uint64_t i = 0;
	uint64_t j = 0;
	
	memcpy(Y->data, V->data, V->len*sizeof(double));
	
	CGResultNode* result = calloc(1, sizeof(CGResultNode));
	result->type = CGVT_MATRIX;
	result->value = Y;
	
	return result;
}


/*
 * M^t
 */
CGResultNode* transposeM(CGMatrix* M, CGraph* graph, CGNode* parentNode){
	uint64_t size = M->rows*M->cols;
	
	double* y = calloc(size, sizeof(double));
	CGMatrix* Y = calloc(1, sizeof(CGMatrix));
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
	
	CGResultNode* result = calloc(1, sizeof(CGResultNode));
	result->type = CGVT_MATRIX;
	result->value = Y;
	
	return result;
}


/*
 * *********************
 * SUM
 * *********************
 */

/*
 * sum(D)
 */
CGResultNode* sumD(CGDouble* D, CGraph* graph, CGNode* parentNode){
	double y = D->value;
	CGDouble* Y = calloc(1, sizeof(CGDouble));
	Y->value = y;
	
	CGResultNode* result = calloc(1, sizeof(CGResultNode));
	result->type = CGVT_DOUBLE;
	result->value = Y;
	
	return result;
}

/*
 * sum(V)
 */
CGResultNode* sumV(CGVector* V, CGraph* graph, CGNode* parentNode){
	double y = 0;
	CGDouble* Y = calloc(1, sizeof(CGDouble));
	
	uint64_t i = 0;
	
	for(;i<V->len;i++){
		y += V->data[i];
	}
	
	Y->value = y;
	CGResultNode* result = calloc(1, sizeof(CGResultNode));
	result->type = CGVT_DOUBLE;
	result->value = Y;
	
	return result;
}

/*
 * TODO:
 * sum(M, axis=axis)
 */

CGResultNode* sumM(CGMatrix* M, CGraph* graph, CGNode* parentNode, uint8_t axis){
	CGVector* V = calloc(1, sizeof(CGVector));
	
	if(axis == 0){
		uint64_t len = M->cols;
		double* y = calloc(len, sizeof(double));
		
		V->data = y;
		V->len = len;
		
		uint64_t i = 0;
		
		for(;i<M->rows*M->cols;i++){
			y[i%len] += M->data[i];
		}
	}
	else {
		uint64_t len = M->rows;
		double* y = calloc(len, sizeof(double));
		
		V->data = y;
		V->len = len;
		
		uint64_t i = 0;
		
		for(;i<M->rows*M->cols;i++){
			y[i/M->cols] += M->data[i];
		}
	}
		
	CGResultNode* result = calloc(1, sizeof(CGResultNode));
	result->type = CGVT_VECTOR;
	result->value = V;
	
	return result;
}


CGResultNode* max(CGNode* X, CGraph* graph){
	CGResultNode* res = computeCGNode(graph, X->axop->uhs);
	
	switch(res->type){
		case CGVT_DOUBLE:
		{
				double y = 0;
				CGDouble* Y = calloc(1, sizeof(CGDouble));
				
				Y->value = ((CGDouble*)res->value)->value;
				CGResultNode* result = calloc(1, sizeof(CGResultNode));
				result->type = CGVT_DOUBLE;
				result->value = Y;
				
				return result;
		}
		
		case CGVT_VECTOR:
		{
			CGVector* v = (CGVector*)res->value;
			CGDouble* Y = calloc(1, sizeof(CGDouble));
			
			uint64_t i = 1;
			double m = v->data[0];
			
			for (;i < v->len; i++){
				if(v->data[i] > m)
					m = v->data[i];
			}
			
			Y->value = m;
			CGResultNode* result = calloc(1, sizeof(CGResultNode));
			result->type = CGVT_DOUBLE;
			result->value = Y;
			
			return result;
		}
		
		case CGVT_MATRIX: {
				CGMatrix* M = (CGMatrix*)res->value;
				CGVector* V = calloc(1, sizeof(CGVector));
	
				if(X->axop->axis == 0){
					uint64_t len = M->cols;
					double* y = calloc(len, sizeof(double));
					
					V->data = y;
					V->len = len;
					
					uint64_t i = 0;
					
					for(;i<M->cols;i++){
						y[i] = M->data[i];
					}
					
					for(i=0;i<M->rows*M->cols;i++){
						if(y[i%len] < M->data[i])
							y[i%len] = M->data[i];
					}
				}
				else {
					uint64_t len = M->rows;
					double* y = calloc(len, sizeof(double));
					
					V->data = y;
					V->len = len;
					
					uint64_t i = 0;
					
					for(;i<M->rows;i++){
						y[i] = M->data[i*M->cols];
					}
					
					for(i=0;i<M->rows*M->cols;i++){
						if(y[i/M->cols] < M->data[i])
							y[i/M->cols] = M->data[i];
					}
				}
					
				CGResultNode* result = calloc(1, sizeof(CGResultNode));
				result->type = CGVT_VECTOR;
				result->value = V;
				
				return result;
			
		}
	}
}


CGResultNode* min(CGNode* X, CGraph* graph){
	CGResultNode* res = computeCGNode(graph, X->axop->uhs);
	
	switch(res->type){
		case CGVT_DOUBLE:
		{
				double y = 0;
				CGDouble* Y = calloc(1, sizeof(CGDouble));
				
				Y->value = ((CGDouble*)res->value)->value;
				CGResultNode* result = calloc(1, sizeof(CGResultNode));
				result->type = CGVT_DOUBLE;
				result->value = Y;
				
				return result;
		}
		
		case CGVT_VECTOR:
		{
			CGVector* v = (CGVector*)res->value;
			CGDouble* Y = calloc(1, sizeof(CGDouble));
			
			uint64_t i = 1;
			double m = v->data[0];
			
			for (;i < v->len; i++){
				if(v->data[i] > m)
					m = v->data[i];
			}
			
			Y->value = m;
			CGResultNode* result = calloc(1, sizeof(CGResultNode));
			result->type = CGVT_DOUBLE;
			result->value = Y;
			
			return result;
		}
		
		case CGVT_MATRIX: {
				CGMatrix* M = (CGMatrix*)res->value;
				CGVector* V = calloc(1, sizeof(CGVector));
	
				if(X->axop->axis == 0){
					uint64_t len = M->cols;
					double* y = calloc(len, sizeof(double));
					
					V->data = y;
					V->len = len;
					
					uint64_t i = 0;
					
					for(;i<M->cols;i++){
						y[i] = M->data[i];
					}
					
					for(i=0;i<M->rows*M->cols;i++){
						if(y[i%len] > M->data[i])
							y[i%len] = M->data[i];
					}
				}
				else {
					uint64_t len = M->rows;
					double* y = calloc(len, sizeof(double));
					
					V->data = y;
					V->len = len;
					
					uint64_t i = 0;
					
					for(;i<M->rows;i++){
						y[i] = M->data[i*M->cols];
					}
					
					for(i=0;i<M->rows*M->cols;i++){
						if(y[i/M->cols] > M->data[i])
							y[i/M->cols] = M->data[i];
					}
				}
					
				CGResultNode* result = calloc(1, sizeof(CGResultNode));
				result->type = CGVT_VECTOR;
				result->value = V;
				
				return result;
			
		}
	}
}


CGResultNode* mean(CGNode* X, CGraph* graph){
	CGResultNode* res = computeCGNode(graph, X->axop->uhs);
	
	switch(res->type){
		case CGVT_DOUBLE:
		{
				double y = 0;
				CGDouble* Y = calloc(1, sizeof(CGDouble));
				
				Y->value = ((CGDouble*)res->value)->value;
				CGResultNode* result = calloc(1, sizeof(CGResultNode));
				result->type = CGVT_DOUBLE;
				result->value = Y;
				
				return result;
		}
		
		case CGVT_VECTOR:
		{
			CGVector* v = (CGVector*)res->value;
			CGDouble* Y = calloc(1, sizeof(CGDouble));
			
			uint64_t i = 0;
			double m = v->data[0];
			
			for (;i < v->len; i++){
				m += v->data[i];
			}
			
			m /= v->len;
			
			Y->value = m;
			CGResultNode* result = calloc(1, sizeof(CGResultNode));
			result->type = CGVT_DOUBLE;
			result->value = Y;
			
			return result;
		}
		
		case CGVT_MATRIX: {
				CGMatrix* M = (CGMatrix*)res->value;
				CGVector* V = calloc(1, sizeof(CGVector));
	
				if(X->axop->axis == 0){
					uint64_t len = M->cols;
					double* y = calloc(len, sizeof(double));
					
					V->data = y;
					V->len = len;
					
					uint64_t i = 0;
					
					for(;i<M->rows*M->cols;i++){
						y[i%len] += M->data[i];
					}
					
					
					for(i=0;i<M->cols;i++){
						y[i] /= M->rows;
					}
				}
				else {
					uint64_t len = M->rows;
					double* y = calloc(len, sizeof(double));
					
					V->data = y;
					V->len = len;
					
					uint64_t i = 0;
					
					for(;i<M->rows*M->cols;i++){
						y[i/M->cols] += M->data[i];
					}
					
					for(i=0;i<M->rows;i++){
						y[i] /= M->cols;
					}
				}
					
				CGResultNode* result = calloc(1, sizeof(CGResultNode));
				result->type = CGVT_VECTOR;
				result->value = V;
				
				return result;
			
		}
	}
}

/*
 * Computational Graph traversing
 */

CGResultNode* processUnaryOperation(CGraph* graph, CGUnaryOperationType type, CGNode* uhs, CGNode* parentNode){
	CGVarType uhsType = CGVT_DOUBLE;
	void* uhsValue = NULL;
	CGResultNode* newres = NULL;
	
	CGResultNode* lhsResult = computeCGNode(graph, uhs);
	CHECK_RESULT(lhsResult)
	uhsType = lhsResult->type;
	uhsValue = lhsResult->value;
	
	switch(type){
		case CGUOT_EXP:{
			if(uhsType == CGVT_DOUBLE){
				newres = expD((CGDouble*)uhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			if(uhsType == CGVT_VECTOR){
				newres = expV((CGVector*)uhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			if(uhsType == CGVT_MATRIX){
				newres = expM((CGMatrix*)uhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			break;
		}
		
		case CGUOT_LOG:{
			if(uhsType == CGVT_DOUBLE){
				newres = logD((CGDouble*)uhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			if(uhsType == CGVT_VECTOR){
				newres = logV((CGVector*)uhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			if(uhsType == CGVT_MATRIX){
				newres = logM((CGMatrix*)uhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			break;
		}
	
		case CGUOT_MINUS:{
			if(uhsType == CGVT_DOUBLE){
				CGDouble* rhs = calloc(1, sizeof(CGDouble));
				rhs->value = -1;
				
				CGResultNode* res = mulDD((CGDouble*)uhsValue, rhs, graph, parentNode);
				
				freeDoubleValue(&rhs);
				
				parentNode->result = res;
				return res;
			}
			
			if(uhsType == CGVT_VECTOR){
				CGDouble* lhs = calloc(1, sizeof(CGDouble));
				lhs->value = -1;
				
				CGResultNode* res = mulDV(lhs, (CGVector*)uhsValue, graph, parentNode);
			
				free(lhs);
				parentNode->result = res;
				return res;
			}
			
			if(uhsType == CGVT_MATRIX){
				CGDouble* lhs = calloc(1, sizeof(CGDouble));
				lhs->value = -1;
				CGResultNode* res = mulDM(lhs, (CGMatrix*)uhsValue, graph, parentNode);
				
				freeDoubleValue(&lhs);
				
				parentNode->result = res;
				return res;
			}
			break;
		}
		
		case CGUOT_SIN:{
			if(uhsType == CGVT_DOUBLE){
				newres = sinD((CGDouble*)uhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			if(uhsType == CGVT_VECTOR){
				newres = sinV((CGVector*)uhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			if(uhsType == CGVT_MATRIX){
				newres = sinM((CGMatrix*)uhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			break;
		}
		
		case CGUOT_COS:{
			if(uhsType == CGVT_DOUBLE){
				newres = cosD((CGDouble*)uhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			if(uhsType == CGVT_VECTOR){
				newres = cosV((CGVector*)uhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			if(uhsType == CGVT_MATRIX){
				newres = cosM((CGMatrix*)uhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			break;
		}
		
		case CGUOT_TAN:{
			if(uhsType == CGVT_DOUBLE){
				newres = tanD((CGDouble*)uhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			if(uhsType == CGVT_VECTOR){
				newres = tanV((CGVector*)uhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			if(uhsType == CGVT_MATRIX){
				newres = tanM((CGMatrix*)uhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			break;
		}
		
		case CGUOT_TANH:{
			if(uhsType == CGVT_DOUBLE){
				newres = tanhD((CGDouble*)uhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			if(uhsType == CGVT_VECTOR){
				newres = tanhV((CGVector*)uhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			if(uhsType == CGVT_MATRIX){
				newres = tanhM((CGMatrix*)uhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			break;
		}
		
		case CGUOT_INV:{
			char msg[MAX_ERR_FMT_LEN];
			snprintf(msg, MAX_ERR_FMT_LEN, "Operation `%s` is not implemented/supported", getUnaryOperationTypeString(type));
			newres = returnResultError(graph, CGET_OPERATION_NOT_IMPLEMENTED, parentNode, msg);
		}
			
		case CGUOT_TRANSPOSE:{
			
			if(uhsType == CGVT_DOUBLE){
				newres = transposeD((CGDouble*)uhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			if(uhsType == CGVT_VECTOR){
				newres = transposeV((CGVector*)uhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			
			if(uhsType == CGVT_MATRIX){
				newres = transposeM((CGMatrix*)uhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			break;
		}
	
		case CGUOT_RELU:{
			newres = relu(lhsResult);
			parentNode->result = newres;
			return newres;
		}
	}
	char msg[MAX_ERR_FMT_LEN];
	snprintf(msg, MAX_ERR_FMT_LEN, "Operation [%s %s] cannot be applied", getVariableTypeString(uhsType), getUnaryOperationTypeString(type));
	newres = returnResultError(graph, CGET_INCOMPATIBLE_ARGS_EXCEPTION, parentNode, msg);
	return newres;
}

CGResultNode* processBinaryOperation(CGraph* graph, CGBinaryOperationType type, CGNode* lhs, CGNode* rhs, CGNode* parentNode){
	CGVarType lhsType = CGVT_DOUBLE;
	CGVarType rhsType = CGVT_DOUBLE;
	CGResultNode* newres = NULL;
	void* lhsValue = NULL;
	void* rhsValue = NULL;
	
	CGResultNode* lhsResult = computeCGNode(graph, lhs);
	CHECK_RESULT(lhsResult)
	lhsType = lhsResult->type;
	lhsValue = lhsResult->value;
		
	CGResultNode* rhsResult = computeCGNode(graph, rhs);
	CHECK_RESULT(rhsResult)
	rhsType = rhsResult->type;
	rhsValue = rhsResult->value;
	
	switch(type){
		case CGBOT_ADD:{
			if((lhsType == CGVT_DOUBLE) && (rhsType == CGVT_DOUBLE)){
				newres = addDD((CGDouble*)lhsValue, (CGDouble*)rhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			if((lhsType == CGVT_VECTOR) && (rhsType == CGVT_DOUBLE)){
				newres = addVD((CGVector*)lhsValue, (CGDouble*)rhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			
			if((lhsType == CGVT_DOUBLE) && (rhsType == CGVT_VECTOR)){
				newres = addVD((CGVector*)rhsValue, (CGDouble*)lhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			if((lhsType == CGVT_MATRIX) && (rhsType == CGVT_DOUBLE)){
				newres = addMD((CGMatrix*)lhsValue, (CGDouble*)rhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			if((lhsType == CGVT_DOUBLE) && (rhsType == CGVT_MATRIX)){
				newres = addMD((CGMatrix*)rhsValue, (CGDouble*)lhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			if((lhsType == CGVT_VECTOR) && (rhsType == CGVT_VECTOR)){
				newres = addVV((CGVector*)lhsValue, (CGVector*)rhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			if((lhsType == CGVT_MATRIX) && (rhsType == CGVT_VECTOR)){
				newres = addMV((CGMatrix*)lhsValue, (CGVector*)rhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			if((lhsType == CGVT_VECTOR) && (rhsType == CGVT_MATRIX)){
				newres = addMV((CGMatrix*)rhsValue, (CGVector*)lhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			if((lhsType == CGVT_MATRIX) && (rhsType == CGVT_MATRIX)){
				newres = addMM((CGMatrix*)lhsValue, (CGMatrix*)rhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			break;
		}
		
		case CGBOT_SUB:{
			if((lhsType == CGVT_DOUBLE) && (rhsType == CGVT_DOUBLE)){
				newres = subDD((CGDouble*)lhsValue, (CGDouble*)rhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			if((lhsType == CGVT_VECTOR) && (rhsType == CGVT_DOUBLE)){
				newres = subVD((CGVector*)lhsValue, (CGDouble*)rhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			if((lhsType == CGVT_DOUBLE) && (rhsType == CGVT_VECTOR)){
				newres = subDV((CGDouble*)lhsValue, (CGVector*)rhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			if((lhsType == CGVT_MATRIX) && (rhsType == CGVT_DOUBLE)){
				newres = subMD((CGMatrix*)lhsValue, (CGDouble*)rhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			if((lhsType == CGVT_DOUBLE) && (rhsType == CGVT_MATRIX)){
				newres = subDM((CGDouble*)lhsValue, (CGMatrix*)rhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			if((lhsType == CGVT_VECTOR) && (rhsType == CGVT_VECTOR)){
				newres = subVV((CGVector*)lhsValue, (CGVector*)rhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			if((lhsType == CGVT_MATRIX) && (rhsType == CGVT_MATRIX)){
				newres = subMM((CGMatrix*)lhsValue, (CGMatrix*)rhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			if((lhsType == CGVT_MATRIX) && (rhsType == CGVT_VECTOR)){
				newres = subMV((CGMatrix*)lhsValue, (CGVector*)rhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			if((lhsType == CGVT_VECTOR) && (rhsType == CGVT_MATRIX)){
				newres = subVM((CGVector*)lhsValue, (CGMatrix*)rhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			break;
		}
		
		case CGBOT_DIV:{
			if((lhsType == CGVT_DOUBLE) && (rhsType == CGVT_DOUBLE)){
				newres = divDD((CGDouble*)lhsValue, (CGDouble*)rhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			if((lhsType == CGVT_VECTOR) && (rhsType == CGVT_DOUBLE)){
				newres = divVD((CGVector*)lhsValue, (CGDouble*)rhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			if((lhsType == CGVT_DOUBLE) && (rhsType == CGVT_VECTOR)){
				newres = divDV((CGDouble*)lhsValue, (CGVector*)rhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			if((lhsType == CGVT_VECTOR) && (rhsType == CGVT_VECTOR)){
				newres = divVV((CGVector*)lhsValue, (CGVector*)rhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			if((lhsType == CGVT_MATRIX) && (rhsType == CGVT_DOUBLE)){
				newres = divMD((CGMatrix*)lhsValue, (CGDouble*)rhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			if((lhsType == CGVT_MATRIX) && (rhsType == CGVT_VECTOR)){
				newres = divMV((CGMatrix*)lhsValue, (CGVector*)rhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			if((lhsType == CGVT_DOUBLE) && (rhsType == CGVT_MATRIX)){
				newres = divDM((CGDouble*)lhsValue, (CGMatrix*)rhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			break;
		}
		
		case CGBOT_MULT:{
			// TODO: update
			if((lhsType == CGVT_MATRIX) && (rhsType == CGVT_MATRIX)){
				newres = mulMM((CGMatrix*)lhsValue, (CGMatrix*)rhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			// TODO: update
			if((lhsType == CGVT_MATRIX) && (rhsType == CGVT_VECTOR)){
				newres = mulMV((CGMatrix*)lhsValue, (CGVector*)rhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			// TODO: update
			if((lhsType == CGVT_VECTOR) && (rhsType == CGVT_MATRIX)){
				newres = mulMV((CGMatrix*)rhsValue, (CGVector*)lhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			if((lhsType == CGVT_DOUBLE) && (rhsType == CGVT_DOUBLE)){
				newres = mulDD((CGDouble*)lhsValue, (CGDouble*)rhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			if((lhsType == CGVT_DOUBLE) && (rhsType == CGVT_VECTOR)){
				newres = mulDV((CGDouble*)lhsValue, (CGVector*)rhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			if((lhsType == CGVT_VECTOR) && (rhsType == CGVT_DOUBLE)){
				newres = mulDV((CGDouble*)rhsValue, (CGVector*)lhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			if((lhsType == CGVT_DOUBLE) && (rhsType == CGVT_MATRIX)){
				newres = mulDM((CGDouble*)lhsValue, (CGMatrix*)rhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			if((lhsType == CGVT_MATRIX) && (rhsType == CGVT_DOUBLE)){
				newres = mulDM((CGDouble*)rhsValue, (CGMatrix*)lhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			if((lhsType == CGVT_VECTOR) && (rhsType == CGVT_VECTOR)){
				newres = crossVV((CGVector*)rhsValue, (CGVector*)lhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			break;
		}
		
		case CGBOT_POW:{
			if((lhsType == CGVT_DOUBLE) && (rhsType == CGVT_DOUBLE)){
				newres = powDD((CGDouble*)lhsValue, (CGDouble*)rhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			if((lhsType == CGVT_VECTOR) && (rhsType == CGVT_DOUBLE)){
				newres = powVD((CGVector*)lhsValue, (CGDouble*)rhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			if((lhsType == CGVT_MATRIX) && (rhsType == CGVT_DOUBLE)){
				newres = powMD((CGMatrix*)lhsValue, (CGDouble*)rhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			break;
		}
		
		case CGBOT_DOT: {
			/*
			 * The following are the same as MUL
			 */
			if((lhsType == CGVT_DOUBLE) && (rhsType == CGVT_DOUBLE)){
				newres = mulDD((CGDouble*)lhsValue, (CGDouble*)rhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			if((lhsType == CGVT_DOUBLE) && (rhsType == CGVT_VECTOR)){
				newres = mulDV((CGDouble*)lhsValue, (CGVector*)rhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			if((lhsType == CGVT_VECTOR) && (rhsType == CGVT_DOUBLE)){
				newres = mulDV((CGDouble*)rhsValue, (CGVector*)lhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			if((lhsType == CGVT_DOUBLE) && (rhsType == CGVT_MATRIX)){
				newres = mulDM((CGDouble*)lhsValue, (CGMatrix*)rhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			if((lhsType == CGVT_MATRIX) && (rhsType == CGVT_DOUBLE)){
				newres = mulDM((CGDouble*)rhsValue, (CGMatrix*)lhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			/* here starts dot specific impl */
			
			
			if((lhsType == CGVT_MATRIX) && (rhsType == CGVT_MATRIX)){
				newres = dotMM((CGMatrix*)lhsValue, (CGMatrix*)rhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			if((lhsType == CGVT_MATRIX) && (rhsType == CGVT_VECTOR)){
				newres = dotMV((CGMatrix*)lhsValue, (CGVector*)rhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			if((lhsType == CGVT_VECTOR) && (rhsType == CGVT_MATRIX)){
				newres = dotVM((CGVector*)lhsValue, (CGMatrix*)rhsValue, graph, parentNode);
				//newres = dotMM(vectorToMatrix((CGVector*)lhsValue), (CGMatrix*)rhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			
			/*
			if((lhsType == CGVT_VECTOR) && (rhsType == CGVT_MATRIX)){
				newres = mulMV((CGMatrix*)rhsValue, (CGVector*)lhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			*/
			
			if((lhsType == CGVT_VECTOR) && (rhsType == CGVT_VECTOR)){
				newres = dotVV((CGVector*)lhsValue, (CGVector*)rhsValue, graph, parentNode);
				parentNode->result = newres;
				return newres;
			}
			break;
		}
		
		case CGBOT_TMULT:{
			char msg[MAX_ERR_FMT_LEN];
			snprintf(msg, MAX_ERR_FMT_LEN, "Operation `%s` is not implemented/supported", getBinaryOperationTypeString(type));
			return returnResultError(graph, CGET_OPERATION_NOT_IMPLEMENTED, parentNode, msg);
		}
	}
	
	char msg[MAX_ERR_FMT_LEN];
	snprintf(msg, MAX_ERR_FMT_LEN, "Operation [%s %s %s] cannot be applied", getVariableTypeString(lhsType), getBinaryOperationTypeString(type), getVariableTypeString(rhsType));
	return returnResultError(graph, CGET_INCOMPATIBLE_ARGS_EXCEPTION, parentNode, msg);
}

CGResultNode* computeRawNode(CGNode* node){
	return computeCGNode(NULL, node);
}


CGResultNode* computeCGNode(CGraph* graph, CGNode* node){
	CGResultNode* result = NULL;
	
	
	if(node->result != NULL){
		return node->result;
	}
	
	switch(node->type){
		case CGNT_CONSTANT:{
			result = constantNodeToResultNodeCopy(node);
			break;
		}

		case CGNT_VARIABLE:{
			if(graph == NULL)
			{
				char msg[MAX_ERR_FMT_LEN];
				snprintf(msg, MAX_ERR_FMT_LEN, "Cannot compute variable`%s` without the graph instance", node->var->name);
				return returnResultError(graph, CGET_NO_GRAPH_INSTANCE, node, msg);
			}
			
			
			CGNode* constantNode = *map_get(&graph->vars, node->var->name);
			if(constantNode == NULL)
			{
				char msg[MAX_ERR_FMT_LEN];
				snprintf(msg, MAX_ERR_FMT_LEN, "No variable `%s` was found in graph `%s`", node->var->name, graph!=NULL?graph->name:"[anonymous]");
				return returnResultError(graph, CGET_VARIABLE_DOES_NOT_EXIST, node, msg);
			}
			
			CGResultNode* rnode = computeCGNode(graph, constantNode);
			CHECK_RESULT(rnode)
			constantNode->result = rnode;
			node->result = copyResultNode(rnode);
			
			result = node->result;
			break;
		}
		case CGNT_BINARY_OPERATION:
			result = processBinaryOperation(graph, node->bop->type, node->bop->lhs, node->bop->rhs, node);
			break;
		case CGNT_UNARY_OPERATION:
			result = processUnaryOperation(graph, node->uop->type, node->uop->uhs, node);
			break;
		
		/* 
		 * TODO: add this test to unittest
		 */
		case CGNT_AXIS_BOUND_OPERATION:
		{
			switch(node->axop->type){
				case CGABOT_SUM:{
					CGResultNode* newres = computeCGNode(graph, node->axop->uhs);
					CHECK_RESULT(newres)
					node->axop->uhs->result = newres;
					
					if(newres->type == CGVT_DOUBLE){
						result = sumD((CGDouble*)newres->value, graph, node);
					}
					
					if(newres->type == CGVT_VECTOR){
						result = sumV((CGVector*)newres->value, graph, node);
					}
					
					if(newres->type == CGVT_MATRIX){
						result = sumM((CGMatrix*)newres->value, graph, node, node->axop->axis);
					}
				
					break;
				}
				
				case CGABOT_MAX:{
					result = max(node, graph);
					break;
				}
				
				case CGABOT_MIN:{
					result = min(node, graph);
					break;
				}
				
				case CGABOT_MEAN:{
					result = mean(node, graph);
					break;
				}

				case CGABOT_SOFTMAX:{
					char msg[MAX_ERR_FMT_LEN];
					snprintf(msg, MAX_ERR_FMT_LEN, "Operation [CGABOT_SOFTMAX] is not implemented/supported");
					return returnResultError(graph, CGET_OPERATION_NOT_IMPLEMENTED, node, msg);
					//CGResultNode* res = computeCGNode(graph, node->axop->uhs);
					//break;
				}
			}
			
			break;
		}
		case CGNT_GRAPH:{
			result = computeGraph(node->graph);
			break;
			/*
			char msg[MAX_ERR_FMT_LEN];
			snprintf(msg, MAX_ERR_FMT_LEN, "Operation [GRAPH] is not implemented/supported");
			return returnResultError(graph, CGET_OPERATION_NOT_IMPLEMENTED, node, msg);
			*/
		}
		
		case CGNT_CROSS_ENTROPY_LOSS_FUNC:
		{
			result = crossEntropy(graph, node->crossEntropyLoss->x, node->crossEntropyLoss->y, node->crossEntropyLoss->num_classes);
			break;
		}
	}
	
	node->result = reduceDim(result);
	
	switch(result->type){
		case CGVT_DOUBLE:
			node->diff = makeZeroDoubleConstantNode();
			node->diff->diff = NULL;
			break;
		case CGVT_VECTOR:{
			CGVector* v = (CGVector*)result->value;
			node->diff = makeZeroVectorConstantNode(v->len);
			break;
		}
		
		case CGVT_MATRIX:{
			CGMatrix* v = (CGMatrix*)result->value;
			node->diff = makeZeroMatrixConstantNode(v->rows, v->cols);
			break;
		}
	}
	
	return node->result;
}

CGResultNode* reduceDim(CGResultNode* result){
	switch(result->type){
		case CGVT_DOUBLE:{
			return result;
		}
		
		case CGVT_VECTOR:{
			CGVector* vec = (CGVector*)result->value;
			if (vec->len > 1)
				return result;
			
			CGDouble* d = calloc(1, sizeof(CGDouble));
			d->value = vec->data[0];
			
			freeVectorValue(result->value);
			free(result->value);
			
			result->type = CGVT_DOUBLE;
			result->value = d;
			
			return result;
		}
		
		case CGVT_MATRIX:{
			CGMatrix* mat = (CGMatrix*)result->value;
			
			if((mat->rows>1) &&(mat->cols>1))
				return result;
			
			if((mat->rows == 1) && (mat->cols == 1)){
				
				CGDouble* d = calloc(1, sizeof(CGDouble));
				d->value = mat->data[0];
				
				
				freeMatrixValue(result->value);
				free(result->value);
				
				result->type = CGVT_DOUBLE;
				result->value = d;
				
				return result;
			}
			
			if(mat->rows == 1){
				
				CGVector* vec = calloc(1, sizeof(CGVector));
				vec->len = mat->cols;
				vec->data = mat->data;
				
				//freeMatrixValue(result->value);
				free(result->value);
				
				result->type = CGVT_VECTOR;
				result->value = vec;
				
				return result;
			}
			
			return result;
		}
	}
}

CGResultNode* computeGraph(CGraph* graph){
	return computeCGNode(graph, graph->root);
}

void storeNodesInGraph(CGraph* graph, CGNode* node){
	int idx = -1;
	
	vec_find(&graph->nodes, node, idx);
	
	if(idx == -1){
		vec_push(&graph->nodes, node);
	}
	
	switch(node->type){
		case CGNT_CONSTANT:
			break;
		case CGNT_VARIABLE:
			break;
		case CGNT_BINARY_OPERATION:
			storeNodesInGraph(graph, node->bop->lhs);
			storeNodesInGraph(graph, node->bop->rhs);
			break;
		case CGNT_UNARY_OPERATION:
			storeNodesInGraph(graph, node->uop->uhs);
			break;
		case CGNT_AXIS_BOUND_OPERATION:
			storeNodesInGraph(graph, node->axop->uhs);
			break;
		case CGNT_GRAPH:
			storeNodesInGraph(graph, node->graph->root);
			break;
		case CGNT_CROSS_ENTROPY_LOSS_FUNC:
			storeNodesInGraph(graph, node->crossEntropyLoss->x);
			storeNodesInGraph(graph, node->crossEntropyLoss->y);
			break;
	}
	
}

