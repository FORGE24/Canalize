#pragma once
#include "CanalizeAPI.h"
#include <vector>
#include <memory>
#include <string>

// Disable MSVC warning C4251 for STL containers in DLL-exported classes
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4251)
#endif

namespace Canalize {

    class CANALIZE_API IPlugin {
    public:
        virtual ~IPlugin() = default;
        
        virtual std::string getName() const = 0;
        virtual std::string getVersion() const = 0;

        // Called when plugin is loaded
        virtual void onInit() {}

        // Called before default terrain generation.
        // Return true to prevent default generation (replace it completely).
        virtual bool onPreGenerateChunk(int chunkX, int chunkZ, int* buffer) { return false; }

        // Called after default terrain generation.
        // Can be used to decorate or modify the generated chunk.
        virtual void onPostGenerateChunk(int chunkX, int chunkZ, int* buffer) {}
    };

    class CANALIZE_API PluginManager {
    public:
        static PluginManager& getInstance();

        void registerPlugin(std::shared_ptr<IPlugin> plugin);
        
        // Internal use
        bool dispatchPreGenerate(int chunkX, int chunkZ, int* buffer);
        void dispatchPostGenerate(int chunkX, int chunkZ, int* buffer);

    private:
        PluginManager() = default;
        std::vector<std::shared_ptr<IPlugin>> plugins;
    };
}

// Re-enable warnings
#ifdef _MSC_VER
#pragma warning(pop)
#endif
