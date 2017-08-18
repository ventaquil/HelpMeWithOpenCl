#include "sha512.h"
#include <profiling.h>

char *sha512(char *message, cl_uint size) {
    return sha512((const char *) message, size);
}

char *sha512(const char *message, cl_uint size) {
    char *hash = new char[SHA512_OUTPUT_SIZE_IN_CHARACTERS + 1];

    for (unsigned int i = 0; i < (SHA512_OUTPUT_SIZE_IN_CHARACTERS  + 1); ++i) {
        hash[i] = '\0';
    }

    cl_device_id device_id = clGetBestDeviceId();

    cl_context context;

    { // Create context
        cl_int error_code = CL_SUCCESS;

        context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &error_code);

        clAssert(error_code);
    }

    cl_program program;

    { // Load kernel code and build program
        {
            size_t code_size = 0;

            char *code = clLoadKernelFile("resrc/sha512.cl", &code_size);

            {
                cl_int error_code = CL_SUCCESS;

                program = clCreateProgramWithSource(context, 1, (const char **) &code, (const size_t *) &code_size,
                                                    &error_code);

                clAssert(error_code);
            }

            delete[] code;
        }

        clAssert(clBuildProgram(program, 1, &device_id, "-I resrc", NULL, NULL));
    }

    cl_command_queue_properties command_queue_properties = 0; // No properties at start

    { // Get available command queue properties
        clAssert(clGetDeviceInfo(device_id, CL_DEVICE_QUEUE_PROPERTIES, sizeof(cl_command_queue_properties),
                                 &command_queue_properties, 0));

        command_queue_properties &= CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE;
    }

    cl_command_queue command_queue;

    { // Create command queue
        cl_int error_code = CL_SUCCESS;

        command_queue = clCreateCommandQueue(context, device_id, command_queue_properties, &error_code);

        clAssert(error_code);
    }

    cl_kernel kernel;

    { // Initialize kernel
        cl_int error_code = CL_SUCCESS;

        kernel = clCreateKernel(program, "_sha512", &error_code);

        clAssert(error_code);
    }

    cl_mem message_mem, hash_mem;

    {
        { // Create memory objects
            {
                cl_int error_code = CL_SUCCESS;

                message_mem = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(char) * size, NULL, &error_code);

                clAssert(error_code);
            }

            {
                cl_int error_code = CL_SUCCESS;

                hash_mem = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(char) * SHA512_OUTPUT_SIZE_IN_CHARACTERS,
                                          NULL, &error_code);

                clAssert(error_code);
            }
        }

        { // Set kernel arguments
            clAssert(clSetKernelArg(kernel, 0, sizeof(cl_mem), &message_mem));

            clAssert(clSetKernelArg(kernel, 1, sizeof(cl_uint), &size));

            clAssert(clSetKernelArg(kernel, 2, sizeof(cl_mem), &hash_mem));
        }

        { // Write initial data to buffers
            clAssert(clEnqueueWriteBuffer(command_queue, message_mem, CL_TRUE, 0, size, message, 0, NULL, NULL));

            clAssert(clFinish(command_queue)); // Wait for buffer write
        }

        size_t *globalWorkSize, *localWorkSize;

        { // Prepare global and local work size
            globalWorkSize = new size_t[1];
            globalWorkSize[0] = 1;

            localWorkSize = new size_t[1];
            localWorkSize[0] = 1;
        }

        { // Launch kernel
            clAssert(clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, globalWorkSize, localWorkSize, 0, NULL,
                                            NULL));
        }

        { // Flush memory
            delete[] localWorkSize;

            delete[] globalWorkSize;
        }

        clAssert(clFinish(command_queue)); // Wait for results

        clAssert(clEnqueueReadBuffer(command_queue, hash_mem, CL_TRUE, 0,
                                     sizeof(char) * SHA512_OUTPUT_SIZE_IN_CHARACTERS, hash, 0, NULL, NULL));

        clAssert(clFinish(command_queue)); // Wait for read
    }

    { // Flush memory
        clAssert(clReleaseMemObject(hash_mem));

        clAssert(clReleaseMemObject(message_mem));

        clAssert(clReleaseKernel(kernel));

        clAssert(clFlush(command_queue));

        clAssert(clReleaseProgram(program));

        clAssert(clReleaseContext(context));
    }

    return hash;
}
