cmake_minimum_required(VERSION 3.10)
project(TerrainOverhaul)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

# If built as part of the SDK examples or main project
if(TARGET canalize_native)
    # In-source build
    add_library(TerrainOverhaul SHARED src/OverhaulPlugin.cpp)
    target_link_libraries(TerrainOverhaul PRIVATE canalize_native)
else()
    # Standalone build using installed SDK
    find_package(CanalizeSDK REQUIRED)
    add_library(TerrainOverhaul SHARED src/OverhaulPlugin.cpp)
    target_link_libraries(TerrainOverhaul PRIVATE Canalize::canalize_native)
endif()

# Output to a specific 'plugins' directory
set_target_properties(TerrainOverhaul PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/../../build/libs/canalize_plugins"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/../../build/libs/canalize_plugins"
)
