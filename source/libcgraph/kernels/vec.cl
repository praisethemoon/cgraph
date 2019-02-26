
#pragma OPENCL EXTENSION cl_khr_fp64 : enable

#ifndef cg_float
#define cg_float float
#endif

typedef cg_float(*ActivationFunc)(cg_float);

cg_float relu_activ(cg_float x){
    return  x*(x>0);
}

cg_float sigmoid_activ(cg_float x){
    return 1/(1+exp(-x));
}

cg_float softplus_activ(cg_float x){
    return log(1 + exp(x));
}

cg_float tanh_activ(cg_float x){
    return tanh(x);
}

/*
static ActivationFunc[]={
    relu_activ,
    sigmoid_activ,
};
*/
__kernel void sigmoid_vec(ulong size, __global cg_float *input, __global cg_float *output)
{
    uint idx = get_global_id(0);

    if(idx >= size){
        return;
    }

    cg_float x = input[idx];
    output[idx] = 1.0/(1.0 + exp(-x));
}

/*
 * Broadcast Matrix-vector multiplication
 */
__kernel void mul_mv(ulong size, __global const cg_float *M, __global const cg_float *V, __global cg_float * c, ulong len) {
    uint gid = get_global_id(0);

    if(gid >= size){
        return;
    }
    //printf("%lu %f*%f = %f\n", gid, M[gid], V[gid%len], M[gid] * V[gid%len]);
    c[gid] = M[gid] * V[gid%len];
}

/*
 * Broadcast Matrix-Matrix / vector-vector multiplication
 */
__kernel void mul_vv(ulong size, __global const cg_float *a, __global const cg_float *b, __global cg_float * c) {
    uint gid = get_global_id(0);

    if(gid >= size){
        return;
    }

    c[gid] = a[gid] * b[gid];
}

/*
 * Broadcast Matrix-Matrix / vector-vector division
 */
__kernel void div_vv(ulong size, __global const cg_float *a, __global const cg_float *b, __global cg_float * c) {
    uint gid = get_global_id(0);

    if(gid >= size){
        return;
    }

    /* TODO: Handle division by zero

    if(b[gid] == 0){
        printf("[Warning] Division by zero detected.\n");
    }*/

    c[gid] = a[gid] / b[gid];
}

/*
 * Broadcast double-Matrix / double-vector division
 */
__kernel void div_dv(ulong size, cg_float a, __global const cg_float *b, __global cg_float * c) {
    uint gid = get_global_id(0);

    if(gid >= size){
        return;
    }

    /* TODO: Handle division by zero

    if(b[gid] == 0){
        printf("[Warning] Division by zero detected.\n");
    }*/

    c[gid] = a / b[gid];
}


/*
 * Broadcast Matrix-vector multiplication
 */
__kernel void div_mv(ulong size, __global const cg_float *M, __global const cg_float *V, __global cg_float * c, ulong len) {
    uint gid = get_global_id(0);

    if(gid >= size){
        return;
    }

    // TODO: Handle division by zero
    c[gid] = M[gid] / V[gid%len];
}


/*
 * Broadcast double-vector addition
 */
__kernel void add_dv(ulong size, cg_float a, __global const cg_float *b, __global cg_float * c) {
    uint gid = get_global_id(0);

    if(gid >= size){
        return;
    }

    c[gid] = a + b[gid];
}


/*
 * Broadcast Matrix-Matrix / vector-vector addition
 */
__kernel void add_vv(ulong size, __global const cg_float *a, __global const cg_float *b, __global cg_float * c) {
    uint gid = get_global_id(0);

    if(gid >= size){
        return;
    }

    c[gid] = a[gid] + b[gid];
}


/*
 * Broadcast Matrix-vector multiplication
 */
__kernel void add_mv(ulong size, __global const cg_float *M, __global const cg_float *V, __global cg_float * c, ulong len) {
    uint gid = get_global_id(0);

    if(gid >= size){
        return;
    }

    c[gid] = M[gid] + V[gid%len];
}


/*
 * Broadcast double-vector substraction
 */
__kernel void sub_dv(ulong size, cg_float a, __global const cg_float *b, __global cg_float * c) {
    uint gid = get_global_id(0);

    if(gid >= size){
        return;
    }

    c[gid] = a - b[gid];
}

/*
 * Broadcast vector-double substraction
 */
__kernel void sub_vd(ulong size, cg_float a, __global const cg_float *b, __global cg_float * c) {
    uint gid = get_global_id(0);

    if(gid >= size){
        return;
    }

    c[gid] =b[gid] - a;
}

/*
 * Broadcast Matrix-Matrix / vector-vector substraction
 */
__kernel void sub_vv(ulong size, __global const cg_float *a, __global const cg_float *b, __global cg_float * c) {
    uint gid = get_global_id(0);

    if(gid >= size){
        return;
    }

    c[gid] = a[gid] - b[gid];
}


/*
 * Broadcast Matrix-vector substraction
 */
__kernel void sub_mv(ulong size, __global const cg_float *M, __global const cg_float *V, __global cg_float * c, ulong len) {
    uint gid = get_global_id(0);

    if(gid >= size){
        return;
    }

    c[gid] = M[gid] - V[gid%len];
}


/*
 * Broadcast Matrix-vector substraction
 */
__kernel void sub_vm(ulong size, __global const cg_float *M, __global const cg_float *V, __global cg_float * c, ulong len) {
    uint gid = get_global_id(0);

    if(gid >= size){
        return;
    }

    c[gid] =  V[gid%len] - M[gid];
}

/*
 * V^d
 */
__kernel void pow_vd(ulong size, cg_float a, __global const cg_float *b, __global cg_float * c) {
    uint gid = get_global_id(0);

    if(gid >= size){
        return;
    }
    c[gid] = pow(b[gid], a);
}

/*
 * exp(V)
 */
__kernel void exp_v(ulong size, __global const cg_float *b, __global cg_float * c) {
    uint gid = get_global_id(0);

    if(gid >= size){
        return;
    }

    c[gid] = exp(b[gid]);
}

/*
 * log(V)
 */
__kernel void log_v(ulong size, __global const cg_float *b, __global cg_float * c) {
    uint gid = get_global_id(0);

    if(gid >= size){
        return;
    }

    c[gid] = log(b[gid]);
}

/*
 * sin(V)
 */
__kernel void sin_v(ulong size, __global const cg_float *b, __global cg_float * c) {
    uint gid = get_global_id(0);

    if(gid >= size){
        return;
    }

    c[gid] = sin(b[gid]);
}


/*
 * cos(V)
 */
__kernel void cos_v(ulong size, __global const cg_float *b, __global cg_float * c) {
    uint gid = get_global_id(0);

    if(gid >= size){
        return;
    }

    c[gid] = cos(b[gid]);
}


/*
 * tan(V)
 */
__kernel void tan_v(ulong size, __global const cg_float *b, __global cg_float * c) {
    uint gid = get_global_id(0);

    if(gid >= size){
        return;
    }

    c[gid] = tan(b[gid]);
}


/*
 * tanh(V)
 */
__kernel void tanh_v(ulong size, __global const cg_float *b, __global cg_float * c) {
    uint gid = get_global_id(0);

    if(gid >= size){
        return;
    }

    c[gid] = tanh(b[gid]);
}



/*
 * relu(V)
 */
__kernel void relu_v(ulong size, __global const cg_float *b, __global cg_float * c) {
    uint gid = get_global_id(0);

    if(gid >= size){
        return;
    }

    c[gid] = relu_activ(b[gid]);
}



/*
 * softplus(V)
 */
__kernel void softplus_v(ulong size, __global const cg_float *b, __global cg_float * c) {
    uint gid = get_global_id(0);

    if(gid >= size){
        return;
    }

    c[gid] = softplus_activ(b[gid]);
}


/*
 * sumMatrixRows(M)
 * @param size: total matrix elements
 * @param cols: matrix cols which is also size of vector c.
 */
__kernel void sum_matrix_rows(ulong size, ulong cols, __global const cg_float *M, __global cg_float * Y) {
    uint gid = get_global_id(0);

    if(gid >= size){
        return;
    }

    //printf("%lu, idx: %lu\n", gid, gid%cols);

    Y[gid%cols] += M[gid];
}

/*
 * sumMatrixCols(M)
 * @param size: total matrix elements
 * @param cols: matrix cols which is also size of vector c.
 */
__kernel void sum_matrix_cols(ulong size, ulong cols, __global const cg_float *b, __global cg_float * c) {
    uint gid = get_global_id(0);

    if(gid >= size){
        return;
    }

    c[gid/cols] += b[gid];
}