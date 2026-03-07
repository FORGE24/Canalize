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
    add_compile_options(-O3)
    # Enable AVX2 only on x86_64; Apple Silicon (arm64) does not support it
    if(NOT (APPLE AND CMAKE_SYSTEM_PROCESSOR MATCHES "arm64"))
        add_compile_options(-mavx2)
    endif()
endif()

# macOS: allow JNI symbols to resolve at runtime
if(APPLE)
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -undefined dynamic_lookup")
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
    src/NativeStatus.cpp
    src/NativeLog.cpp
    src/PluginSystem.cpp
    src/PluginLoader.cpp
    src/JNIHelper.cpp
    src/BlockState.cpp
    src/ChunkAccess.cpp
    src/EventBus.cpp
    src/ItemStack.cpp
    src/Items.cpp
    src/Entity.cpp
    src/EntityType.cpp
    src/Config.cpp
    src/GUI.cpp
    src/Network.cpp
    src/Render.cpp
)

target_compile_definitions(canalize_native PRIVATE CANALIZE_EXPORTS)

# Output directory for the library
set_target_properties(canalize_native PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/../../build/libs"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/../../build/libs"
)

# Install Configuration
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# Install Targets
install(TARGETS canalize_native
    EXPORT CanalizeTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Install Headers
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/canalize
    FILES_MATCHING PATTERN "*.h"
)

# Generate Config Files
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/CanalizeSDKConfigVersion.cmake"
    VERSION 1.0.0
    COMPATIBILITY AnyNewerVersion
)

configure_file(CanalizeSDKConfig.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/CanalizeSDKConfig.cmake"
    @ONLY
)

# Install Config Files
install(EXPORT CanalizeTargets
    FILE CanalizeSDKTargets.cmake
    NAMESPACE Canalize::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/CanalizeSDK
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/CanalizeSDKConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/CanalizeSDKConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/CanalizeSDK
)

# Add Plugins to the build
add_subdirectory(../../sdk/plugins/TerrainOverhaul bin/TerrainOverhaul)
