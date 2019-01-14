/*
 *
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include "Remotery.h"
#include "cg_api.h"

#define PROFILER_DEFINE
#include "profiler.h"
//
// host buffer I/O demo
//
#include "opencl_helper.h"

#define MAX_XOR_DATACOUNT (800*600)

OpenCL_Config gstConfig;

#ifndef MIN
#define MIN(a,b) (((a)<(b))?(a):(b))
#endif

#define M_ASSERT_CLSUCCESS(iRet, func_name) if (iRet != CL_SUCCESS) { \
	printf("%s error, iRet=%d: %s\n", func_name, iRet, GetCLErrString(iRet)); \
	return -__LINE__; \
}

static void PrintArrayBuffer(cl_double *buffer, cl_int num)
{
    cl_int i;

    for (i = 0; i < 1024; i++)
    {
        printf("%f\n", buffer[i]);
    }
    printf("\n");
}

int main__(int argc, char ** argv)
{
    profiler_initialize();
    PROFILER_START(cl_nn);
    OpenCL_Config *pstConfig = &gstConfig;

    cl_int i, iRet;
    cl_kernel kernel;

    cl_uint value = 0;
    cl_double *input = NULL;				// host input buffer ptr
    cl_double *output = NULL;				// host output buffer ptr
    cl_mem inputBuffer;
    cl_mem outputBuffer;

    size_t work_size = 0;
    cl_event kernel_ready;
    cl_event buffer_ready;

    const char *kernel_name = "/Users/praisethemoon/projects/cgraph/source/clgraph/kernels/sigmoid.cl";

    // Init OpenCL with default GPU device
    iRet = clInitOpenCLByDefault(pstConfig, CL_DEVICE_TYPE_CPU);
    M_ASSERT_CLSUCCESS(iRet, "clInitOpenCLByDefault()");

    iRet = clBuildProgramFromSource(pstConfig, kernel_name);
    M_ASSERT_CLSUCCESS(iRet, "clBuildProgramFromSource()");

    // load kernel procedure
    kernel = clCreateKernel( pstConfig->program, "sigmoid_vec", NULL );

    ///
    ///  malloc i/o buffer
    ///
    // 1. malloc host buffer
    input = (cl_double*) malloc( MAX_XOR_DATACOUNT*sizeof(cl_double) );
    output = (cl_double*) malloc( MAX_XOR_DATACOUNT*sizeof(cl_double) );
    if ((input == NULL) || (output == NULL))
    {
        printf("malloc() host i/o buffer error, input=%p output=%p\n", input, output);
        return -11;
    }

    // Init input buffer
    for (i = 0; i < MAX_XOR_DATACOUNT; i++)
    {
        input[i] = 0;
    }

    //printf("--- INPUT ---\n");
    //PrintArrayBuffer(input, 32);

    // 2. alloc OpenCL buffer
    inputBuffer = clCreateBuffer(pstConfig->context, CL_MEM_READ_WRITE|CL_MEM_USE_HOST_PTR, MAX_XOR_DATACOUNT*sizeof(cl_double), input, &iRet);
    M_ASSERT_CLSUCCESS(iRet, "clCreateBuffer(inputBuffer)");

    outputBuffer = clCreateBuffer(pstConfig->context, CL_MEM_READ_WRITE|CL_MEM_USE_HOST_PTR, MAX_XOR_DATACOUNT*sizeof(cl_double), output, &iRet);
    M_ASSERT_CLSUCCESS(iRet, "clCreateBuffer(outputBuffer)");

    // 3. set kernel args
    // i/o buffer
    iRet = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&inputBuffer);
    M_ASSERT_CLSUCCESS(iRet, "clSetKernelArg(inputBuffer)");
    iRet = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&outputBuffer);
    M_ASSERT_CLSUCCESS(iRet, "clSetKernelArg(outputBuffer)");


    // 4. run kernel and wait event
    work_size = MAX_XOR_DATACOUNT;
    iRet = clEnqueueNDRangeKernel(pstConfig->queue, kernel, 1, NULL, &work_size, NULL, 0, NULL, &kernel_ready);
    M_ASSERT_CLSUCCESS(iRet, "clEnqueueNDRangeKernel()");

    // wait for the kernel call to finish execution
    iRet = clWaitAndReleaseOneEvent(kernel_ready);
    M_ASSERT_CLSUCCESS(iRet, "clWaitAndReleaseOneEvent()");


    // 5. read output to host
    iRet = clEnqueueReadBuffer(pstConfig->queue, outputBuffer, CL_TRUE, 0, MAX_XOR_DATACOUNT*sizeof(cl_double), output, 0, NULL, &buffer_ready);
    M_ASSERT_CLSUCCESS(iRet, "clEnqueueReadBuffer()");

    // Wait for the read buffer to finish execution
    iRet = clWaitAndReleaseOneEvent(buffer_ready);
    M_ASSERT_CLSUCCESS(iRet, "clWaitAndReleaseOneEvent()");

    //printf("\n--- OUTPUT ---\n");
    //PrintArrayBuffer(output, 32);

    PROFILER_STOP(cl_nn);
    profiler_dump_console();
    return clFreeOpenCLResources(pstConfig);
}
