
#pragma OPENCL EXTENSION cl_khr_fp64 : enable

#ifndef CL_SCALAR_TYPE
#define CL_SCALAR_TYPE float
#endif

__kernel void sigmoid_vec(ulong size, __global CL_SCALAR_TYPE *input, __global CL_SCALAR_TYPE *output)
{
    uint idx = get_global_id(0);

    if(idx >= size){
        return;
    }

    CL_SCALAR_TYPE x = input[idx];
    output[idx] = 1.0/(1.0 + exp(-x));
}

__kernel void mul_vd(ulong size, __global const CL_SCALAR_TYPE *a, const CL_SCALAR_TYPE b, __global CL_SCALAR_TYPE * c  ) {
    uint gid = get_global_id(0);

    if(gid >= size){
        return;
    }

    c[gid] = a[gid] * b;
}

/*
 * Broadcast Matrix-vector multiplication
 */
__kernel void mul_mv(ulong size, __global const CL_SCALAR_TYPE *M, __global const CL_SCALAR_TYPE *V, __global CL_SCALAR_TYPE * c, ulong len) {
    uint gid = get_global_id(0);

    if(gid >= size){
        return;
    }

    c[gid] = M[gid] * V[gid%len];
}

/*
 * Broadcast Matrix-Matrix multiplication
 */
__kernel void mul_mm(ulong size, __global const CL_SCALAR_TYPE *a, __global const CL_SCALAR_TYPE *b, __global CL_SCALAR_TYPE * c) {
    uint gid = get_global_id(0);

    if(gid >= size){
        return;
    }

    c[gid] = a[gid] * b[gid];
}
