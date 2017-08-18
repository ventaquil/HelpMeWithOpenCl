#include "utils.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <list>

//void clAssert(cl_int error_code) {
//    if (error_code != CL_SUCCESS) {
//        std::cerr << "Error code " << error_code << " [" << clGetErrorString(error_code) << "]" << std::endl;
//    }
//
//    assert(error_code == CL_SUCCESS);
//}

cl_device_id *clGetDevices(cl_platform_id platform_id, cl_device_type device_type, cl_uint count) {
    if (count == 0) {
        count = clGetDevicesCount(platform_id, device_type);
    }

    cl_device_id *devices_ids = new cl_device_id[count];

    clAssert(clGetDeviceIDs(platform_id, device_type, count, devices_ids, NULL));

    return devices_ids;
}

cl_uint clGetDevicesCount(cl_platform_id platform_id, cl_device_type device_type) {
    cl_uint devices_count = 0;

    clAssert(clGetDeviceIDs(platform_id, device_type, 0, NULL, &devices_count));

    return devices_count;
}

const char *clGetErrorString(cl_int error_code) {
    switch (error_code) {
        // run-time and JIT compiler errors
        case CL_SUCCESS:
            return "CL_SUCCESS";
        case CL_DEVICE_NOT_FOUND:
            return "CL_DEVICE_NOT_FOUND";
        case CL_DEVICE_NOT_AVAILABLE:
            return "CL_DEVICE_NOT_AVAILABLE";
        case CL_COMPILER_NOT_AVAILABLE:
            return "CL_COMPILER_NOT_AVAILABLE";
        case CL_MEM_OBJECT_ALLOCATION_FAILURE:
            return "CL_MEM_OBJECT_ALLOCATION_FAILURE";
        case CL_OUT_OF_RESOURCES:
            return "CL_OUT_OF_RESOURCES";
        case CL_OUT_OF_HOST_MEMORY:
            return "CL_OUT_OF_HOST_MEMORY";
        case CL_PROFILING_INFO_NOT_AVAILABLE:
            return "CL_PROFILING_INFO_NOT_AVAILABLE";
        case CL_MEM_COPY_OVERLAP:
            return "CL_MEM_COPY_OVERLAP";
        case CL_IMAGE_FORMAT_MISMATCH:
            return "CL_IMAGE_FORMAT_MISMATCH";
        case CL_IMAGE_FORMAT_NOT_SUPPORTED:
            return "CL_IMAGE_FORMAT_NOT_SUPPORTED";
        case CL_BUILD_PROGRAM_FAILURE:
            return "CL_BUILD_PROGRAM_FAILURE";
        case CL_MAP_FAILURE:
            return "CL_MAP_FAILURE";
        case CL_MISALIGNED_SUB_BUFFER_OFFSET:
            return "CL_MISALIGNED_SUB_BUFFER_OFFSET";
        case CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST:
            return "CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST";
        case CL_COMPILE_PROGRAM_FAILURE:
            return "CL_COMPILE_PROGRAM_FAILURE";
        case CL_LINKER_NOT_AVAILABLE:
            return "CL_LINKER_NOT_AVAILABLE";
        case CL_LINK_PROGRAM_FAILURE:
            return "CL_LINK_PROGRAM_FAILURE";
        case CL_DEVICE_PARTITION_FAILED:
            return "CL_DEVICE_PARTITION_FAILED";
        case CL_KERNEL_ARG_INFO_NOT_AVAILABLE:
            return "CL_KERNEL_ARG_INFO_NOT_AVAILABLE";

        // compile-time errors
        case CL_INVALID_VALUE:
            return "CL_INVALID_VALUE";
        case CL_INVALID_DEVICE_TYPE:
            return "CL_INVALID_DEVICE_TYPE";
        case CL_INVALID_PLATFORM:
            return "CL_INVALID_PLATFORM";
        case CL_INVALID_DEVICE:
            return "CL_INVALID_DEVICE";
        case CL_INVALID_CONTEXT:
            return "CL_INVALID_CONTEXT";
        case CL_INVALID_QUEUE_PROPERTIES:
            return "CL_INVALID_QUEUE_PROPERTIES";
        case CL_INVALID_COMMAND_QUEUE:
            return "CL_INVALID_COMMAND_QUEUE";
        case CL_INVALID_HOST_PTR:
            return "CL_INVALID_HOST_PTR";
        case CL_INVALID_MEM_OBJECT:
            return "CL_INVALID_MEM_OBJECT";
        case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:
            return "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR";
        case CL_INVALID_IMAGE_SIZE:
            return "CL_INVALID_IMAGE_SIZE";
        case CL_INVALID_SAMPLER:
            return "CL_INVALID_SAMPLER";
        case CL_INVALID_BINARY:
            return "CL_INVALID_BINARY";
        case CL_INVALID_BUILD_OPTIONS:
            return "CL_INVALID_BUILD_OPTIONS";
        case CL_INVALID_PROGRAM:
            return "CL_INVALID_PROGRAM";
        case CL_INVALID_PROGRAM_EXECUTABLE:
            return "CL_INVALID_PROGRAM_EXECUTABLE";
        case CL_INVALID_KERNEL_NAME:
            return "CL_INVALID_KERNEL_NAME";
        case CL_INVALID_KERNEL_DEFINITION:
            return "CL_INVALID_KERNEL_DEFINITION";
        case CL_INVALID_KERNEL:
            return "CL_INVALID_KERNEL";
        case CL_INVALID_ARG_INDEX:
            return "CL_INVALID_ARG_INDEX";
        case CL_INVALID_ARG_VALUE:
            return "CL_INVALID_ARG_VALUE";
        case CL_INVALID_ARG_SIZE:
            return "CL_INVALID_ARG_SIZE";
        case CL_INVALID_KERNEL_ARGS:
            return "CL_INVALID_KERNEL_ARGS";
        case CL_INVALID_WORK_DIMENSION:
            return "CL_INVALID_WORK_DIMENSION";
        case CL_INVALID_WORK_GROUP_SIZE:
            return "CL_INVALID_WORK_GROUP_SIZE";
        case CL_INVALID_WORK_ITEM_SIZE:
            return "CL_INVALID_WORK_ITEM_SIZE";
        case CL_INVALID_GLOBAL_OFFSET:
            return "CL_INVALID_GLOBAL_OFFSET";
        case CL_INVALID_EVENT_WAIT_LIST:
            return "CL_INVALID_EVENT_WAIT_LIST";
        case CL_INVALID_EVENT:
            return "CL_INVALID_EVENT";
        case CL_INVALID_OPERATION:
            return "CL_INVALID_OPERATION";
        case CL_INVALID_GL_OBJECT:
            return "CL_INVALID_GL_OBJECT";
        case CL_INVALID_BUFFER_SIZE:
            return "CL_INVALID_BUFFER_SIZE";
        case CL_INVALID_MIP_LEVEL:
            return "CL_INVALID_MIP_LEVEL";
        case CL_INVALID_GLOBAL_WORK_SIZE:
            return "CL_INVALID_GLOBAL_WORK_SIZE";
        case CL_INVALID_PROPERTY:
            return "CL_INVALID_PROPERTY";
        case CL_INVALID_IMAGE_DESCRIPTOR:
            return "CL_INVALID_IMAGE_DESCRIPTOR";
        case CL_INVALID_COMPILER_OPTIONS:
            return "CL_INVALID_COMPILER_OPTIONS";
        case CL_INVALID_LINKER_OPTIONS:
            return "CL_INVALID_LINKER_OPTIONS";
        case CL_INVALID_DEVICE_PARTITION_COUNT:
            return "CL_INVALID_DEVICE_PARTITION_COUNT";

        // extension errors
        #ifdef CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR
            case CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR:
                return "CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR";
        #endif
        #ifdef CL_PLATFORM_NOT_FOUND_KHR
            case CL_PLATFORM_NOT_FOUND_KHR:
                return "CL_PLATFORM_NOT_FOUND_KHR";
        #endif
        #ifdef CL_INVALID_D3D10_DEVICE_KHR
            case CL_INVALID_D3D10_DEVICE_KHR:
                return "CL_INVALID_D3D10_DEVICE_KHR";
        #endif
        #ifdef CL_INVALID_D3D10_RESOURCE_KHR
            case CL_INVALID_D3D10_RESOURCE_KHR:
                return "CL_INVALID_D3D10_RESOURCE_KHR";
        #endif
        #ifdef CL_D3D10_RESOURCE_ALREADY_ACQUIRED_KHR
            case CL_D3D10_RESOURCE_ALREADY_ACQUIRED_KHR:
                return "CL_D3D10_RESOURCE_ALREADY_ACQUIRED_KHR";
        #endif
        #ifdef CL_D3D10_RESOURCE_NOT_ACQUIRED_KHR
            case CL_D3D10_RESOURCE_NOT_ACQUIRED_KHR:
                return "CL_D3D10_RESOURCE_NOT_ACQUIRED_KHR";
        #endif

        default:
            return "Unknown OpenCL error";
    }
}

size_t *clGetKernelLocalWorkSize(cl_kernel kernel, cl_device_id device_id, cl_uint dimensions, size_t problem_size) {
    cl_uint maximum_dimension = 3;

    clAssert(clGetDeviceInfo(device_id, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(cl_uint), &maximum_dimension, NULL));

    assert((1 <= dimensions) || (dimensions <= maximum_dimension));

    size_t *work_size = new size_t[maximum_dimension];

    size_t product = 1;

    {
        clAssert(clGetDeviceInfo(device_id, CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(size_t) * maximum_dimension,
                                 work_size, NULL));

        for (size_t i = 0; i < dimensions; ++i) {
            work_size[i] = std::__gcd(problem_size, work_size[i]);

            product *= work_size[i];
        }

        for (size_t i = dimensions; i < maximum_dimension; ++i) {
            work_size[i] = 1;
        }
    }

    {
        size_t preferred_size = clGetKernelPreferredWorkGroupSize(kernel, device_id);

        if (product > preferred_size) {
            std::list<size_t> primes;

            for (size_t i = 2, t = preferred_size; (i <= preferred_size) || (t > 1); ++i) {
                while ((t % i) == 0) {
                    primes.push_back(i);

                    t /= i;
                }
            }

            while (primes.size() < dimensions) {
                primes.push_back(1);
            }

            primes.sort(std::greater<int>()); // Sort descending

            {
                size_t *t = new size_t[dimensions];
                for (size_t i = 0; i < dimensions; ++i) {
                    t[i] = 1;
                }

                {
                    size_t i = 0;
                    while (!primes.empty()) {
                        t[i++] *= primes.front();

                        primes.pop_front();

                        i %= dimensions;
                    }
                }

                for (size_t i = 0; i < dimensions; ++i) {
                    work_size[i] = std::__gcd(work_size[i], t[i]);
                }

                delete[] t;
            }
        }
    }

    return work_size;
}

size_t clGetKernelPreferredWorkGroupSize(cl_kernel kernel, cl_device_id device_id) {
    size_t kernel_preferred_work_group_size = 0;

    clAssert(clGetKernelWorkGroupInfo(kernel, device_id, CL_KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE, sizeof(size_t),
                                      &kernel_preferred_work_group_size, NULL));

    return kernel_preferred_work_group_size;
}

cl_platform_id *clGetPlatforms(cl_uint count) {
    if (count == 0) {
        count = clGetPlatformsCount();
    }

    cl_platform_id *platforms_ids = new cl_platform_id[count];

    clAssert(clGetPlatformIDs(count, platforms_ids, NULL));

    return platforms_ids;
}

cl_uint clGetPlatformsCount(void) {
    cl_uint platforms_count = 0;

    clAssert(clGetPlatformIDs(0, NULL, &platforms_count));

    return platforms_count;
}

char *clLoadKernelFile(char *filename, size_t *size) {
    return clLoadKernelFile((const char *) filename, size);
}

char *clLoadKernelFile(const char *filename, size_t *size) {
    std::ifstream file(filename, std::ios::in | std::ios::binary);

    if (!file) {
        std::cerr << "Kernel file " << filename << " not found";

        throw new std::exception;
    }

    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    *size = content.size() + 1;

    char *output = new char[*size];
    copy(content.begin(), content.end(), output);
    output[(*size) - 1] = '\0'; // End of string

    return output;
}

char *clLoadKernelFile(std::string filename, size_t *size) {
    return clLoadKernelFile(filename.c_str(), size);
}
