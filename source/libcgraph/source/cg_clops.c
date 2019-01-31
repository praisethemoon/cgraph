
#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <cblas.h>
#include <string.h> // memcpy
#include <math.h>
#include <clblast_c.h>

#include "cgraph.h"
#include "cg_operation.h"
#include "cg_types.h"
#include "cg_variables.h"
#include "cg_errors.h"
#include "cg_constants.h"
#include "cg_enums.h"
#include "cg_factory.h"
#include "cg_math.h"


//#pragma OPENCL EXTENSION cl_khr_fp64 : enable

#if CG_SCALAR_TYPE == float
#define cblas_dcopy cblas_scopy
#define cblas_dscal cblas_sscal
#define cblas_dgemm cblas_sgemm
#define cblas_ddot cblas_sdot
#define cblas_dgemv cblas_sgemv
#define cl_double cl_float
#endif

#define CHECK_ERROR(err) \
    if (err != NULL) { fprintf(stderr, "\n%s\n", err->message); exit(-1); }

CCLContext * ctx = NULL;
CCLProgram* prg = NULL;
CCLQueue* queue = NULL;
CCLDevice* dev = NULL;


#if CG_SCALAR_TYPE == float
#define CLBlastDcopy CLBlastScopy
#define CLBlastDscal CLBlastSscal
#define CLBlastDgemm CLBlastSgemm
#define CLBlastDdot CLBlastSdot
#define CLBlastDomatcopy CLBlastSomatcopy
#endif



void copyDataToHost(CGResultNode* res){
    CCLEvent* evt = NULL;
    CCLErr * err = NULL;
    

    switch(res->type){
        case CGVT_VECTOR:{
            CGVector* V = (CGVector*)res->value;
            V->data = malloc(V->len* sizeof(CG_CL_SCALAR_TYPE));
            if (V->loc == CG_DATALOC_DEVICE_MEM) {
                evt = ccl_buffer_enqueue_read(V->buf, queue, CL_TRUE, 0,
                                              V->len * sizeof(CG_CL_SCALAR_TYPE), V->data, NULL, NULL);
                if (!evt) exit(-1);
            }
            else
                printf("FUCK\n");
            V->loc = CG_DATALOC_HOST_MEM;
            break;
        }

        case CGVT_MATRIX:{
            CGMatrix* M = (CGMatrix*)res->value;
            M->data = malloc(M->rows*M->cols* sizeof(CG_CL_SCALAR_TYPE));

            if (M->loc == CG_DATALOC_DEVICE_MEM) {
                evt = ccl_buffer_enqueue_read(M->buf, queue, CL_TRUE, 0,
                                              M->rows * M->cols * sizeof(CG_CL_SCALAR_TYPE), M->data, NULL, NULL);
                //if (!evt) exit(-1);
            }
            else
            printf("FUCK2\n");
            M->loc = CG_DATALOC_HOST_MEM;
            break;
        }
    }

    cl_bool status = ccl_queue_finish(queue, NULL);
    //if (!status) exit(-1);
    ////ccl_queue_destroy(queue);
}

CGDouble* makeDeviceDouble(){
    CCLErr * err = NULL;
    CGDouble* Y = calloc(1, sizeof(CGDouble));

    Y->buf = ccl_buffer_new(ctx, CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR,
                            sizeof(CG_CL_SCALAR_TYPE), NULL, &err);
    CHECK_ERROR(err)

    Y->loc = CG_DATALOC_DEVICE_MEM;
    Y->value = 0.0;

    return Y;
}

CGVector* makeDeviceVector(uint64_t len){
    CCLErr * err = NULL;
    CGVector* Y = calloc(1, sizeof(CGVector));
    Y->len = len;
    Y->data = NULL; //calloc(len, sizeof(CG_SCALAR_TYPE));

    Y->buf = ccl_buffer_new(ctx, CL_MEM_READ_WRITE ,
                            len * sizeof(CG_CL_SCALAR_TYPE), Y->data, &err);

    CHECK_ERROR(err)

    Y->loc = CG_DATALOC_DEVICE_MEM;

    return Y;
}

CGMatrix* makeDeviceMatrix(uint64_t rows, uint64_t cols){
    CCLErr * err = NULL;
    CGMatrix* Y = calloc(1, sizeof(CGMatrix));
    Y->rows = rows;
    Y->cols = cols;
    Y->data = NULL; //calloc(rows*cols, sizeof(CG_SCALAR_TYPE));

    Y->buf = ccl_buffer_new(ctx, CL_MEM_READ_WRITE ,
                            rows*cols * sizeof(CG_CL_SCALAR_TYPE), Y->data, &err);
    CHECK_ERROR(err)

    Y->loc = CG_DATALOC_DEVICE_MEM;

    return Y;
}

int selectContext() {
    /* Code. */
    ctx = ccl_context_new_from_menu(NULL);
    if (ctx == NULL)
        exit(-1);

    CCLErr * err = NULL;
    prg = ccl_program_new_from_source_file(ctx, "/Users/praisethemoon/projects/cgraph/source/libcgraph/kernels/vec.cl", NULL);
    cl_bool status = ccl_program_build(prg, NULL, &err);
    printf("%s\n", ccl_program_get_build_log(prg, NULL));
    CHECK_ERROR(err)

    dev = ccl_context_get_device(ctx, 0, &err);
    queue  = ccl_queue_new(ctx, dev, 0, &err);

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
CGResultNode* mulDV(CGDouble* A, CGVector* V, CGraph* graph, CGNode* parentNode){
    CGVector* Y = makeDeviceVector(V->len);
    CCLErr * err = NULL;

    //if(A->value != 0){
        uint64_t len = V->len;
        CCLBuffer * a = V->buf, * c = NULL;

        
        CHECK_ERROR(err)

        
        CHECK_ERROR(err)


        CCLEvent* evt = NULL;
        size_t gws = len;

        cl_mem V_mem = ccl_buffer_unwrap(V->buf);
        cl_mem Y_mem = ccl_buffer_unwrap(Y->buf);

        cl_command_queue q = ccl_queue_unwrap(queue);

        //cblas_dcopy(V->len, V->data, 1, y, 1);
        //cblas_dscal(V->len, a->value, y, 1);

        CLBlastDcopy(V->len, V_mem, 0.0, 1, Y_mem, 0.0, 1, &q, NULL);
        CLBlastDscal(V->len, A->value, Y_mem, 0.0, 1, &q, NULL);
        //ccl_queue_destroy(queue);
    //}

    CGResultNode* result = calloc(1, sizeof(CGResultNode));
    result->type = CGVT_VECTOR;
    result->value = Y;

    return result;
}


/*
 * d.M == M.d
 */
CGResultNode* mulDM(CGDouble* A, CGMatrix* M, CGraph* graph, CGNode* parentNode){
    uint64_t size = M->cols*M->rows;
    CG_SCALAR_TYPE value = A->value;

    CGMatrix* Y = makeDeviceMatrix(M->rows, M->cols);

    //if(A->value != 0){
        CCLErr * err = NULL;

        CCLBuffer * a = M->buf;

        
        CHECK_ERROR(err)

        
        CHECK_ERROR(err)


        cl_mem M_mem = ccl_buffer_unwrap(M->buf);
        cl_mem Y_mem = ccl_buffer_unwrap(Y->buf);

        cl_command_queue q = ccl_queue_unwrap(queue);

        //cblas_dcopy(V->len, V->data, 1, y, 1);
        //cblas_dscal(V->len, a->value, y, 1);

        CLBlastDcopy(size, M_mem, 0.0, 1, Y_mem, 0.0, 1, &q, NULL);
        CLBlastDscal(size, A->value, Y_mem, 0.0, 1, &q, NULL);

        //ccl_queue_destroy(queue);
    //}

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
    CCLErr * err = NULL;


    CGMatrix* Y = makeDeviceMatrix(M->rows, M->cols);

    uint64_t i = 0;

    //res[i] = M->data[i] * V->data[(i)%V->len];

    uint64_t len = M->rows*M->cols;
    uint64_t vlen = V->len;

    
    CHECK_ERROR(err)

    
    CHECK_ERROR(err)

    CCLEvent* evt = NULL;
    size_t gws = len;

    evt = ccl_program_enqueue_kernel(prg, "mul_mv", queue, 1, NULL, &gws,
                                     NULL, NULL, &err, ccl_arg_priv(len, cl_uint), M->buf, V->buf, Y->buf, ccl_arg_priv(vlen, cl_ulong), NULL);
    CHECK_ERROR(err)

    //ccl_queue_destroy(queue);

    CGResultNode* result = calloc(1, sizeof(CGResultNode));
    result->type = CGVT_MATRIX;
    result->value = Y;

    return result;
}


/*
 * M*N
 * broadcasting
 */
CGResultNode* mulMM(CGMatrix* M1, CGMatrix* M2, CGraph* graph, CGNode* parentNode){
    CCLErr * err = NULL;
    if((M1->rows != M2->rows) && (M1->cols != M2->cols)){
        char msg[MAX_ERR_FMT_LEN];
        snprintf(msg, MAX_ERR_FMT_LEN, "Cannot multiply M(%"PRIu64", %"PRIu64") by N(%"PRIu64", %"PRIu64")", M1->rows, M1->cols, M2->rows, M2->cols);
        return returnResultError(graph, CGET_INCOMPATIBLE_DIMENTIONS_EXCEPTION, parentNode, msg);
    }

    uint64_t size = M1->cols*M1->rows;

    CGMatrix* Y = makeDeviceMatrix(M1->rows, M1->cols);
    uint64_t len = M1->rows*M1->cols;

    
    CHECK_ERROR(err)

    
    CHECK_ERROR(err)

    CCLEvent* evt = NULL;
    size_t gws = len;

    evt = ccl_program_enqueue_kernel(prg, "mul_vv", queue, 1, NULL, &gws,
                                     NULL, NULL, &err, ccl_arg_priv(len, cl_uint), M1->buf, M2->buf, Y->buf, NULL);
    CHECK_ERROR(err)

    //ccl_queue_destroy(queue);

    CGResultNode* result = calloc(1, sizeof(CGResultNode));
    result->type = CGVT_MATRIX;
    result->value = Y;

    return result;
}

/*
 * M.v
 */
CGResultNode* dotMV(CGMatrix* M, CGVector* V, CGraph* graph, CGNode* parentNode){
    CCLErr * err = NULL;
    if(M->cols != 1){
        char msg[MAX_ERR_FMT_LEN];
        snprintf(msg, MAX_ERR_FMT_LEN, "Cannot calculate M(%"PRIu64", %"PRIu64") DOT V(%"PRIu64")", M->rows, M->cols, V->len);
        return returnResultError(graph, CGET_INCOMPATIBLE_DIMENTIONS_EXCEPTION, parentNode, msg);
    }

    uint64_t len =  M->rows*V->len;


    CGMatrix* Y = makeDeviceMatrix(M->rows, V->len);


    
    CHECK_ERROR(err)
    
    CHECK_ERROR(err)


    CCLEvent* evt = NULL;
    cl_command_queue q = ccl_queue_unwrap(queue);
    cl_mem M_mem = ccl_buffer_unwrap(M->buf);
    cl_mem V_mem = ccl_buffer_unwrap(V->buf);
    cl_mem Y_mem = ccl_buffer_unwrap(Y->buf);

    CLBlastDgemm(CLBlastLayoutRowMajor,
                 CLBlastTransposeNo, CLBlastTransposeNo, M->rows, V->len, M->cols,
                 1.0,
                 M_mem, 0, M->cols,
                 V_mem, 0, V->len,
                 0.0,
                 Y_mem, 0, Y->cols,
                 &q, NULL
                 );


    //evt = ccl_buffer_enqueue_read(Y->buf, queue, CL_TRUE, 0,
    //                              len * sizeof(cl_double), Y->data, NULL, NULL);

    //ccl_queue_destroy(queue);

    CGResultNode* result = calloc(1, sizeof(CGResultNode));
    result->type = CGVT_MATRIX;
    result->value = Y;

    return result;
}


/*
 * v.M
 */
CGResultNode* dotVM(CGVector* V, CGMatrix* M, CGraph* graph, CGNode* parentNode){
    CCLErr * err = NULL;
    if(M->rows != V->len){
        char msg[MAX_ERR_FMT_LEN];
        snprintf(msg, MAX_ERR_FMT_LEN, "Cannot calculate V(%"PRIu64") DOT M(%"PRIu64", %"PRIu64")", V->len, M->rows, M->cols);
        return returnResultError(graph, CGET_INCOMPATIBLE_DIMENTIONS_EXCEPTION, parentNode, msg);
    }

    CGVector* Y = makeDeviceVector(M->cols);

    uint64_t m = 1;
    uint64_t n = M->cols;
    uint64_t k = V->len;

    
    CHECK_ERROR(err)
    
    CHECK_ERROR(err)


    CCLEvent* evt = NULL;
    cl_command_queue q = ccl_queue_unwrap(queue);
    cl_mem M_mem = ccl_buffer_unwrap(M->buf);
    cl_mem V_mem = ccl_buffer_unwrap(V->buf);
    cl_mem Y_mem = ccl_buffer_unwrap(Y->buf);

    //cblas_dgemm(CblasRowMajor,
    //            CblasNoTrans, CblasNoTrans, m, n, k, 1, V->data, k, M->data, n, 0, Y->data, n);

    CLBlastDgemm(CLBlastLayoutRowMajor,
                 CLBlastTransposeNo, CLBlastTransposeNo, m, n, k,
                 1.0,
                 V_mem, 0, k,
                 M_mem, 0, n,
                 0.0,
                 Y_mem, 0, n,
                 &q, NULL
    );


    //ccl_queue_destroy(queue);

    CGResultNode* result = calloc(1, sizeof(CGResultNode));
    result->type = CGVT_VECTOR;
    result->value = Y;

    return result;
}

/*
 * M.N
 */
CGResultNode* dotMM(CGMatrix* M, CGMatrix* N, CGraph* graph, CGNode* parentNode){
    CCLErr * err = NULL;
    if(M->cols != N->rows){
        char msg[MAX_ERR_FMT_LEN];
        snprintf(msg, MAX_ERR_FMT_LEN, "Cannot DOT M(%"PRIu64", %"PRIu64") by N(%"PRIu64", %"PRIu64")", M->rows, M->cols, N->rows, N->cols);
        return returnResultError(graph, CGET_INCOMPATIBLE_DIMENTIONS_EXCEPTION, parentNode, msg);
    }

    CGMatrix* Z = makeDeviceMatrix(M->rows, N->cols);

    
    CHECK_ERROR(err)
    
    CHECK_ERROR(err)


    CCLEvent* evt = NULL;
    cl_command_queue q = ccl_queue_unwrap(queue);
    cl_mem M_mem = ccl_buffer_unwrap(M->buf);
    cl_mem N_mem = ccl_buffer_unwrap(N->buf);
    cl_mem Z_mem = ccl_buffer_unwrap(Z->buf);

    /*
    cblas_dgemm(CblasRowMajor,
                CblasNoTrans, CblasNoTrans, M->rows, N->cols, M->cols,
                1.0, M->data, M->cols, N->data, N->cols, 0, z, Z->cols);
    */

    CLBlastDgemm(CLBlastLayoutRowMajor,
                 CLBlastTransposeNo, CLBlastTransposeNo, M->rows, N->cols, M->cols,
                 1.0,
                 M_mem, 0, M->cols,
                 N_mem, 0, N->cols,
                 0.0,
                 Z_mem, 0, Z->cols,
                 &q, NULL
    );


    //ccl_queue_destroy(queue);

    CGResultNode* result = calloc(1, sizeof(CGResultNode));
    result->type = CGVT_MATRIX;
    result->value = Z;

    return result;
}


/*
 * V.V
 */
CGResultNode* crossVV(CGVector* V1, CGVector* V2, CGraph* graph, CGNode* parentNode){
    CCLErr * err = NULL;
    if(V1->len != V2->len){
        char msg[MAX_ERR_FMT_LEN];
        snprintf(msg, MAX_ERR_FMT_LEN, "Cannot compute Cross Product of V(%"PRIu64") by V(%"PRIu64")", V1->len, V2->len);
        return returnResultError(graph, CGET_INCOMPATIBLE_DIMENTIONS_EXCEPTION, parentNode, msg);
    }


    uint64_t size = V1->len;

    CGVector* Y = makeDeviceVector(V1->len);
    uint64_t len = V1->len;

    
    CHECK_ERROR(err)

    
    CHECK_ERROR(err)


    CCLEvent* evt = NULL;
    size_t gws = len;

    evt = ccl_program_enqueue_kernel(prg, "mul_vv", queue, 1, NULL, &gws,
                                     NULL, NULL, &err, ccl_arg_priv(len, cl_uint), V1->buf, V2->buf, Y->buf, NULL);
    CHECK_ERROR(err)

    //ccl_queue_destroy(queue);

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
    CCLErr * err = NULL;
    if(V1->len != V2->len){
        char msg[MAX_ERR_FMT_LEN];
        snprintf(msg, MAX_ERR_FMT_LEN, "Cannot compute Dot Product of V(%"PRIu64") by V(%"PRIu64")", V1->len, V2->len);
        return returnResultError(graph, CGET_INCOMPATIBLE_DIMENTIONS_EXCEPTION, parentNode, msg);
    }

    uint64_t size = V1->len;
    CG_SCALAR_TYPE res = 0;

    CGDouble* Y = makeDeviceDouble();


    
    CHECK_ERROR(err)
    
    CHECK_ERROR(err)


    CCLEvent* evt = NULL;
    cl_command_queue q = ccl_queue_unwrap(queue);
    cl_mem M_mem = ccl_buffer_unwrap(V1->buf);
    cl_mem N_mem = ccl_buffer_unwrap(V2->buf);
    cl_mem Y_mem = ccl_buffer_unwrap(Y->buf);


    //Y->value = cblas_ddot(V1->len, V1->data, 1, V2->data, 1);

    CLBlastDdot(size, Y_mem, 0,
            M_mem, 0.0, 1,
            N_mem, 0.0, 1,
            &q, NULL);


    evt = ccl_buffer_enqueue_read(Y->buf, queue, CL_TRUE, 0,
                                  sizeof(cl_double), &Y->value, NULL, NULL);

    //ccl_queue_destroy(queue);
    ccl_buffer_destroy(Y->buf);

    Y->loc = CG_DATALOC_HOST_MEM;

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

    CGVector* Y = makeDeviceVector(V->len);
    CCLErr * err = NULL;

    //if(A->value != 0){
    uint64_t len = V->len;
    CCLBuffer * a = V->buf, * c = NULL;

    
    CHECK_ERROR(err)

    
    CHECK_ERROR(err)


    CCLEvent* evt = NULL;
    size_t gws = len;

    cl_mem V_mem = ccl_buffer_unwrap(V->buf);
    cl_mem Y_mem = ccl_buffer_unwrap(Y->buf);

    cl_command_queue q = ccl_queue_unwrap(queue);

    //cblas_dcopy(V->len, V->data, 1, y, 1);
    //cblas_dscal(V->len, a->value, y, 1);

    CLBlastDcopy(V->len, V_mem, 0.0, 1, Y_mem, 0.0, 1, &q, NULL);
    CLBlastDscal(V->len, 1.0/D->value, Y_mem, 0.0, 1, &q, NULL);
    //ccl_queue_destroy(queue);
    //}

    CGResultNode* result = calloc(1, sizeof(CGResultNode));
    result->type = CGVT_VECTOR;
    result->value = Y;

    return result;


}

/*
 *  V/V
 */
CGResultNode* divVV(CGVector* V1, CGVector* V2, CGraph* graph, CGNode* parentNode){
    if(V1->len != V2->len){
        char msg[MAX_ERR_FMT_LEN];
        snprintf(msg, MAX_ERR_FMT_LEN, "Cannot calculate  V(%"PRIu64") DIV V(%"PRIu64")", V1->len, V2->len);
        return returnResultError(graph, CGET_INCOMPATIBLE_DIMENTIONS_EXCEPTION, parentNode, msg);
    }
    CCLErr * err = NULL;


    uint64_t size = V1->len;

    CGVector* Y = makeDeviceVector(V1->len);
    uint64_t len = V1->len;

    
    CHECK_ERROR(err)

    
    CHECK_ERROR(err)


    CCLEvent* evt = NULL;
    size_t gws = len;

    evt = ccl_program_enqueue_kernel(prg, "div_vv", queue, 1, NULL, &gws,
                                     NULL, NULL, &err, ccl_arg_priv(len, cl_uint), V1->buf, V2->buf, Y->buf, NULL);
    CHECK_ERROR(err)

    //ccl_queue_destroy(queue);

    CGResultNode* result = calloc(1, sizeof(CGResultNode));
    result->type = CGVT_VECTOR;
    result->value = Y;

    return result;
}

/*
 *  d/V element-wise
 */
CGResultNode* divDV(CGDouble* D, CGVector* V, CGraph* graph, CGNode* parentNode){
    CCLErr * err = NULL;

    uint64_t size = V->len;

    CGVector* Y = makeDeviceVector(V->len);
    uint64_t len = V->len;

    // TODO: check if D.value == 0.0
    
    CHECK_ERROR(err)

    
    CHECK_ERROR(err)


    CCLEvent* evt = NULL;
    size_t gws = len;

    evt = ccl_program_enqueue_kernel(prg, "div_dv", queue, 1, NULL, &gws,
                                     NULL, NULL, &err, ccl_arg_priv(len, cl_uint), ccl_arg_priv(D->value, CG_CL_SCALAR_TYPE), V->buf, Y->buf, NULL);
    CHECK_ERROR(err)

    //ccl_queue_destroy(queue);

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

    uint64_t size = M->cols*M->rows;
    CG_SCALAR_TYPE value = D->value;

    CGMatrix* Y = makeDeviceMatrix(M->rows, M->cols);

    //if(A->value != 0){
    CCLErr * err = NULL;

    CCLBuffer * a = M->buf;

    
    CHECK_ERROR(err)

    
    CHECK_ERROR(err)


    cl_mem M_mem = ccl_buffer_unwrap(M->buf);
    cl_mem Y_mem = ccl_buffer_unwrap(Y->buf);

    cl_command_queue q = ccl_queue_unwrap(queue);

    CLBlastDcopy(size, M_mem, 0.0, 1, Y_mem, 0.0, 1, &q, NULL);
    CLBlastDscal(size, 1.0/D->value, Y_mem, 0.0, 1, &q, NULL);

    //ccl_queue_destroy(queue);
    //}

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

    CCLErr * err = NULL;

    CGMatrix* Y = makeDeviceMatrix(M->rows, M->cols);


    uint64_t len = M->rows*M->cols;
    uint64_t vlen = V->len;

    
    CHECK_ERROR(err)

    
    CHECK_ERROR(err)

    CCLEvent* evt = NULL;
    size_t gws = len;

    evt = ccl_program_enqueue_kernel(prg, "div_mv", queue, 1, NULL, &gws,
                                     NULL, NULL, &err, ccl_arg_priv(len, cl_uint), M->buf, V->buf, Y->buf, ccl_arg_priv(vlen, cl_ulong), NULL);
    CHECK_ERROR(err)

    //ccl_queue_destroy(queue);

    CGResultNode* result = calloc(1, sizeof(CGResultNode));
    result->type = CGVT_MATRIX;
    result->value = Y;

    return result;
}

/*
 *  d/M element-wise
 */
CGResultNode* divDM(CGDouble* D, CGMatrix* M, CGraph* graph, CGNode* parentNode){
    uint64_t len = M->rows*M->cols;
    CCLErr * err = NULL;

    CGMatrix* Y = makeDeviceMatrix(M->rows, M->cols);

    // TODO: check if D.value == 0.0
    
    CHECK_ERROR(err)

    
    CHECK_ERROR(err)


    CCLEvent* evt = NULL;
    size_t gws = len;

    evt = ccl_program_enqueue_kernel(prg, "div_dv", queue, 1, NULL, &gws,
                                     NULL, NULL, &err, ccl_arg_priv(len, cl_uint), ccl_arg_priv(D->value, CG_CL_SCALAR_TYPE), M->buf, Y->buf, NULL);
    CHECK_ERROR(err)

    //ccl_queue_destroy(queue);

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
    CCLErr * err = NULL;

    uint64_t size = V->len;

    CGVector* Y = makeDeviceVector(V->len);
    uint64_t len = V->len;

    
    CHECK_ERROR(err)

    
    CHECK_ERROR(err)


    CCLEvent* evt = NULL;
    size_t gws = len;

    evt = ccl_program_enqueue_kernel(prg, "add_dv", queue, 1, NULL, &gws,
                                     NULL, NULL, &err, ccl_arg_priv(len, cl_uint), ccl_arg_priv(D->value, CG_CL_SCALAR_TYPE), V->buf, Y->buf, NULL);
    CHECK_ERROR(err)

    //ccl_queue_destroy(queue);

    CGResultNode* result = calloc(1, sizeof(CGResultNode));
    result->type = CGVT_VECTOR;
    result->value = Y;

    return result;
}

/*
 * M+d
 */
CGResultNode* addMD(CGMatrix* M, CGDouble* D, CGraph* graph, CGNode* parentNode){
    CCLErr * err = NULL;

    uint64_t size = M->rows*M->cols;

    CGMatrix* Y = makeDeviceMatrix(M->rows, M->cols);

    
    CHECK_ERROR(err)

    
    CHECK_ERROR(err)


    CCLEvent* evt = NULL;
    size_t gws = size;

    evt = ccl_program_enqueue_kernel(prg, "add_dv", queue, 1, NULL, &gws,
                                     NULL, NULL, &err, ccl_arg_priv(size, cl_uint), ccl_arg_priv(D->value, CG_CL_SCALAR_TYPE), M->buf, Y->buf, NULL);
    CHECK_ERROR(err)

    //ccl_queue_destroy(queue);

    CGResultNode* result = calloc(1, sizeof(CGResultNode));
    result->type = CGVT_MATRIX;
    result->value = Y;

    return result;;
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
    CCLErr * err = NULL;


    CGVector* Y = makeDeviceVector(size);

    
    CHECK_ERROR(err)

    
    CHECK_ERROR(err)

    CCLEvent* evt = NULL;
    size_t gws = size;

    evt = ccl_program_enqueue_kernel(prg, "add_vv", queue, 1, NULL, &gws,
                                     NULL, NULL, &err, ccl_arg_priv(size, cl_uint), V1->buf, V2->buf, Y->buf, NULL);
    CHECK_ERROR(err)

    //ccl_queue_destroy(queue);

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

    CCLErr * err = NULL;

    CGMatrix* Y = makeDeviceMatrix(M->rows, M->cols);


    uint64_t len = M->rows*M->cols;
    uint64_t vlen = V->len;

    
    CHECK_ERROR(err)

    
    CHECK_ERROR(err)

    CCLEvent* evt = NULL;
    size_t gws = len;

    evt = ccl_program_enqueue_kernel(prg, "add_mv", queue, 1, NULL, &gws,
                                     NULL, NULL, &err, ccl_arg_priv(len, cl_uint), M->buf, V->buf, Y->buf, ccl_arg_priv(vlen, cl_ulong), NULL);
    CHECK_ERROR(err)

    //ccl_queue_destroy(queue);

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

    CCLErr * err = NULL;
    uint64_t size = M1->cols*M1->rows;

    CGMatrix* Y = makeDeviceMatrix(M1->rows, M1->cols);
    uint64_t len = M1->rows*M1->cols;

    
    CHECK_ERROR(err)

    
    CHECK_ERROR(err)

    CCLEvent* evt = NULL;
    size_t gws = len;

    evt = ccl_program_enqueue_kernel(prg, "add_vv", queue, 1, NULL, &gws,
                                     NULL, NULL, &err, ccl_arg_priv(len, cl_uint), M1->buf, M2->buf, Y->buf, NULL);
    CHECK_ERROR(err)

    //ccl_queue_destroy(queue);

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
    CCLErr * err = NULL;

    uint64_t size = V->len;

    CGVector* Y = makeDeviceVector(V->len);
    uint64_t len = V->len;

    
    CHECK_ERROR(err)

    
    CHECK_ERROR(err)


    CCLEvent* evt = NULL;
    size_t gws = len;

    evt = ccl_program_enqueue_kernel(prg, "sub_vd", queue, 1, NULL, &gws,
                                     NULL, NULL, &err, ccl_arg_priv(len, cl_uint), ccl_arg_priv(D->value, CG_CL_SCALAR_TYPE), V->buf, Y->buf, NULL);
    CHECK_ERROR(err)

    //ccl_queue_destroy(queue);

    CGResultNode* result = calloc(1, sizeof(CGResultNode));
    result->type = CGVT_VECTOR;
    result->value = Y;

    return result;
}

/*
 * d-V
 */
CGResultNode* subDV(CGDouble* D, CGVector* V, CGraph* graph, CGNode* parentNode){
    CCLErr * err = NULL;

    uint64_t size = V->len;

    CGVector* Y = makeDeviceVector(V->len);
    uint64_t len = V->len;

    
    CHECK_ERROR(err)

    
    CHECK_ERROR(err)


    CCLEvent* evt = NULL;
    size_t gws = len;

    evt = ccl_program_enqueue_kernel(prg, "sub_dv", queue, 1, NULL, &gws,
                                     NULL, NULL, &err, ccl_arg_priv(len, cl_uint), ccl_arg_priv(D->value, CG_CL_SCALAR_TYPE), V->buf, Y->buf, NULL);
    CHECK_ERROR(err)

    //ccl_queue_destroy(queue);

    CGResultNode* result = calloc(1, sizeof(CGResultNode));
    result->type = CGVT_VECTOR;
    result->value = Y;

    return result;
}


/*
 * M-d
 */
CGResultNode* subMD(CGMatrix* M, CGDouble* D, CGraph* graph, CGNode* parentNode){
    CCLErr * err = NULL;

    uint64_t size = M->rows*M->cols;

    CGMatrix* Y = makeDeviceMatrix(M->rows, M->cols);

    
    CHECK_ERROR(err)

    
    CHECK_ERROR(err)


    CCLEvent* evt = NULL;
    size_t gws = size;

    evt = ccl_program_enqueue_kernel(prg, "sub_vd", queue, 1, NULL, &gws,
                                     NULL, NULL, &err, ccl_arg_priv(size, cl_uint), ccl_arg_priv(D->value, CG_CL_SCALAR_TYPE), M->buf, Y->buf, NULL);
    CHECK_ERROR(err)

    //ccl_queue_destroy(queue);

    CGResultNode* result = calloc(1, sizeof(CGResultNode));
    result->type = CGVT_MATRIX;
    result->value = Y;

    return result;
}

/*
 * D-M
 */
CGResultNode* subDM(CGDouble* D, CGMatrix* M, CGraph* graph, CGNode* parentNode){
    CCLErr * err = NULL;

    uint64_t size = M->rows*M->cols;

    CGMatrix* Y = makeDeviceMatrix(M->rows, M->cols);

    
    CHECK_ERROR(err)

    
    CHECK_ERROR(err)


    CCLEvent* evt = NULL;
    size_t gws = size;

    evt = ccl_program_enqueue_kernel(prg, "sub_dv", queue, 1, NULL, &gws,
                                     NULL, NULL, &err, ccl_arg_priv(size, cl_uint), ccl_arg_priv(D->value, CG_CL_SCALAR_TYPE), M->buf, Y->buf, NULL);
    CHECK_ERROR(err)

    //ccl_queue_destroy(queue);

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
    CCLErr * err = NULL;


    CGVector* Y = makeDeviceVector(size);

    
    CHECK_ERROR(err)

    
    CHECK_ERROR(err)

    CCLEvent* evt = NULL;
    size_t gws = size;

    evt = ccl_program_enqueue_kernel(prg, "sub_vv", queue, 1, NULL, &gws,
                                     NULL, NULL, &err, ccl_arg_priv(size, cl_uint), V1->buf, V2->buf, Y->buf, NULL);
    CHECK_ERROR(err)

    //ccl_queue_destroy(queue);

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

    CCLErr * err = NULL;
    uint64_t size = M1->cols*M1->rows;

    CGMatrix* Y = makeDeviceMatrix(M1->rows, M1->cols);
    uint64_t len = M1->rows*M1->cols;

    
    CHECK_ERROR(err)

    
    CHECK_ERROR(err)

    CCLEvent* evt = NULL;
    size_t gws = len;

    evt = ccl_program_enqueue_kernel(prg, "sub_vv", queue, 1, NULL, &gws,
                                     NULL, NULL, &err, ccl_arg_priv(len, cl_uint), M1->buf, M2->buf, Y->buf, NULL);
    CHECK_ERROR(err)

    //ccl_queue_destroy(queue);

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

    CCLErr * err = NULL;

    CGMatrix* Y = makeDeviceMatrix(M->rows, M->cols);

    uint64_t len = M->rows*M->cols;
    uint64_t vlen = V->len;

    
    CHECK_ERROR(err)

    
    CHECK_ERROR(err)

    CCLEvent* evt = NULL;
    size_t gws = len;

    evt = ccl_program_enqueue_kernel(prg, "sub_mv", queue, 1, NULL, &gws,
                                     NULL, NULL, &err, ccl_arg_priv(len, cl_uint), M->buf, V->buf, Y->buf, ccl_arg_priv(vlen, cl_ulong), NULL);
    CHECK_ERROR(err)

    //ccl_queue_destroy(queue);

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
        snprintf(msg, MAX_ERR_FMT_LEN, "Cannot Substract M(%"PRIu64", %"PRIu64") by V(%"PRIu64")", M->rows, M->cols, V->len);
        return returnResultError(graph, CGET_INCOMPATIBLE_DIMENTIONS_EXCEPTION, parentNode, msg);
    }

    CCLErr * err = NULL;

    CGMatrix* Y = makeDeviceMatrix(M->rows, M->cols);

    uint64_t len = M->rows*M->cols;
    uint64_t vlen = V->len;

    
    CHECK_ERROR(err)

    
    CHECK_ERROR(err)

    CCLEvent* evt = NULL;
    size_t gws = len;

    evt = ccl_program_enqueue_kernel(prg, "sub_mv", queue, 1, NULL, &gws,
                                     NULL, NULL, &err, ccl_arg_priv(len, cl_uint), M->buf, V->buf, Y->buf, ccl_arg_priv(vlen, cl_ulong), NULL);
    CHECK_ERROR(err)

    //ccl_queue_destroy(queue);

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
    CG_SCALAR_TYPE value = D->value;

    CCLErr * err = NULL;

    CGVector* Y = makeDeviceVector(V->len);

    uint64_t len = V->len;

    CCLEvent* evt = NULL;
    size_t gws = len;

    evt = ccl_program_enqueue_kernel(prg, "pow_vd", queue, 1, NULL, &gws,
                                     NULL, NULL, &err, ccl_arg_priv(len, cl_uint), ccl_arg_priv(value, CG_CL_SCALAR_TYPE), V->buf, Y->buf, NULL);
    CHECK_ERROR(err)


    CGResultNode* result = calloc(1, sizeof(CGResultNode));
    result->type = CGVT_VECTOR;
    result->value = Y;

    return result;
}

/*
 * M^d
 */
CGResultNode* powMD(CGMatrix* M, CGDouble* D, CGraph* graph, CGNode* parentNode){
    CG_SCALAR_TYPE value = D->value;

    CCLErr * err = NULL;

    CGMatrix* Y = makeDeviceMatrix(M->rows, M->cols);

    uint64_t len = M->rows*M->cols;

    CCLEvent* evt = NULL;
    size_t gws = len;

    evt = ccl_program_enqueue_kernel(prg, "pow_vd", queue, 1, NULL, &gws,
                                     NULL, NULL, &err, ccl_arg_priv(len, cl_uint), ccl_arg_priv(value, CG_CL_SCALAR_TYPE), M->buf, Y->buf, NULL);
    CHECK_ERROR(err)


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
 * TODO
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
    CCLErr * err = NULL;

    CGVector* Y = makeDeviceVector(V->len);

    uint64_t len = V->len;

    CCLEvent* evt = NULL;
    size_t gws = len;

    evt = ccl_program_enqueue_kernel(prg, "exp_v", queue, 1, NULL, &gws,
                                     NULL, NULL, &err, ccl_arg_priv(len, cl_uint), V->buf, Y->buf, NULL);
    CHECK_ERROR(err)

    CGResultNode* result = calloc(1, sizeof(CGResultNode));
    result->type = CGVT_VECTOR;
    result->value = Y;

    return result;
}

/*
 * exp(M)
 */
CGResultNode* expM(CGMatrix* M, CGraph* graph, CGNode* parentNode){
    CCLErr * err = NULL;

    CGMatrix* Y = makeDeviceMatrix(M->rows, M->cols);
    uint64_t len = M->rows*M->cols;

    CCLEvent* evt = NULL;
    size_t gws = len;

    evt = ccl_program_enqueue_kernel(prg, "exp_v", queue, 1, NULL, &gws,
                                     NULL, NULL, &err, ccl_arg_priv(len, cl_uint), M->buf, Y->buf, NULL);
    CHECK_ERROR(err)

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
    CCLErr * err = NULL;

    CGVector* Y = makeDeviceVector(V->len);

    uint64_t len = V->len;

    CCLEvent* evt = NULL;
    size_t gws = len;

    evt = ccl_program_enqueue_kernel(prg, "log_v", queue, 1, NULL, &gws,
                                     NULL, NULL, &err, ccl_arg_priv(len, cl_uint), V->buf, Y->buf, NULL);
    CHECK_ERROR(err)

    CGResultNode* result = calloc(1, sizeof(CGResultNode));
    result->type = CGVT_VECTOR;
    result->value = Y;

    return result;
}

/*
 * exp(M)
 */
CGResultNode* logM(CGMatrix* M, CGraph* graph, CGNode* parentNode){
    CCLErr * err = NULL;

    CGMatrix* Y = makeDeviceMatrix(M->rows, M->cols);
    uint64_t len = M->rows*M->cols;

    CCLEvent* evt = NULL;
    size_t gws = len;

    evt = ccl_program_enqueue_kernel(prg, "log_v", queue, 1, NULL, &gws,
                                     NULL, NULL, &err, ccl_arg_priv(len, cl_uint), M->buf, Y->buf, NULL);
    CHECK_ERROR(err)

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
    CCLErr * err = NULL;

    CGVector* Y = makeDeviceVector(V->len);

    uint64_t len = V->len;

    CCLEvent* evt = NULL;
    size_t gws = len;

    evt = ccl_program_enqueue_kernel(prg, "sin_v", queue, 1, NULL, &gws,
                                     NULL, NULL, &err, ccl_arg_priv(len, cl_uint), V->buf, Y->buf, NULL);
    CHECK_ERROR(err)


    CGResultNode* result = calloc(1, sizeof(CGResultNode));
    result->type = CGVT_VECTOR;
    result->value = Y;

    return result;
}

/*
 * sin(M)
 */
CGResultNode* sinM(CGMatrix* M, CGraph* graph, CGNode* parentNode){
    CCLErr * err = NULL;

    CGMatrix* Y = makeDeviceMatrix(M->rows, M->cols);
    uint64_t len = M->rows*M->cols;

    CCLEvent* evt = NULL;
    size_t gws = len;

    evt = ccl_program_enqueue_kernel(prg, "sin_v", queue, 1, NULL, &gws,
                                     NULL, NULL, &err, ccl_arg_priv(len, cl_uint), M->buf, Y->buf, NULL);
    CHECK_ERROR(err)

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
    CCLErr * err = NULL;

    CGVector* Y = makeDeviceVector(V->len);

    uint64_t len = V->len;

    CCLEvent* evt = NULL;
    size_t gws = len;

    evt = ccl_program_enqueue_kernel(prg, "cos_v", queue, 1, NULL, &gws,
                                     NULL, NULL, &err, ccl_arg_priv(len, cl_uint), V->buf, Y->buf, NULL);
    CHECK_ERROR(err)


    CGResultNode* result = calloc(1, sizeof(CGResultNode));
    result->type = CGVT_VECTOR;
    result->value = Y;

    return result;
}

/*
 * sin(M)
 */
CGResultNode* cosM(CGMatrix* M, CGraph* graph, CGNode* parentNode){
    CCLErr * err = NULL;

    CGMatrix* Y = makeDeviceMatrix(M->rows, M->cols);
    uint64_t len = M->rows*M->cols;

    CCLEvent* evt = NULL;
    size_t gws = len;

    evt = ccl_program_enqueue_kernel(prg, "cos_v", queue, 1, NULL, &gws,
                                     NULL, NULL, &err, ccl_arg_priv(len, cl_uint), M->buf, Y->buf, NULL);
    CHECK_ERROR(err)

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
    CCLErr * err = NULL;

    CGVector* Y = makeDeviceVector(V->len);

    uint64_t len = V->len;

    CCLEvent* evt = NULL;
    size_t gws = len;

    evt = ccl_program_enqueue_kernel(prg, "tan_v", queue, 1, NULL, &gws,
                                     NULL, NULL, &err, ccl_arg_priv(len, cl_uint), V->buf, Y->buf, NULL);
    CHECK_ERROR(err)


    CGResultNode* result = calloc(1, sizeof(CGResultNode));
    result->type = CGVT_VECTOR;
    result->value = Y;

    return result;
}

/*
 * tan(M)
 */
CGResultNode* tanM(CGMatrix* M, CGraph* graph, CGNode* parentNode){
    CCLErr * err = NULL;

    CGMatrix* Y = makeDeviceMatrix(M->rows, M->cols);
    uint64_t len = M->rows*M->cols;

    CCLEvent* evt = NULL;
    size_t gws = len;

    evt = ccl_program_enqueue_kernel(prg, "tan_v", queue, 1, NULL, &gws,
                                     NULL, NULL, &err, ccl_arg_priv(len, cl_uint), M->buf, Y->buf, NULL);
    CHECK_ERROR(err)

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
    CCLErr * err = NULL;

    CGVector* Y = makeDeviceVector(V->len);

    uint64_t len = V->len;

    CCLEvent* evt = NULL;
    size_t gws = len;

    evt = ccl_program_enqueue_kernel(prg, "tanh_v", queue, 1, NULL, &gws,
                                     NULL, NULL, &err, ccl_arg_priv(len, cl_uint), V->buf, Y->buf, NULL);
    CHECK_ERROR(err)


    CGResultNode* result = calloc(1, sizeof(CGResultNode));
    result->type = CGVT_VECTOR;
    result->value = Y;

    return result;
}

/*
 * tanh(M)
 */
CGResultNode* tanhM(CGMatrix* M, CGraph* graph, CGNode* parentNode){
    CCLErr * err = NULL;

    CGMatrix* Y = makeDeviceMatrix(M->rows, M->cols);
    uint64_t len = M->rows*M->cols;

    CCLEvent* evt = NULL;
    size_t gws = len;

    evt = ccl_program_enqueue_kernel(prg, "tanh_v", queue, 1, NULL, &gws,
                                     NULL, NULL, &err, ccl_arg_priv(len, cl_uint), M->buf, Y->buf, NULL);
    CHECK_ERROR(err)

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


    CCLErr * err = NULL;
    CGMatrix* Y = calloc(1, sizeof(CGMatrix));
    Y->rows = 1;
    Y->cols = V->len;
    Y->data = NULL;

    Y->buf = ccl_buffer_new(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR ,
                            V->len * sizeof(CG_CL_SCALAR_TYPE), Y->data, &err);
    CHECK_ERROR(err)

    Y->loc = CG_DATALOC_DEVICE_MEM;
    CGResultNode* result = calloc(1, sizeof(CGResultNode));
    result->type = CGVT_MATRIX;
    result->value = Y;

    return result;
}


/*
 * M^t
 */
CGResultNode* transposeM(CGMatrix* M, CGraph* graph, CGNode* parentNode){
    uint64_t size = M->cols*M->rows;
    CGMatrix* Y = makeDeviceMatrix(M->cols, M->rows);

    //if(A->value != 0){
    CCLErr * err = NULL;

    CCLBuffer * a = M->buf;


    CHECK_ERROR(err)


    CHECK_ERROR(err)


    cl_mem M_mem = ccl_buffer_unwrap(M->buf);
    cl_mem Y_mem = ccl_buffer_unwrap(Y->buf);

    cl_command_queue q = ccl_queue_unwrap(queue);

    //cblas_dcopy(V->len, V->data, 1, y, 1);
    //cblas_dscal(V->len, a->value, y, 1);

    //CLBlastDcopy(size, M_mem, 0.0, 1, Y_mem, 0.0, 1, &q, NULL);
    CLBlastDomatcopy(CLBlastLayoutRowMajor, CLBlastTransposeYes, M->rows, M->cols, 1.0f, M_mem, 0, M->cols, Y_mem, 0, Y->cols, &q, NULL);


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
#undef cl_double
#endif


#if CG_SCALAR_TYPE == float
#undef CLBlastDcopy CLBlastScopy
#undef CLBlastDscal CLBlastSscal
#undef CLBlastDgemm CLBlastSgemm
#undef CLBlastDdot CLBlastSdot
#endif