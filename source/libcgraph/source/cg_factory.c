

#include <inttypes.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>


#include "cgraph.h"
#include "cg_operation.h"
#include "cg_types.h"
#include "cg_variables.h"
#include "cg_factory.h"
#include "cg_enums.h"

#include "vec.h"
#include "map.h"
#include "random.h"


#define CHECK_ERROR(err) \
    if (err != NULL) { fprintf(stderr, "\n%s\n", err->message); exit(-1); }

#ifdef CG_USE_OPENCL
#include <cf4ocl2.h>
extern CCLContext * ctx;
#endif

CG_SCALAR_TYPE* vcopy(uint64_t len, const CG_SCALAR_TYPE* data){
    CG_SCALAR_TYPE* newdata = calloc(len, sizeof(CG_SCALAR_TYPE));
    memcpy(newdata, data, len*sizeof(CG_SCALAR_TYPE));
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

    return node;
}

CGNode* makeDoubleConstantNode(CG_SCALAR_TYPE value){
    CGDouble* d = calloc(1, sizeof(CGDouble));
    d->value = value;

    CGPConstant* c = calloc(1, sizeof(CGPConstant));
    c->type = CGVT_DOUBLE;
    c->value = d;

    CGNode* node = calloc(1, sizeof(CGNode));
    node->type = CGNT_CONSTANT;
    node->constant = c;

    node->result = NULL;
    node->diff = NULL;

    return node;
}

CGNode* makeVectorConstantNode(uint64_t  len, CG_SCALAR_TYPE* value){
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
    node->diff = NULL;

    return node;
}


CGNode* makeMatrixConstantNode(uint64_t  rows, uint64_t cols, CG_SCALAR_TYPE* value){
    CGMatrix* m = calloc(1, sizeof(CGMatrix));
    m->data = value;
    m->rows = rows;
    m->cols = cols;

    CGPConstant* c = calloc(1, sizeof(CGPConstant));
    c->type = CGVT_MATRIX;
    c->value = m;

    CGNode* node = calloc(1, sizeof(CGNode));
    node->type = CGNT_CONSTANT;
    node->constant = c;

    node->result = NULL;
    node->diff = NULL;

    return node;
}


CGNode* makeVectorConstantNodeCopy(uint64_t  len, CG_SCALAR_TYPE* value){
    return makeVectorConstantNode(len, cg_raw_copy(value, len));
}

CGNode* makeMatrixConstantNodeCopy(uint64_t  rows, uint64_t cols, CG_SCALAR_TYPE* value){
    return makeMatrixConstantNode(rows, cols, cg_raw_copy(value, cols*rows));
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
    node->diff = NULL;

    return node;
}

CGNode* makeZeroVectorConstantNode(uint64_t  len){
    CGVector* v = calloc(1, sizeof(CGVector));
    v->len = len;
    v->data = calloc(len, sizeof(CG_SCALAR_TYPE));

    CGPConstant* c = calloc(1, sizeof(CGPConstant));
    c->type = CGVT_VECTOR;
    c->value = v;

    CGNode* node = calloc(1, sizeof(CGNode));
    node->type = CGNT_CONSTANT;
    node->constant = c;

    node->result = NULL;
    node->diff = NULL;

    return node;
}


CGNode* makeZeroMatrixConstantNode(uint64_t  rows, uint64_t cols){
    CGMatrix* m = calloc(1, sizeof(CGMatrix));
    m->data = calloc(rows*cols, sizeof(CG_SCALAR_TYPE));
    m->rows = rows;
    m->cols = cols;

    CGPConstant* c = calloc(1, sizeof(CGPConstant));
    c->type = CGVT_MATRIX;
    c->value = m;

    CGNode* node = calloc(1, sizeof(CGNode));
    node->type = CGNT_CONSTANT;
    node->constant = c;

    node->result = NULL;
    node->diff = NULL;

    return node;
}

#define TEST_SEED 54321U

CGNode* makeRandomDoubleConstantNode(){
    CGDouble* d = calloc(1, sizeof(CGDouble));

    MT mt;

    init_genrand(&mt, time(NULL));
    d->value = genrand_res53(&mt);
    d->value = (d->value - 0.5)/10;

    CGPConstant* c = calloc(1, sizeof(CGPConstant));
    c->type = CGVT_DOUBLE;
    c->value = d;

    CGNode* node = calloc(1, sizeof(CGNode));
    node->type = CGNT_CONSTANT;
    node->constant = c;

    node->result = NULL;
    node->diff = NULL;

    return node;
}

CGNode* makeRandomVectorConstantNode(uint64_t  len){
    CGVector* v = calloc(1, sizeof(CGVector));
    v->len = len;
    v->data = calloc(len, sizeof(CG_SCALAR_TYPE));

    uint64_t i = 0;

    MT mt;

    init_genrand(&mt, time(NULL));
    for(;i<len;i++){
        v->data[i] = (genrand_res53(&mt) - 0.5)/10;
    }

    CGPConstant* c = calloc(1, sizeof(CGPConstant));
    c->type = CGVT_VECTOR;
    c->value = v;

    CGNode* node = calloc(1, sizeof(CGNode));
    node->type = CGNT_CONSTANT;
    node->constant = c;

    node->result = NULL;
    node->diff = NULL;

    return node;
}


CGNode* makeRandomMatrixConstantNode(uint64_t  rows, uint64_t cols){
    CGMatrix* m = calloc(1, sizeof(CGMatrix));
    m->data = calloc(rows*cols, sizeof(CG_SCALAR_TYPE));
    m->rows = rows;
    m->cols = cols;


    uint64_t i = 0;

    MT mt;

    init_genrand(&mt, time(NULL));
    for(;i<rows*cols;i++){
        m->data[i] = (genrand_res53(&mt))/10;
    }

    CGPConstant* c = calloc(1, sizeof(CGPConstant));
    c->type = CGVT_MATRIX;
    c->value = m;

    CGNode* node = calloc(1, sizeof(CGNode));
    node->type = CGNT_CONSTANT;
    node->constant = c;

    node->result = NULL;
    node->diff = NULL;

    return node;
}





CGNode* makeOnesDoubleConstantNode(){
    CGDouble* d = calloc(1, sizeof(CGDouble));
    d->value=1;
    CGPConstant* c = calloc(1, sizeof(CGPConstant));
    c->type = CGVT_DOUBLE;
    c->value = d;

    CGNode* node = calloc(1, sizeof(CGNode));
    node->type = CGNT_CONSTANT;
    node->constant = c;

    node->result = NULL;
    node->diff = NULL;

    return node;
}

CGNode* makeOnesVectorConstantNode(uint64_t  len){
    CGVector* v = calloc(1, sizeof(CGVector));
    v->len = len;
    v->data = calloc(len, sizeof(CG_SCALAR_TYPE));

    uint64_t i = 0;
    for(;i<len;v->data[i++]=1);

    CGPConstant* c = calloc(1, sizeof(CGPConstant));
    c->type = CGVT_VECTOR;
    c->value = v;

    CGNode* node = calloc(1, sizeof(CGNode));
    node->type = CGNT_CONSTANT;
    node->constant = c;

    node->result = NULL;
    node->diff = NULL;

    return node;
}


CGNode* makeOnesMatrixConstantNode(uint64_t  rows, uint64_t cols){
    CGMatrix* m = calloc(1, sizeof(CGMatrix));
    m->data = calloc(rows*cols, sizeof(CG_SCALAR_TYPE));
    m->rows = rows;
    m->cols = cols;

    uint64_t i = 0;
    for(;i<cols*rows;m->data[i++]=1);


    CGPConstant* c = calloc(1, sizeof(CGPConstant));
    c->type = CGVT_MATRIX;
    c->value = m;

    CGNode* node = calloc(1, sizeof(CGNode));
    node->type = CGNT_CONSTANT;
    node->constant = c;

    node->result = NULL;
    node->diff = NULL;

    return node;
}

CGNode* makeGraphNode(CGraph* graph){
    CGNode* node = calloc(1, sizeof(CGNode));
    node->type = CGNT_GRAPH;
    node->graph = graph;

    node->result = NULL;
    node->diff = NULL;

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
    node->diff = NULL;

    return node;
}

CGNode* makeUnaryOpNode(CGUnaryOperationType type, CGNode* uhs){
    CGNode* node = calloc(1, sizeof(CGNode));
    node->type = CGNT_UNARY_OPERATION;

    node->uop = calloc(1, sizeof(CGUnaryOperation));
    node->uop->type = type;
    node->uop->uhs = uhs;

    node->result = NULL;
    node->diff = NULL;

    return node;
}

CGNode* makeAxisBoundNode(CGAxisBoundOperationType type, CGNode* uhs, uint8_t axis){
    CGNode* node = calloc(1, sizeof(CGNode));
    node->type = CGNT_AXIS_BOUND_OPERATION;
    node->axop = calloc(1, sizeof(CGAxisBoundOperation));

    node->axop->type = type;
    node->axop->axis = axis;
    node->axop->uhs = uhs;

    node->result = NULL;
    node->diff = NULL;

    return node;
}


CGNode* makeCrossEntropyLossFunc(CGNode* x, CGNode* y, uint64_t num_classes){
    CGNode* node = calloc(1, sizeof(CGNode));
    node->type = CGNT_CROSS_ENTROPY_LOSS_FUNC;
    node->crossEntropyLoss = calloc(1, sizeof(CGCrossEntropyLoss));
    node->crossEntropyLoss->x = x;
    node->crossEntropyLoss->y = y;
    node->crossEntropyLoss->num_classes = num_classes;

    node->result = NULL;
    node->diff = NULL;

    return node;
}

CGResultNode* makeDoubleResultNode(CG_SCALAR_TYPE val){
    CGDouble* Y = calloc(1, sizeof(CGDouble));
    Y->value = val;

    CGResultNode* result = calloc(1, sizeof(CGResultNode));
    result->type = CGVT_DOUBLE;
    result->value = Y;

    return result;
}

CGResultNode* makeVectorResultNode(uint64_t len, CG_SCALAR_TYPE* val){
    CGVector* v = calloc(1, sizeof(CGVector));
    v->data = val;
    v->len = len;

#ifdef CG_USE_OPENCL
    CCLErr * err = NULL;
    if(len == 0){
        printf("computer says no.\n");
    }
    v->buf = ccl_buffer_new(ctx, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR,
                            len * sizeof(CG_CL_SCALAR_TYPE), v->data, &err);
    CHECK_ERROR(err)
#endif

    CGResultNode* result = calloc(1, sizeof(CGResultNode));
    result->type = CGVT_VECTOR;
    result->value = v;

    return result;
}

CGResultNode* makeMatrixResultNode(uint64_t rows, uint64_t cols, CG_SCALAR_TYPE* val){
    CGMatrix* m = calloc(1, sizeof(CGMatrix));
    m->data = val;
    m->rows = rows;
    m->cols = cols;

    if(rows*cols == 0){
        printf("computer says no.\n");
    }
#ifdef CG_USE_OPENCL
    CCLErr * err = NULL;
    m->buf = ccl_buffer_new(ctx, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR,
                            cols*rows * sizeof(CG_CL_SCALAR_TYPE), m->data, &err);
    CHECK_ERROR(err)
#endif

    CGResultNode* result = calloc(1, sizeof(CGResultNode));
    result->type = CGVT_MATRIX;
    result->value = m;

    return result;
}

CGraph* makeGraph(char* name){
    CGraph* graph = calloc(1, sizeof(CGraph));
    graph->name = name;
    map_init(&graph->vars);
    vec_init(&graph->nodes);

    return graph;
}

void graphSetVar(CGraph* graph, const char* name, CGNode* value){
    CGNode** old = map_get(&graph->vars, name);
    if(old != NULL){
        //freeNode(graph, *old);
        //free(*old);
        map_remove(&graph->vars, name);
    }

    int res = map_set(&graph->vars, name, value);
}

CGNode* graphGetVar(CGraph* graph, const char* name){
    CGNode** noderef = map_get(&graph->vars, name);
    if (noderef == NULL)
        return NULL;
    return *noderef;
}

void graphUnsetVar(CGraph* graph, const char* name){
    map_remove(&graph->vars, name);
}


CG_SCALAR_TYPE* cg_raw_copy(CG_SCALAR_TYPE* src, uint64_t len){
    CG_SCALAR_TYPE* dest = calloc(len, sizeof(CG_SCALAR_TYPE));
    memcpy(dest, src, len*sizeof(CG_SCALAR_TYPE));
    return dest;
}

#ifndef CG_USE_OPENCL

CGResultNode* constantNodeToResultNode(CGNode* node){
    switch(node->constant->type){
        case CGVT_DOUBLE:
        {
            CGDouble* d = (CGDouble*)node->constant->value;
            return makeDoubleResultNode(d->value);
        }
        case CGVT_VECTOR:
        {
            CGVector* v = (CGVector*)node->constant->value;
            return makeVectorResultNode(v->len, v->data);
        }
        case CGVT_MATRIX:
        {
            CGMatrix* m = (CGMatrix*)node->constant->value;
            return makeMatrixResultNode(m->rows, m->cols, m->data);
        }
    }
}


CGResultNode* constantNodeToResultNodeCopy(CGNode* node){
    switch(node->constant->type){
        case CGVT_DOUBLE:
        {
            CGDouble* d = (CGDouble*)node->constant->value;
            return makeDoubleResultNode(d->value);
        }
        case CGVT_VECTOR:
        {
            CGVector* v = (CGVector*)node->constant->value;
            return makeVectorResultNode(v->len, cg_raw_copy(v->data, v->len));
        }
        case CGVT_MATRIX:
        {
            CGMatrix* m = (CGMatrix*)node->constant->value;
            return makeMatrixResultNode(m->rows, m->cols, cg_raw_copy(m->data, m->rows*m->cols));
        }
    }
}


CGNode* resultNodeToConstantNodeCopy(CGResultNode* result){
    switch(result->type){
        case CGVT_DOUBLE:
        {
            CGDouble* d = (CGDouble*)result->value;
            return makeDoubleConstantNode(d->value);
        }
        case CGVT_VECTOR:
        {
            CGVector* v = (CGVector*)result->value;
            return makeVectorConstantNode(v->len, cg_raw_copy(v->data, v->len));
        }
        case CGVT_MATRIX:
        {
            CGMatrix* m = (CGMatrix*)result->value;
            return makeMatrixConstantNode(m->rows, m->cols, cg_raw_copy(m->data, m->rows*m->cols));
        }
    }
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


#else


CGNode* makeDeviceConstantVectorNode(uint64_t len, CCLBuffer* buf){
    CGVector* v = calloc(1, sizeof(CGVector));
    v->data = NULL;
    v->len = len;
    v->buf = buf;
    v->loc = CG_DATALOC_DEVICE_MEM;

    CGPConstant* c = calloc(1, sizeof(CGPConstant));
    c->type = CGVT_VECTOR;
    c->value = v;

    CGNode* node = calloc(1, sizeof(CGNode));
    node->type = CGNT_CONSTANT;
    node->constant = c;

    node->result = NULL;
    node->diff = NULL;

    return node;
}


CGNode* makeDeviceConstantMatrixNode(uint64_t rows, uint64_t cols, CCLBuffer* buf){
    CGMatrix* m = calloc(1, sizeof(CGMatrix));
    m->data = NULL;
    m->rows = rows;
    m->cols = cols;
    m->buf = buf;
    m->loc = CG_DATALOC_DEVICE_MEM;

    CGPConstant* c = calloc(1, sizeof(CGPConstant));
    c->type = CGVT_MATRIX;
    c->value = m;

    CGNode* node = calloc(1, sizeof(CGNode));
    node->type = CGNT_CONSTANT;
    node->constant = c;

    node->result = NULL;
    node->diff = NULL;

    return node;
}

CGResultNode* makeDeviceVectorResultNode(uint64_t len, CCLBuffer* buf){
    CGVector* v = calloc(1, sizeof(CGVector));
    v->data = NULL;
    v->len = len;
    v->buf = buf;
    v->loc = CG_DATALOC_DEVICE_MEM;

    CGResultNode* result = calloc(1, sizeof(CGResultNode));
    result->type = CGVT_VECTOR;
    result->value = v;

    return result;
}

CGResultNode* makeDeviceMatrixResultNode(uint64_t rows, uint64_t cols, CCLBuffer* buf){
    CGMatrix* m = calloc(1, sizeof(CGMatrix));
    m->data = NULL;
    m->rows = rows;
    m->cols = cols;
    m->buf = buf;
    m->loc = CG_DATALOC_DEVICE_MEM;


    CGResultNode* result = calloc(1, sizeof(CGResultNode));
    result->type = CGVT_MATRIX;
    result->value = m;

    return result;
}


CGNode* resultNodeToConstantNodeCopy(CGResultNode* result){
    switch(result->type){
        case CGVT_DOUBLE:
        {
            CGDouble* d = (CGDouble*)result->value;
            return makeDoubleConstantNode(d->value);
        }
        case CGVT_VECTOR:
        {
            CGVector* v = (CGVector*)result->value;
            if(v->loc == CG_DATALOC_HOST_MEM)
                return makeVectorConstantNode(v->len, cg_raw_copy(v->data, v->len));
            else
                return makeDeviceConstantVectorNode(v->len, v->buf);
        }
        case CGVT_MATRIX:
        {
            CGMatrix* m = (CGMatrix*)result->value;

            if(m->loc == CG_DATALOC_HOST_MEM)
                return makeMatrixConstantNode(m->rows, m->cols, cg_raw_copy(m->data, m->cols*m->rows));
            else
                return makeDeviceConstantMatrixNode(m->rows, m->cols, m->buf);
        }
    }
}

CGNode* resultNodeToConstantNode(CGResultNode* result){
    {
        switch(result->type){
            case CGVT_DOUBLE:
            {
                CGDouble* d = (CGDouble*)result->value;
                return makeDoubleConstantNode(d->value);
            }
            case CGVT_VECTOR:
            {
                CGVector* v = (CGVector*)result->value;
                if(v->loc == CG_DATALOC_HOST_MEM)
                    return makeVectorConstantNode(v->len, v->data);
                else
                    return makeDeviceConstantVectorNode(v->len, v->buf);
            }
            case CGVT_MATRIX:
            {
                CGMatrix* m = (CGMatrix*)result->value;

                if(m->loc == CG_DATALOC_HOST_MEM)
                    return makeMatrixConstantNode(m->rows, m->cols, m->data);
                else
                    return makeDeviceConstantMatrixNode(m->rows, m->cols, m->buf);
            }
        }
    }
}


CGResultNode* constantNodeToResultNode(CGNode* node){
    switch(node->constant->type){
        case CGVT_DOUBLE:
        {
            CGDouble* d = (CGDouble*)node->constant->value;
            return makeDoubleResultNode(d->value);
        }
        case CGVT_VECTOR:
        {
            CGVector* v = (CGVector*)node->constant->value;
            if (v->loc == CG_DATALOC_HOST_MEM)
                return makeVectorResultNode(v->len, v->data);
            else
                return makeDeviceVectorResultNode(v->len, v->buf);
        }
        case CGVT_MATRIX:
        {
            CGMatrix* m = (CGMatrix*)node->constant->value;
            if (m->loc == CG_DATALOC_HOST_MEM)
                return makeMatrixResultNode(m->rows, m->cols, m->data);
            else
                return makeDeviceMatrixResultNode(m->rows, m->cols, m->buf);
        }
    }
}


CGResultNode* constantNodeToResultNodeCopy(CGNode* node){
    switch(node->constant->type){
        case CGVT_DOUBLE:
        {
            CGDouble* d = (CGDouble*)node->constant->value;
            return makeDoubleResultNode(d->value);
        }
        case CGVT_VECTOR:
        {
            CGVector* v = (CGVector*)node->constant->value;
            if (v->loc == CG_DATALOC_HOST_MEM)
                return makeVectorResultNode(v->len, cg_raw_copy(v->data, v->len));
            else
                return makeDeviceVectorResultNode(v->len, v->buf);
        }
        case CGVT_MATRIX:
        {
            CGMatrix* m = (CGMatrix*)node->constant->value;
            if (m->loc == CG_DATALOC_HOST_MEM)
                return makeMatrixResultNode(m->rows, m->cols, cg_raw_copy(m->data, m->rows*m->cols));
            else
                return makeDeviceMatrixResultNode(m->rows, m->cols, m->buf);
        }
    }
}


#endif