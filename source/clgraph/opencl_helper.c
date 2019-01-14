
#ifdef __APPLE__
#include <OpenCL/cl.h>
#else
#include <CL/cl.h>
#endif

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "opencl_helper.h"

 cl_uint getdeviceinfo_cl_uint(cl_device_id device, cl_device_info param_name)
{
    cl_uint value;
    clGetDeviceInfo(device, param_name, sizeof(value), &value, NULL);
    return value;
}

// device_type = CL_DEVICE_TYPE_GPU or CL_DEVICE_TYPE_CPU
cl_int clInitOpenCLByDefault(OpenCL_Config *pstConfig, cl_device_type device_type)
{
    if (pstConfig == NULL)
        return -1;

    cl_int iRet;

    // 1. Get default platform.
    iRet = clGetPlatformIDs( 1, &(pstConfig->platform), NULL );
    if (iRet != 0)
    {
        printf("[ERROR] clGetPlatformIDs(1) error, iRet=%d\n", iRet);
        return iRet;
    }

    // 2. Find default device.
    iRet = clGetDeviceIDs( pstConfig->platform, device_type, 1, &(pstConfig->device), NULL);
    if (iRet != 0)
    {
        printf("[ERROR] clGetDeviceIDs(type=0x%x, 1) error, iRet=%d\n", device_type, iRet);
        return iRet;
    }

    pstConfig->ulGroupSize = getdeviceinfo_cl_uint(pstConfig->device, CL_DEVICE_MAX_WORK_GROUP_SIZE);
    pstConfig->ulItemDimension = getdeviceinfo_cl_uint(pstConfig->device, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS);
    pstConfig->ulItemSize = getdeviceinfo_cl_uint(pstConfig->device, CL_DEVICE_MAX_WORK_ITEM_SIZES);

    // 3. Create a context and command queue on that device.
    pstConfig->context = clCreateContext( NULL, 1, &(pstConfig->device), NULL, NULL, NULL);
    pstConfig->queue = clCreateCommandQueue( pstConfig->context, pstConfig->device, 0, NULL );
    return CL_SUCCESS;
}

cl_int clFreeOpenCLResources(OpenCL_Config *pstConfig)
{
    if (pstConfig == NULL)
        return -1;

    // release all without error check
    clReleaseProgram(pstConfig->program);
    clReleaseCommandQueue(pstConfig->queue);
    clReleaseContext(pstConfig->context);
    return CL_SUCCESS;
}

// source_name = xxx_kernel.cl
cl_int clBuildProgramFromSource(OpenCL_Config *pstConfig, const char *source_name)
{
    if (pstConfig == NULL)
        return -1;

    char *source = NULL;
    FILE *pf;
    size_t lSize;
    cl_int status;

    // open and get source length
    pf = fopen(source_name, "rb");
    if (pf == NULL)
    {
        printf("[ERROR] fopen(%s) error: %s\n", source_name, ERRSTR);
        return -100;
    }
    fseek(pf, 0, SEEK_END);
    lSize = ftell(pf);

    // load program from file
    source = (char*) malloc(lSize+1);
    fseek(pf, 0, SEEK_SET);
    fread(source, sizeof(char), lSize, pf);
    source[lSize] = '\0';


    // 4. Perform runtime source compilation, and obtain kernel entry point.
    pstConfig->program = clCreateProgramWithSource( pstConfig->context, 1, (const char**)&source, NULL, NULL );
    status = clBuildProgram( pstConfig->program, 1, &(pstConfig->device), NULL, NULL, NULL );
    if (status != CL_SUCCESS)
    {
        printf("[ERROR] clBuildProgram() error, status=%d\n", status);
        clGetProgramBuildInfo( pstConfig->program, pstConfig->device, CL_PROGRAM_BUILD_LOG,
                               sizeof(pstConfig->error_msg), pstConfig->error_msg, &lSize);
        printf("--------------------------------------------------------------------------------------------\n");
        printf("%s\n", pstConfig->error_msg);
        printf("--------------------------------------------------------------------------------------------\n");

        free(source);
        return CL_BUILD_PROGRAM_FAILURE;
    }

    free(source);
    return CL_SUCCESS;
}

 cl_int clWaitAndReleaseOneEvent(cl_event event)
{
    if (event == NULL)
        return CL_INVALID_EVENT;

    cl_int status;
    status = clWaitForEvents(1, &event);
    if (status != CL_SUCCESS)
        return status;

    status = clReleaseEvent(event);
    return status;
}

const char *GetCLErrString(cl_int error)
{
    switch(error)
    {
        case(CL_SUCCESS):						   return "Success";
        case(CL_DEVICE_NOT_FOUND):				  return "Device not found!";
        case(CL_DEVICE_NOT_AVAILABLE):			  return "Device not available!";
        case(CL_MEM_OBJECT_ALLOCATION_FAILURE):	 return "Memory object allocation failure!";
        case(CL_OUT_OF_RESOURCES):				  return "Out of resources!";
        case(CL_OUT_OF_HOST_MEMORY):				return "Out of host memory!";
        case(CL_PROFILING_INFO_NOT_AVAILABLE):	  return "Profiling information not available!";
        case(CL_MEM_COPY_OVERLAP):				  return "Overlap detected in memory copy operation!";
        case(CL_IMAGE_FORMAT_MISMATCH):			 return "Image format mismatch detected!";
        case(CL_IMAGE_FORMAT_NOT_SUPPORTED):		return "Image format not supported!";
        case(CL_INVALID_VALUE):					 return "Invalid value!";
        case(CL_INVALID_DEVICE_TYPE):			   return "Invalid device type!";
        case(CL_INVALID_DEVICE):					return "Invalid device!";
        case(CL_INVALID_CONTEXT):				   return "Invalid context!";
        case(CL_INVALID_QUEUE_PROPERTIES):		  return "Invalid queue properties!";
        case(CL_INVALID_COMMAND_QUEUE):			 return "Invalid command queue!";
        case(CL_INVALID_HOST_PTR):				  return "Invalid host pointer address!";
        case(CL_INVALID_MEM_OBJECT):				return "Invalid memory object!";
        case(CL_INVALID_IMAGE_FORMAT_DESCRIPTOR):   return "Invalid image format descriptor!";
        case(CL_INVALID_IMAGE_SIZE):				return "Invalid image size!";
        case(CL_INVALID_SAMPLER):				   return "Invalid sampler!";
        case(CL_INVALID_BINARY):					return "Invalid binary!";
        case(CL_INVALID_BUILD_OPTIONS):			 return "Invalid build options!";
        case(CL_INVALID_PROGRAM):				   return "Invalid program object!";
        case(CL_INVALID_PROGRAM_EXECUTABLE):		return "Invalid program executable!";
        case(CL_INVALID_KERNEL_NAME):			   return "Invalid kernel name!";
        case(CL_INVALID_KERNEL):					return "Invalid kernel object!";
        case(CL_INVALID_ARG_INDEX):				 return "Invalid index for kernel argument!";
        case(CL_INVALID_ARG_VALUE):				 return "Invalid value for kernel argument!";
        case(CL_INVALID_ARG_SIZE):				  return "Invalid size for kernel argument!";
        case(CL_INVALID_KERNEL_ARGS):			   return "Invalid kernel arguments!";
        case(CL_INVALID_WORK_DIMENSION):			return "Invalid work dimension!";
        case(CL_INVALID_WORK_GROUP_SIZE):		   return "Invalid work group size!";
        case(CL_INVALID_WORK_ITEM_SIZE):			return "Invalid work item size!";
        case(CL_INVALID_GLOBAL_OFFSET):			 return "Invalid global offset!";
        case(CL_INVALID_EVENT_WAIT_LIST):		   return "Invalid event wait list!";
        case(CL_INVALID_EVENT):					 return "Invalid event!";
        case(CL_INVALID_OPERATION):				 return "Invalid operation!";
        case(CL_INVALID_GL_OBJECT):				 return "Invalid OpenGL object!";
        case(CL_INVALID_BUFFER_SIZE):			   return "Invalid buffer size!";
        default:									return "Unknown error!";
    };

    return "Unknown error";
}
