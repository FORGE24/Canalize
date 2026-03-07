# Canalize SDK Developer Guide

Canalize SDK allows you to create C++ plugins that hook into the terrain generation pipeline of Canalize.

## Directory Structure

- `include/`: Contains the public API headers.
- `cmake/`: CMake configuration files for `find_package`.
- `examples/`: Example plugins.

## How to Build a Plugin

1. Create a `CMakeLists.txt` for your project.
2. Find the Canalize SDK package:
   ```cmake
   find_package(CanalizeSDK REQUIRED)
   ```
3. Link your library against `Canalize::canalize_native`:
   ```cmake
   add_library(MyPlugin SHARED src/MyPlugin.cpp)
   target_link_libraries(MyPlugin PRIVATE Canalize::canalize_native)
   ```
4. Implement the `IPlugin` interface and export the entry point:
   ```cpp
   #include <canalize/CanalizeSDK.h>

   class MyPlugin : public Canalize::IPlugin {
       // ... implement methods ...
   };

   CANALIZE_PLUGIN_ENTRY {
       manager.registerPlugin(std::make_shared<MyPlugin>());
   }
   ```
5. Build your plugin and place the `.dll` (or `.so`) into the `canalize_plugins` folder in your Minecraft game directory.

## API Overview

- **IPlugin**: The interface you must implement.
  - `onPreGenerateChunk`: Called before default generation. Return `true` to skip default generation.
  - `onPostGenerateChunk`: Called after generation. Use this to decorate or modify the chunk.
- **TerrainGen**: Access to the default noise generators and terrain algorithms.
- **WorldLoader**: Core chunk management.
- **NativeLog**: Send log messages to the Minecraft chat console.
