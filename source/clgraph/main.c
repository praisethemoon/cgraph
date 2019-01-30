
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <cf4ocl2.h>
/* Error handling macros. */
#define ERROR_MSG_AND_EXIT(msg) \
    do { fprintf(stderr, "\n%s\n", msg); exit(EXIT_FAILURE); } while(0)
#define HANDLE_ERROR(err) \
    if (err != NULL) { ERROR_MSG_AND_EXIT(err->message); }
int main() {
    /* cf4ocl objects. */
    CCLPlatforms* platf_list;
    CCLPlatform* platf;
    CCLDevice* dev;
    /* Other variables. */
    cl_uint num_platfs;
    cl_uint num_devs;
    char* info;
    /* Error handling object. */
    CCLErr* err = NULL;
    /* Get all platforms in system. */
    platf_list = ccl_platforms_new(&err);
    HANDLE_ERROR(err);
    /* Get number of platforms in system. */
    num_platfs = ccl_platforms_count(platf_list);
    /* Cycle through platforms. */
    for (cl_uint i = 0; i < num_platfs; ++i) {
        /* Get current platform. */
        platf = ccl_platforms_get(platf_list, i);
        /* Get platform name. */
        info = ccl_platform_get_info_string(
                platf, CL_PLATFORM_NAME, &err);
        HANDLE_ERROR(err);
        /* Print platform name. */
        printf("Platform #%d: %s\n", i, info);
        /* Get number of devices in current platform. */
        num_devs = ccl_platform_get_num_devices(platf, &err);
        HANDLE_ERROR(err);
        /* Cycle through devices. */
        for (cl_uint j = 0; j < num_devs; ++j) {
            /* Get current device. */
            dev = ccl_platform_get_device(platf, j, &err);
            HANDLE_ERROR(err);
            /* Get device name. */
            info = ccl_device_get_info_array(
                    dev, CL_DEVICE_NAME, char*, &err);
            HANDLE_ERROR(err);
            /* Print device name. */
            printf("\tDevice #%d: %s\n", j, info);
        } /* Cycle devices. */
    } /* Cycle platforms. */
    /* Release platform set, which will release the underlying
     * platform wrappers, device wrappers and the requested info. */
    ccl_platforms_destroy(platf_list);
    /* Check all wrappers have been destroyed. */
    assert(ccl_wrapper_memcheck());
    /* Bye. */
    return EXIT_SUCCESS;
}