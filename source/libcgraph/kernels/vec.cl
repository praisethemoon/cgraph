
#pragma OPENCL EXTENSION cl_khr_fp64 : enable

/*
__kernel void sigmoid_vec(__global double *input, __global double *output)
{
    uint idx = get_global_id(0);
    double x = input[idx];
    output[idx] = 1.0/(1.0 + exp(-x));
}
 */

__kernel void mul_vd(__global const double *a, const double b, __global double * c  ) {
    uint gid = get_global_id(0);
    c[gid] = a[gid] * b;
}

/*
 * Broadcast Matrix-vector multiplication
 */
__kernel void mul_mv(__global const double *M, __global const double *V, __global double * c, ulong len) {
    uint gid = get_global_id(0);
    c[gid] = M[gid] * V[gid%len];
}

/*
 * Broadcast Matrix-Matrix multiplication
 */
__kernel void mul_mm(__global const double *a, __global const double *b, __global double * c) {
    uint gid = get_global_id(0);
    c[gid] = a[gid] * b[gid];
}
