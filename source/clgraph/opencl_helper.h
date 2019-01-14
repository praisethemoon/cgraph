#ifndef __CL_COMM_FUNC_H__
#define __CL_COMM_FUNC_H__

#ifdef __APPLE__
#include <OpenCL/cl.h>
#else
#include <CL/cl.h>
#endif

#include <stdio.h>
#include <string.h>
#include <errno.h>

// enable printf output in kernel
#pragma OPENCL EXTENSION cl_amd_printf : enable
#pragma OPENCL EXTENSION cl_khr_fp64 : enable

#ifndef ERRSTR
#define ERRSTR (strerror(errno))
#endif

typedef struct OpenCL_Config
{
    cl_platform_id platform;
    cl_device_id device;
    cl_context context;
    cl_command_queue queue;
    cl_program program;

    char error_msg[4096];

    // device info get by init function
    cl_uint ulGroupSize;			// CL_DEVICE_MAX_WORK_GROUP_SIZE
    cl_uint ulItemDimension;		// CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS
    cl_uint ulItemSize;				// CL_DEVICE_MAX_WORK_ITEM_SIZES
} OpenCL_Config;

 cl_uint getdeviceinfo_cl_uint(cl_device_id device, cl_device_info param_name);

// device_type = CL_DEVICE_TYPE_GPU or CL_DEVICE_TYPE_CPU
cl_int clInitOpenCLByDefault(OpenCL_Config *pstConfig, cl_device_type device_type);

cl_int clFreeOpenCLResources(OpenCL_Config *pstConfig);

// source_name = xxx_kernel.cl
cl_int clBuildProgramFromSource(OpenCL_Config *pstConfig, const char *source_name);

 cl_int clWaitAndReleaseOneEvent(cl_event event);

const char *GetCLErrString(cl_int error);

#endif
