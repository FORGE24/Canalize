#include "PluginSystem.h"
#include "NativeLog.h"

namespace Canalize {

    PluginManager& PluginManager::getInstance() {
        static PluginManager instance;
        return instance;
    }

    void PluginManager::registerPlugin(std::shared_ptr<IPlugin> plugin) {
        plugins.push_back(plugin);
        // We use a simplified log format here to avoid dependency loop if NativeLog depends on something else
        // But NativeLog seems standalone.
        // NativeLog::info("Plugin registered: " + plugin->getName() + " v" + plugin->getVersion());
        plugin->onInit();
    }

    bool PluginManager::dispatchPreGenerate(int chunkX, int chunkZ, int* buffer) {
        bool handled = false;
        for (auto& plugin : plugins) {
            if (plugin->onPreGenerateChunk(chunkX, chunkZ, buffer)) {
                handled = true;
                // If one plugin handles it, do we stop? Or let others try?
                // For "replace generation", usually the first one wins.
                // But let's assume multiple plugins might want to contribute?
                // If it returns true, it means "skip default".
                // We'll keep iterating to let all plugins see the event, 
                // but the return value is OR'ed.
            }
        }
        return handled;
    }

    void PluginManager::dispatchPostGenerate(int chunkX, int chunkZ, int* buffer) {
        for (auto& plugin : plugins) {
            plugin->onPostGenerateChunk(chunkX, chunkZ, buffer);
        }
    }
}
