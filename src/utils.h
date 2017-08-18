#ifndef PBKDF2_HMAC_SHA_BREAKER_UTILS_H
    #define PBKDF2_HMAC_SHA_BREAKER_UTILS_H

    #include <cassert>
    #include <CL/cl.h>
    #include <string>

    #define clAssert(error_code) assert(error_code == CL_SUCCESS)
    //void clAssert(cl_int error_code);

    cl_device_id *clGetDevices(cl_platform_id platform_id, cl_device_type device_type = CL_DEVICE_TYPE_ALL, cl_uint count = 0);

    cl_uint clGetDevicesCount(cl_platform_id platform_id, cl_device_type device_type = CL_DEVICE_TYPE_ALL);

    const char *clGetErrorString(cl_int error_code);

    size_t *clGetKernelLocalWorkSize(cl_kernel kernel, cl_device_id device_id, cl_uint dimensions, size_t problem_size);

    size_t clGetKernelPreferredWorkGroupSize(cl_kernel kernel, cl_device_id device_id);

    cl_platform_id *clGetPlatforms(cl_uint count = 0);

    cl_uint clGetPlatformsCount(void);

    char *clLoadKernelFile(char *filename, size_t *size);

    char *clLoadKernelFile(const char *filename, size_t *size);

    char *clLoadKernelFile(std::string filename, size_t *size);
#endif // PBKDF2_HMAC_SHA_BREAKER_UTILS_H
