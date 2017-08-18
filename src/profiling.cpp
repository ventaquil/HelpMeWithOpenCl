#include "profiling.h"

#include <algorithm>
#include <cmath>
#include <ctime>

cl_device_id clGetBestDeviceId(cl_device_type device_type, cl_uint problem_size) {
    cl_platform_profiling_data *platforms_profiling_data = clGetPlatformsProfilingData(device_type, problem_size);

    cl_uint platforms_count = clGetPlatformsCount();

    cl_device_id best_device_id;

    {
        cl_ulong min = CL_ULONG_MAX;

        for (cl_uint i = 0; i < platforms_count; ++i) {
            for (cl_uint j = 0; j < platforms_profiling_data[i].devices_count; ++j) {
                if (platforms_profiling_data[i].devices_profiling_data[j].time < min) {
                    min = platforms_profiling_data[i].devices_profiling_data[j].time;

                    best_device_id = platforms_profiling_data[i].devices_profiling_data[j].device_id;
                }
            }

            delete[] platforms_profiling_data[i].devices_profiling_data;
        }

        delete[] platforms_profiling_data;
    }

    return best_device_id;
}

cl_device_profiling_data clGetDeviceProfilingData(cl_device_id device_id, cl_context context, cl_program program, cl_uint problem_size) {
    cl_command_queue_properties command_queue_properties = 0; // No properties at start
    bool profiling_enabled; // Profiling disabled by default

    { // Get available command queue properties
        clAssert(clGetDeviceInfo(device_id, CL_DEVICE_QUEUE_PROPERTIES, sizeof(cl_command_queue_properties),
                                 &command_queue_properties, 0));

        command_queue_properties &= CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE | CL_QUEUE_PROFILING_ENABLE;

        profiling_enabled = ((command_queue_properties & CL_QUEUE_PROFILING_ENABLE) > 0);
    }

    cl_command_queue command_queue;

    { // Create command queue
        cl_int error_code = CL_SUCCESS;

        command_queue = clCreateCommandQueue(context, device_id, command_queue_properties, &error_code);

        clAssert(error_code);
    }

    cl_uint problem_size_2 = problem_size * problem_size;

    float *A, *B, *C, *seeds;

    { // Create arrays for data
        A = new float[problem_size_2];

        B = new float[problem_size_2];

        C = new float[problem_size_2];

        seeds = new float[problem_size_2];

        srand((unsigned int) time(NULL));

        for (size_t i = 0; i < problem_size; ++i) { // Set initial values
            for (size_t j = 0; j < problem_size; ++j) {
                size_t index = (i * problem_size) + j;
                A[index] = B[index] = C[index] = 0.0;

                seeds[index] = rand();
            }
        }
    }

    size_t memory_size = problem_size_2 * sizeof(float);

    cl_kernel random_kernel;

    { // Initialize kernel
        cl_int error_code = CL_SUCCESS;

        random_kernel = clCreateKernel(program, "random", &error_code);

        clAssert(error_code);
    }

    cl_mem memA, memB, memC, memSeeds;

    { // Create memory objects
        {
            cl_int error_code = CL_SUCCESS;

            memA = clCreateBuffer(context, CL_MEM_READ_ONLY, memory_size, NULL, &error_code);

            clAssert(error_code);
        }

        {
            cl_int error_code = CL_SUCCESS;

            memB = clCreateBuffer(context, CL_MEM_READ_ONLY, memory_size, NULL, &error_code);

            clAssert(error_code);
        }

        {
            cl_int error_code = CL_SUCCESS;

            memSeeds = clCreateBuffer(context, CL_MEM_WRITE_ONLY, memory_size, NULL, &error_code);

            clAssert(error_code);
        }
    }

    { // Fill arrays using random values with kernels
        cl_mem objects[] = {memA, memB};

        float *arrays[] = {A, B};

        { // Set constant arguments
            clAssert(clSetKernelArg(random_kernel, 0, sizeof(cl_uint), &problem_size));

            clAssert(clSetKernelArg(random_kernel, 2, sizeof(cl_mem), &memSeeds));
        }

        { // Write initial seeds for randomize process
            clAssert(clEnqueueWriteBuffer(command_queue, memSeeds, CL_TRUE, 0, memory_size, seeds, 0, NULL, NULL));
        }

        for (size_t i = 0, j = (sizeof(objects) / sizeof(cl_mem)); i < j; ++i) {
            clAssert(clSetKernelArg(random_kernel, 1, sizeof(cl_mem), &objects[i])); // Set latest argument

            {
                size_t global_work_size[2] = {problem_size, problem_size};

                size_t *local_work_size = clGetKernelLocalWorkSize(random_kernel, device_id, 2, problem_size);

                { // Launch kernel
                    clAssert(clEnqueueNDRangeKernel(command_queue, random_kernel, 2, NULL, global_work_size,
                                                    local_work_size, 0, NULL, NULL));
                }

                delete[] local_work_size;
            }

            clAssert(clFinish(command_queue)); // Wait for results

            { // Read data from device
                clAssert(clEnqueueReadBuffer(command_queue, objects[i], CL_TRUE, 0, memory_size, arrays[i], 0, NULL,
                                             NULL));
            }
        }

        { // Flush memory
            delete[] seeds;

            clAssert(clReleaseMemObject(memSeeds));

            clAssert(clReleaseMemObject(memB));

            clAssert(clReleaseMemObject(memA));

            clAssert(clReleaseKernel(random_kernel));
        }
    }

    cl_kernel multiply_kernel;

    { // Initialize kernel
        cl_int error_code = CL_SUCCESS;

        multiply_kernel = clCreateKernel(program, "multiply", &error_code);

        clAssert(error_code);
    }

    { // Create memory objects
        {
            cl_int error_code = CL_SUCCESS;

            memA = clCreateBuffer(context, CL_MEM_WRITE_ONLY, memory_size, NULL, &error_code);

            clAssert(error_code);
        }

        {
            cl_int error_code = CL_SUCCESS;

            memB = clCreateBuffer(context, CL_MEM_WRITE_ONLY, memory_size, NULL, &error_code);

            clAssert(error_code);
        }

        {
            cl_int error_code = CL_SUCCESS;

            memC = clCreateBuffer(context, CL_MEM_READ_ONLY, memory_size, NULL, &error_code);

            clAssert(error_code);
        }
    }

    { // Set kernel arguments
        clAssert(clSetKernelArg(multiply_kernel, 0, sizeof(cl_uint), &problem_size));

        clAssert(clSetKernelArg(multiply_kernel, 1, sizeof(cl_mem), &memA));

        clAssert(clSetKernelArg(multiply_kernel, 2, sizeof(cl_mem), &memB));

        clAssert(clSetKernelArg(multiply_kernel, 3, sizeof(cl_mem), &memC));
    }

    { // Write needed data
        clAssert(clEnqueueWriteBuffer(command_queue, memA, CL_TRUE, 0, memory_size, A, 0, NULL, NULL));

        clAssert(clEnqueueWriteBuffer(command_queue, memB, CL_TRUE, 0, memory_size, B, 0, NULL, NULL));
    }

    cl_event event;

    {
        size_t global_work_size[2] = {problem_size, problem_size};

        size_t *local_work_size = clGetKernelLocalWorkSize(multiply_kernel, device_id, 2, problem_size);

        { // Launch kernel
            clAssert(clEnqueueNDRangeKernel(command_queue, multiply_kernel, 2, NULL, global_work_size, local_work_size,
                                            0, NULL, &event));
        }

        delete[] local_work_size;
    }

    clFinish(command_queue); // Wait for results

    cl_device_profiling_data device_profiling_data;
    device_profiling_data.device_id = device_id;
    device_profiling_data.ratio = 1.0;
    device_profiling_data.time = CL_ULONG_MAX;

    { // Collect profiling data
        cl_int status;

        clAssert(clGetEventInfo(event, CL_EVENT_COMMAND_EXECUTION_STATUS, sizeof(cl_int), &status, NULL));

        if ((status == CL_COMPLETE) && profiling_enabled) {
            cl_ulong start, end;

            clAssert(clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &start, NULL));

            clAssert(clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &end, NULL));

            device_profiling_data.time = end - start;
        }
    }

    clAssert(clEnqueueReadBuffer(command_queue, memC, CL_TRUE, 0, memory_size, C, 0, NULL, NULL));

    { // Flush memory
        clReleaseEvent(event);

        delete[] A, B, C;

        clReleaseMemObject(memC);

        clReleaseMemObject(memB);

        clReleaseMemObject(memA);

        clReleaseKernel(multiply_kernel);

        clFlush(command_queue);
    }

    return device_profiling_data;
}

cl_device_profiling_data *clGetDevicesProfilingData(cl_platform_id platform_id, cl_device_type device_type, cl_uint problem_size) {
    cl_uint devices_count = clGetDevicesCount(platform_id);

    cl_device_id *devices_ids = clGetDevices(platform_id, device_type, devices_count);

    cl_context context;

    { // Create context
        cl_int error_code = CL_SUCCESS;

        context = clCreateContext(NULL, devices_count, devices_ids, NULL, NULL, &error_code);

        clAssert(error_code);
    }

    cl_program program;

    { // Load kernel code and build program
        {
            size_t code_size = 0;

            char *code = clLoadKernelFile("resrc/matrix.cl", &code_size);

            {
                cl_int error_code = CL_SUCCESS;

                program = clCreateProgramWithSource(context, 1, (const char **) &code, (const size_t *) &code_size,
                                                    &error_code);

                clAssert(error_code);
            }

            delete[] code;
        }

        clAssert(clBuildProgram(program, devices_count, devices_ids, "-I resrc", NULL, NULL));
    }

    cl_device_profiling_data *devices_profiling_data = new cl_device_profiling_data[devices_count];

    { // Get profiling data for each device
        cl_ulong total_time = 0;

        for (cl_uint i = 0; i < devices_count; ++i) {
            devices_profiling_data[i] = clGetDeviceProfilingData(devices_ids[i], context, program, problem_size);

            total_time += devices_profiling_data[i].time;
        }

        std::sort(devices_profiling_data, devices_profiling_data + devices_count, // Sort in ascending order
                  [](cl_device_profiling_data a, cl_device_profiling_data b) { return a.time < b.time; });

        { // Calculate problem shares
            double sum = 0;

            for (cl_uint i = 0; i < devices_count; ++i) {
                double ratio = 1;

                if (i > 0) {
                    ratio = devices_profiling_data[i - 1].time /
                            (devices_profiling_data[i].time * devices_profiling_data[i - 1].ratio);
                }

                sum += devices_profiling_data[i].ratio = ratio;
            }

            for (cl_uint i = 0; i < devices_count; ++i) {
                devices_profiling_data[i].ratio /= sum;
            }
        }
    }

    { // Flush memory
        clReleaseProgram(program);

        clReleaseContext(context);

        delete[] devices_ids;
    }

    return devices_profiling_data;
}


cl_device_profiling_data *clGetDevicesProfilingData(cl_platform_id platform_id, cl_device_type device_type, cl_uint *problems_sizes) {
    cl_uint devices_count = clGetDevicesCount(platform_id);

    cl_device_id *devices_ids = clGetDevices(platform_id, device_type, devices_count);

    cl_context context;

    { // Create context
        cl_int error_code = CL_SUCCESS;

        context = clCreateContext(NULL, devices_count, devices_ids, NULL, NULL, &error_code);

        clAssert(error_code);
    }

    cl_program program;

    { // Load kernel code and build program
        {
            size_t code_size = 0;

            char *code = clLoadKernelFile("resrc/matrix.cl", &code_size);

            {
                cl_int error_code = CL_SUCCESS;

                program = clCreateProgramWithSource(context, 1, (const char **) &code, (const size_t *) &code_size,
                                                    &error_code);

                clAssert(error_code);
            }

            delete[] code;
        }

        clAssert(clBuildProgram(program, devices_count, devices_ids, "-I resrc", NULL, NULL));
    }

    cl_device_profiling_data *devices_profiling_data = new cl_device_profiling_data[devices_count];

    { // Get profiling data for each device
        for (cl_uint i = 0; i < devices_count; ++i) {
            devices_profiling_data[i] = clGetDeviceProfilingData(devices_ids[i], context, program, problems_sizes[i]);
        }

        std::sort(devices_profiling_data, devices_profiling_data + devices_count, // Sort in descending order
                  [](cl_device_profiling_data a, cl_device_profiling_data b) { return a.time > b.time; });
    }

    { // Flush memory
        clReleaseProgram(program);

        clReleaseContext(context);

        delete[] devices_ids;
    }

    return devices_profiling_data;
}

cl_platform_profiling_data clGetPlatformProfilingData(cl_platform_id platform_id, cl_device_type device_type, cl_uint problem_size) {
    cl_platform_profiling_data platform_profiling_data;
    platform_profiling_data.devices_count = clGetDevicesCount(platform_id);
    platform_profiling_data.devices_profiling_data = clGetDevicesProfilingData(platform_id, device_type, problem_size);
    platform_profiling_data.platform_id = platform_id;

    if (platform_profiling_data.devices_count > 1) {
        platform_profiling_data.time = 0;

        cl_uint problems_sizes[platform_profiling_data.devices_count];

        { // Prepare new problems
            cl_uint temporary = problem_size;

            for (cl_uint i = 0; i < platform_profiling_data.devices_count; ++i) {
                if (i == (platform_profiling_data.devices_count - 1)) {
                    problems_sizes[i] = temporary;
                } else {
                    temporary -= problems_sizes[i] = cl_uint(
                            floor(platform_profiling_data.devices_profiling_data[i].ratio * problem_size));
                }
            }
        }

        { // Get maximum value
            cl_device_profiling_data *devices_profiling_data = clGetDevicesProfilingData(platform_id, device_type,
                                                                                         problems_sizes);

            for (cl_uint i = 0; i < platform_profiling_data.devices_count; ++i) {
                if (platform_profiling_data.time < devices_profiling_data[i].time) {
                    platform_profiling_data.time = devices_profiling_data[i].time;
                }
            }

            delete[] devices_profiling_data;
        }
    } else {
        platform_profiling_data.time = platform_profiling_data.devices_profiling_data[0].time;
    }

    return platform_profiling_data;
}

cl_platform_profiling_data *clGetPlatformsProfilingData(cl_device_type device_type, cl_uint problem_size) {
    cl_uint platforms_count = clGetPlatformsCount();

    cl_platform_profiling_data *platforms_profiling_data = new cl_platform_profiling_data[platforms_count];

    { // Get profiling data for each platform
        cl_platform_id *platforms_ids = clGetPlatforms();

        for (cl_uint i = 0; i < platforms_count; ++i) {
            platforms_profiling_data[i] = clGetPlatformProfilingData(platforms_ids[i], device_type, problem_size);
        }

        delete[] platforms_ids;

        std::sort(platforms_profiling_data, platforms_profiling_data + platforms_count, // Sort in ascending order
                  [](cl_platform_profiling_data first, cl_platform_profiling_data second) {
                      return first.time < second.time;
                  });
    }

    return platforms_profiling_data;
}
