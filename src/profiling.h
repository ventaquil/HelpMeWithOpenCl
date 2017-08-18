#ifndef PBKDF2_HMAC_SHA_BREAKER_PROFILING_H
    #define PBKDF2_HMAC_SHA_BREAKER_PROFILING_H

    #include <CL/cl.h>
    #include <utils.h>

    struct cl_device_profiling_data {
        cl_device_id device_id;

        double ratio;

        cl_ulong time;
    };

    struct cl_platform_profiling_data {
        cl_uint devices_count;

        cl_device_profiling_data *devices_profiling_data;

        cl_platform_id platform_id;

        cl_ulong time;
    };

    cl_device_id clGetBestDeviceId(cl_device_type device_type = CL_DEVICE_TYPE_ALL, cl_uint problem_size = 64);

    cl_device_profiling_data clGetDeviceProfilingData(cl_device_id device_id, cl_context context, cl_program program, cl_uint problem_size = 64);

    cl_device_profiling_data *clGetDevicesProfilingData(cl_platform_id platform_id, cl_device_type device_type = CL_DEVICE_TYPE_ALL, cl_uint problem_size = 64);

    cl_device_profiling_data *clGetDevicesProfilingData(cl_platform_id platform_id, cl_device_type device_type = CL_DEVICE_TYPE_ALL, cl_uint *problems_sizes = {});

    cl_platform_profiling_data clGetPlatformProfilingData(cl_platform_id platform_id, cl_device_type device_type = CL_DEVICE_TYPE_ALL, cl_uint problem_size = 64);

    cl_platform_profiling_data *clGetPlatformsProfilingData(cl_device_type device_type = CL_DEVICE_TYPE_ALL, cl_uint problem_size = 64);
#endif // PBKDF2_HMAC_SHA_BREAKER_PROFILING_H
