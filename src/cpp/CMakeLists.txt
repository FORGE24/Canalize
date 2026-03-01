cmake_minimum_required(VERSION 3.10)
project(canalize_native)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find JNI
find_package(JNI REQUIRED)

include_directories(${JNI_INCLUDE_DIRS})

# Create shared library
add_library(canalize_native SHARED
    jni.cpp
    # Add other source files here
)

# Output directory for the library (optional, can be adjusted)
set_target_properties(canalize_native PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/../../build/libs"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/../../build/libs"
)
