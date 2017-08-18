cmake_minimum_required(VERSION 3.7)
project(PBKDF2-HMAC-SHA-BREAKER)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES main.cpp src/breaking.cpp src/hash/sha512.cpp src/profiling.cpp src/utils.cpp)

find_package(OpenCL REQUIRED)

include_directories(src)
include_directories(resrc)

add_executable(main ${SOURCE_FILES})

target_link_libraries(main OpenCL::OpenCL)

configure_file(resrc/sha384.cl resrc/sha384.cl @ONLY)
configure_file(resrc/sha384.h resrc/sha384.h @ONLY)
configure_file(resrc/sha512.cl resrc/sha512.cl @ONLY)
configure_file(resrc/sha512.h resrc/sha512.h @ONLY)
configure_file(resrc/types.h resrc/types.h @ONLY)
