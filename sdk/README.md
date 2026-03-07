# Canalize SDK Developer Guide

Canalize SDK allows you to create C++ plugins that hook into the terrain generation pipeline of Canalize.
With version 2.0, we introduce a **NeoForge-like C++ API** that allows you to develop mods with familiar concepts like Events, Blocks, Items, Entities, GUI, Network, and Rendering.

## Directory Structure

- `include/`: Contains the public API headers.
  - `canalize/api`: JNI and core utilities.
  - `canalize/block`: BlockState and Block registry.
  - `canalize/item`: Item, ItemStack, and Item registry.
  - `canalize/entity`: Entity and EntityType registry.
  - `canalize/world`: World access, Chunk access, BlockPos.
  - `canalize/forge`: Event bus and event definitions.
  - `canalize/config`: Configuration system.
  - `canalize/gui`: Screen and FontRenderer.
  - `canalize/network`: Packet system.
  - `canalize/render`: RenderSystem and BufferBuilder.
- `cmake/`: CMake configuration files.
- `plugins/`: Source code for built-in or example plugins (e.g., `TerrainOverhaul`).

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
4. Implement the `IPlugin` interface and register it:
   ```cpp
   #include <CanalizeSDK.h>

   using namespace Canalize;
   using namespace Canalize::Forge;

   class MyPlugin : public IPlugin {
       void onInit() override {
           // Subscribe to events
           EventBus::getInstance().subscribe<ChunkGeneratorEvent>([](ChunkGeneratorEvent& e) {
               // Handle generation
           });
       }
   };

   CANALIZE_PLUGIN_ENTRY {
       manager.registerPlugin(std::make_shared<MyPlugin>());
   }
   ```

## API Overview

### Event System
The `EventBus` allows you to subscribe to lifecycle and gameplay events.
```cpp
EventBus::getInstance().subscribe<ChunkGeneratorEvent>([](ChunkGeneratorEvent& e) {
    if (e.getChunkX() == 0 && e.getChunkZ() == 0) {
        NativeLog::info("Generating spawn chunk!");
    }
});
```

### Block & World Manipulation
Use `ChunkAccess` and `BlockPos` to manipulate the world safely.
```cpp
ChunkAccess* chunk = event.getChunk();
chunk->setBlockState(BlockPos(0, 64, 0), Blocks::DIAMOND_BLOCK);
```

### Items & Entities
Access standard Minecraft items and entities via `Items` and `EntityType`.
```cpp
Item item = Items::DIAMOND_SWORD;
EntityType type = EntityType::ZOMBIE;
```

### Configuration
Load and save configuration files easily.
```cpp
Config::getInstance().load("config/my_mod.toml");
int value = Config::getInstance().get<int>("my_setting", 42);
```

### GUI
Create custom screens by inheriting from `Screen`.
```cpp
class MyScreen : public Screen {
public:
    MyScreen() : Screen("My GUI") {}
    void render(int mx, int my, float pt) override {
        FontRenderer::drawString("Hello World", 10, 10, 0xFFFFFF);
    }
};
```

### Network
Send custom packets between client and server.
```cpp
class MyPacket : public Packet { ... };
NetworkManager::getInstance().sendToServer(MyPacket("data"));
```

### Rendering
Use `RenderSystem` and `BufferBuilder` for custom drawing.
```cpp
RenderSystem::setShaderColor(1.0f, 0.0f, 0.0f, 1.0f);
auto& builder = Tessellator::getInstance().getBuilder();
builder.begin(GL_QUADS, DefaultVertexFormat::POSITION);
builder.vertex(0, 0, 0);
builder.vertex(10, 10, 0);
builder.end();
builder.draw();
```

## Advanced: JNI Bridge
For deep integration, use `JNIHelper` to call Java methods directly from C++.
```cpp
JNIEnv* env = JNIHelper::getEnv();
jclass cls = JNIHelper::findClass("net/minecraft/world/level/block/Blocks");
```
