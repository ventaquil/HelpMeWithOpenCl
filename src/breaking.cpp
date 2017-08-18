#include "breaking.h"

#include <iostream>

void breaker(Algorithm algorithm, char *salt, size_t salt_length, char *key, size_t key_length) {
    breaker(algorithm, (const char *) salt, salt_length, (const char *) key, key_length);
}

void breaker(Algorithm algorithm, const char *salt, size_t salt_length, const char *key, size_t key_length) {
    cl_uint platforms_count = clGetPlatformsCount();

    if (platforms_count > 0) {
        cl_platform_profiling_data *platforms_profiling_data = clGetPlatformsProfilingData();

        const char alphabet[] = "abcdefghijklmnopqrstuvwxyz0123456789!@#$%^&*()_+-={};:,./<>?\\";

        for (cl_uint i = 0; i < platforms_count; ++i) {
            for (cl_uint j = 0; j < platforms_profiling_data[i].devices_count; ++j) {
                cl_device_id device_id = platforms_profiling_data[i].devices_profiling_data[j].device_id;

                cl_context context;

                { // Create context
                    cl_int error_code = CL_SUCCESS;

                    context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &error_code);

                    clAssert(error_code);
                }

                cl_program program;

                { // Load kernel code and build program
//                    {
//                        size_t code_size = 0;
//
//                        char *code = clLoadKernelFile("resrc/breaker.cl", &code_size);
//
//                        {
//                            cl_int error_code = CL_SUCCESS;
//
//                            program = clCreateProgramWithSource(context, 1, (const char **) &code, (const size_t *) &code_size, &error_code);
//
//                            clAssert(error_code);
//                        }
//
//                        delete[] code;
//                    }
//
////                    size_t size = 0;
////
////                    clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, 0, NULL, &size);
////
////                    char *log = new char[size];
////
////                    clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, size, log, NULL);
////
////                    std::cout << log << std::endl;
////
////                    delete[] log;
//
//                    clAssert(clBuildProgram(program, 1, &device_id, "-I resrc", NULL, NULL));
                }

                { // Flush memory
//                    delete[] password;
//
//                    clAssert(clReleaseMemObject(key_mem));
//
//                    clAssert(clReleaseMemObject(salt_mem));
//
//                    clAssert(clReleaseMemObject(alphabet_mem));
//
//                    clAssert(clReleaseKernel(kernel));
//
//                    clAssert(clFlush(command_queue));
//
//                    clAssert(clReleaseProgram(program));

                    clAssert(clReleaseContext(context));
                }
            }
        }

        { // Flush memory
            for (cl_uint i = 0; i < platforms_count; ++i) {
                delete[] platforms_profiling_data[i].devices_profiling_data;
            }

            delete[] platforms_profiling_data;
        }
    } else {
        std::cout << "No OpenCL platforms detected" << std::endl;
    }
}
