cmake_minimum_required(VERSION 3.10)
project(canalize_native)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Force MSVC to read source files as UTF-8, suppresses C4819 warning
if(MSVC)
    add_compile_options(/utf-8)
endif()

# AVX2 + optimization flags
if(MSVC)
    add_compile_options(/O2 /arch:AVX2)
else()
    add_compile_options(-O3 -mavx2)
endif()

# Find JNI
find_package(JNI REQUIRED)

include_directories(${JNI_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR}/include)

# Create shared library
add_library(canalize_native SHARED
    jni.cpp
    src/TerrainGen.cpp
    src/Carver.cpp
    src/Decorator.cpp
    src/WorldLoader.cpp
)

# Output directory for the library
set_target_properties(canalize_native PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/../../build/libs"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/../../build/libs"
)
