
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




#if CG_SCALAR_TYPE == float
#define cblas_dcopy cblas_scopy
#define cblas_dscal cblas_sscal
#define cblas_dgemm cblas_sgemm
#define cblas_ddot cblas_sdot
#define cblas_dgemv cblas_sgemv
#endif

int selectContext() {
    return 0;
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
    CG_SCALAR_TYPE* y = calloc(V->len, sizeof(CG_SCALAR_TYPE));
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
    CG_SCALAR_TYPE value = a->value;

    CG_SCALAR_TYPE* y = calloc(size, sizeof(CG_SCALAR_TYPE));
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
    CG_SCALAR_TYPE* res = calloc(size, sizeof(CG_SCALAR_TYPE));

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
	CG_SCALAR_TYPE* res = calloc(size, sizeof(CG_SCALAR_TYPE));

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
    CG_SCALAR_TYPE* res = calloc(size, sizeof(CG_SCALAR_TYPE));

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

    CG_SCALAR_TYPE* y = calloc(M->rows*V->len, sizeof(CG_SCALAR_TYPE));
    CGMatrix* Y = calloc(1, sizeof(CGMatrix));
    Y->rows= M->rows;
    Y->cols  = V->len;
    Y->data = y;

    /*
    cblas_dgemv(CblasRowMajor,
            CblasNoTrans, M->rows, M->cols, 1.0, M->data,
            M->cols, V->data, 1, 0.0, Y->data, 1);
    */


    cblas_dgemm(CblasRowMajor,
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

    CG_SCALAR_TYPE* y = calloc(M->cols, sizeof(CG_SCALAR_TYPE));
    CGVector* Y = calloc(1, sizeof(CGVector));
    Y->len = M->cols;
    Y->data = y;

    int m = 1;
    int n = M->cols;
    int k = V->len;

    cblas_dgemm(CblasRowMajor,
                CblasNoTrans, CblasNoTrans, m, n, k, 1, V->data, k, M->data, n, 0, Y->data, n);


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

    CG_SCALAR_TYPE* z = calloc(M->rows*N->cols, sizeof(CG_SCALAR_TYPE));

    CGMatrix* Z = calloc(1, sizeof(CGMatrix));
    Z->rows = M->rows;
    Z->cols = N->cols;

    cblas_dgemm(CblasRowMajor,
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
    CG_SCALAR_TYPE* res = calloc(size, sizeof(CG_SCALAR_TYPE));

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
    CG_SCALAR_TYPE res = 0;

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

    CG_SCALAR_TYPE res = D1->value / D2->value;

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

    CG_SCALAR_TYPE* res = calloc(V->len, sizeof(CG_SCALAR_TYPE));
    CG_SCALAR_TYPE value = D->value;

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

    CG_SCALAR_TYPE* res = calloc(V->len, sizeof(CG_SCALAR_TYPE));

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
    CG_SCALAR_TYPE* res = calloc(V->len, sizeof(CG_SCALAR_TYPE));
    CG_SCALAR_TYPE value = D->value;

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
    CG_SCALAR_TYPE* res = calloc(size, sizeof(CG_SCALAR_TYPE));
    CG_SCALAR_TYPE value = D->value;

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
    if(M->rows != V->len){
        char msg[MAX_ERR_FMT_LEN];
        snprintf(msg, MAX_ERR_FMT_LEN, "Cannot calculate  M(%"PRIu64", %"PRIu64") DIV V(%"PRIu64")", M->rows, M->cols, V->len);
        return returnResultError(graph, CGET_INCOMPATIBLE_DIMENTIONS_EXCEPTION, parentNode, msg);
    }

    uint64_t size = M->rows*M->cols;
    CG_SCALAR_TYPE* res = calloc(size, sizeof(CG_SCALAR_TYPE));

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
    CG_SCALAR_TYPE* res = calloc(size, sizeof(CG_SCALAR_TYPE));
    CG_SCALAR_TYPE value = D->value;

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
    CG_SCALAR_TYPE res = D1->value + D2->value;

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
    CG_SCALAR_TYPE* res = calloc(V->len, sizeof(CG_SCALAR_TYPE));
    CG_SCALAR_TYPE value = D->value;

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
    CG_SCALAR_TYPE* res = calloc(size, sizeof(CG_SCALAR_TYPE));
    CG_SCALAR_TYPE value = D->value;

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
    CG_SCALAR_TYPE* res = calloc(size, sizeof(CG_SCALAR_TYPE));

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
    CG_SCALAR_TYPE* res = calloc(size, sizeof(CG_SCALAR_TYPE));

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
    CG_SCALAR_TYPE* res = calloc(size, sizeof(CG_SCALAR_TYPE));

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
    CG_SCALAR_TYPE res = D1->value - D2->value;

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
    CG_SCALAR_TYPE* res = calloc(V->len, sizeof(CG_SCALAR_TYPE));
    CG_SCALAR_TYPE value = D->value;

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
    CG_SCALAR_TYPE* res = calloc(V->len, sizeof(CG_SCALAR_TYPE));
    CG_SCALAR_TYPE value = D->value;

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
    CG_SCALAR_TYPE* res = calloc(size, sizeof(CG_SCALAR_TYPE));
    CG_SCALAR_TYPE value = D->value;

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
    CG_SCALAR_TYPE* res = calloc(size, sizeof(CG_SCALAR_TYPE));
    CG_SCALAR_TYPE value = D->value;

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
    CG_SCALAR_TYPE* res = calloc(size, sizeof(CG_SCALAR_TYPE));

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
    CG_SCALAR_TYPE* res = calloc(size, sizeof(CG_SCALAR_TYPE));

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
    CG_SCALAR_TYPE* res = calloc(size, sizeof(CG_SCALAR_TYPE));

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
    CG_SCALAR_TYPE* res = calloc(size, sizeof(CG_SCALAR_TYPE));

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
    CG_SCALAR_TYPE res = pow(D1->value, D2->value);

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
    CG_SCALAR_TYPE* res = calloc(V->len, sizeof(CG_SCALAR_TYPE));
    CG_SCALAR_TYPE value = D->value;

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
    CG_SCALAR_TYPE* res = calloc(size, sizeof(CG_SCALAR_TYPE));
    CG_SCALAR_TYPE value = D->value;

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
    CG_SCALAR_TYPE* y = calloc(V->len, sizeof(CG_SCALAR_TYPE));
    CGVector* Y = calloc(1, sizeof(CGVector));
    Y->len = V->len;
    Y->data = y;

    cblas_dgemv(CblasRowMajor,
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
    CG_SCALAR_TYPE y = exp(D->value);
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
    CG_SCALAR_TYPE* y = calloc(V->len, sizeof(CG_SCALAR_TYPE));
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

    CG_SCALAR_TYPE* y = calloc(size, sizeof(CG_SCALAR_TYPE));
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
    CG_SCALAR_TYPE y = log(D->value);
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
    CG_SCALAR_TYPE* y = calloc(V->len, sizeof(CG_SCALAR_TYPE));
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

    CG_SCALAR_TYPE* y = calloc(size, sizeof(CG_SCALAR_TYPE));
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
    CG_SCALAR_TYPE y = sin(D->value);
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
    CG_SCALAR_TYPE* y = calloc(V->len, sizeof(CG_SCALAR_TYPE));
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

    CG_SCALAR_TYPE* y = calloc(size, sizeof(CG_SCALAR_TYPE));
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
    CG_SCALAR_TYPE y = cos(D->value);
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
    CG_SCALAR_TYPE* y = calloc(V->len, sizeof(CG_SCALAR_TYPE));
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

    CG_SCALAR_TYPE* y = calloc(size, sizeof(CG_SCALAR_TYPE));
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
    CG_SCALAR_TYPE y = tan(D->value);
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
    CG_SCALAR_TYPE* y = calloc(V->len, sizeof(CG_SCALAR_TYPE));
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

    CG_SCALAR_TYPE* y = calloc(size, sizeof(CG_SCALAR_TYPE));
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
    CG_SCALAR_TYPE y = tanh(D->value);
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
    CG_SCALAR_TYPE* y = calloc(V->len, sizeof(CG_SCALAR_TYPE));
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

    CG_SCALAR_TYPE* y = calloc(size, sizeof(CG_SCALAR_TYPE));
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

    CG_SCALAR_TYPE* y = calloc(size, sizeof(CG_SCALAR_TYPE));
    CGMatrix* Y = calloc(1, sizeof(CGMatrix));
    Y->data = y;
    Y->rows = V->len;
    Y->cols = 1;
    Y->data = y;

    uint64_t i = 0;
    uint64_t j = 0;

    memcpy(Y->data, V->data, V->len*sizeof(CG_SCALAR_TYPE));

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

    CG_SCALAR_TYPE* y = calloc(size, sizeof(CG_SCALAR_TYPE));
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
    CG_SCALAR_TYPE y = D->value;
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
    CG_SCALAR_TYPE y = 0;
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
        CG_SCALAR_TYPE* y = calloc(len, sizeof(CG_SCALAR_TYPE));

        V->data = y;
        V->len = len;

        uint64_t i = 0;

        for(;i<M->rows*M->cols;i++){
            y[i%len] += M->data[i];
        }
    }
    else {
        uint64_t len = M->rows;
        CG_SCALAR_TYPE* y = calloc(len, sizeof(CG_SCALAR_TYPE));

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
            CG_SCALAR_TYPE y = 0;
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
            CG_SCALAR_TYPE m = v->data[0];

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
                CG_SCALAR_TYPE* y = calloc(len, sizeof(CG_SCALAR_TYPE));

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
                CG_SCALAR_TYPE* y = calloc(len, sizeof(CG_SCALAR_TYPE));

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
            CG_SCALAR_TYPE y = 0;
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
            CG_SCALAR_TYPE m = v->data[0];

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
                CG_SCALAR_TYPE* y = calloc(len, sizeof(CG_SCALAR_TYPE));

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
                CG_SCALAR_TYPE* y = calloc(len, sizeof(CG_SCALAR_TYPE));

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


CGResultNode* argmax(CGNode* X, CGraph* graph){
    CGResultNode* res = computeCGNode(graph, X->axop->uhs);

    switch(res->type){
        case CGVT_DOUBLE:
        {
            CG_SCALAR_TYPE y = 0;
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
            uint64_t maxIdx = 0;

            for (;i < v->len; i++){
                if(v->data[i] > v->data[maxIdx])
                    maxIdx = i;
            }

            Y->value = maxIdx;
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
                CG_SCALAR_TYPE* y = calloc(len, sizeof(CG_SCALAR_TYPE));

                V->data = y;
                V->len = len;

                uint64_t i = 0;

                for(;i<M->cols;i++){
                    y[i] = i;
                }

                for(i=0;i<M->rows*M->cols;i++){
                    if(M->data[(uint64_t)y[i%len]] < M->data[i])
                        y[i%len] = i;
                }

                // transform vector representation into matrix
                for(i=0;i<M->cols;i++){
                    y[i] = ((int)y[i])/M->cols;
                }
            }
            else {
                uint64_t len = M->rows;
                CG_SCALAR_TYPE* y = calloc(len, sizeof(CG_SCALAR_TYPE));

                V->data = y;
                V->len = len;

                uint64_t i = 0;

                for(;i<M->rows;i++){
                    y[i] = i*M->cols;
                }

                for(i=0;i<M->rows*M->cols;i++){
                    if(M->data[(uint64_t)y[i/M->cols]] < M->data[i])
                        y[i/M->cols] = i;
                }

                // transform vector representation into matrix
                for(i=0;i<M->rows;i++){
                    y[i] = ((int)y[i])%M->cols;
                }
            }

            CGResultNode* result = calloc(1, sizeof(CGResultNode));
            result->type = CGVT_VECTOR;
            result->value = V;

            return result;

        }
    }
}


CGResultNode* argmin(CGNode* X, CGraph* graph){
    CGResultNode* res = computeCGNode(graph, X->axop->uhs);

    switch(res->type){
        case CGVT_DOUBLE:
        {
            CG_SCALAR_TYPE y = 0;
            CGDouble* Y = calloc(1, sizeof(CGDouble));

            Y->value = 0;
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
            uint64_t minIdx = 0;

            for (;i < v->len; i++){
                if(v->data[i] > v->data[minIdx])
                    minIdx = (CG_SCALAR_TYPE)i;
            }

            Y->value = minIdx;
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
                CG_SCALAR_TYPE* y = calloc(len, sizeof(CG_SCALAR_TYPE));

                V->data = y;
                V->len = len;

                uint64_t i = 0;

                for(;i<M->cols;i++){
                    y[i] = i;
                }

                for(i=0;i<M->rows*M->cols;i++){
                    if(M->data[(uint64_t)y[i%len]] > M->data[i])
                        y[i%len] = (CG_SCALAR_TYPE)i;
                }

                // transform vector representation into matrix
                for(i=0;i<M->cols;i++){
                    y[i] = ((int)y[i])/M->cols;
                }
            }
            else {
                uint64_t len = M->rows;
                CG_SCALAR_TYPE* y = calloc(len, sizeof(CG_SCALAR_TYPE));

                V->data = y;
                V->len = len;

                uint64_t i = 0;

                for(;i<M->rows;i++){
                    y[i] = i*M->cols;
                }

                for(i=0;i<M->rows*M->cols;i++){
                    if(M->data[(uint64_t)y[i/M->cols]] > M->data[i])
                        y[i/M->cols] = (CG_SCALAR_TYPE)i;
                }


                // transform vector representation into matrix
                for(i=0;i<M->rows;i++){
                    y[i] = ((int)y[i])%M->cols;
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
            CG_SCALAR_TYPE y = 0;
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
            CG_SCALAR_TYPE m = v->data[0];

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
                CG_SCALAR_TYPE* y = calloc(len, sizeof(CG_SCALAR_TYPE));

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
                CG_SCALAR_TYPE* y = calloc(len, sizeof(CG_SCALAR_TYPE));

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


#if CG_SCALAR_TYPE == float
#undef cblas_dcopy
#undef cblas_dscal
#undef cblas_dgemm
#undef cblas_ddot
#undef cblas_dgemv
#endif