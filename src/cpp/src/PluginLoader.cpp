#include "PluginLoader.h"
#include "NativeLog.h"
#include "PluginSystem.h"
#include <filesystem>
#include <iostream>

#ifdef _WIN32
#include <windows.h>
typedef void (*PluginInitFunc)(Canalize::PluginManager&);
#else
#include <dlfcn.h>
typedef void (*PluginInitFunc)(Canalize::PluginManager&);
#endif

namespace fs = std::filesystem;

namespace Canalize {

    void PluginLoader::loadPlugins(const std::string& directory) {
        if (!fs::exists(directory)) {
            NativeLog::info("Plugin directory not found: " + directory);
            return;
        }

        NativeLog::info("Scanning for plugins in: " + directory);

        for (const auto& entry : fs::directory_iterator(directory)) {
            auto ext = entry.path().extension().string();
            // Case insensitive check would be better but keeping it simple
            if (ext == ".dll" || ext == ".so" || ext == ".dylib") {
                std::string path = entry.path().string();
                NativeLog::info("Loading plugin candidate: " + path);

#ifdef _WIN32
                HMODULE handle = LoadLibraryA(path.c_str());
                if (!handle) {
                    NativeLog::error("Failed to load library: " + path + " (Error: " + std::to_string(GetLastError()) + ")");
                    continue;
                }
                PluginInitFunc init = (PluginInitFunc)GetProcAddress(handle, "CanalizePluginInit");
#else
                void* handle = dlopen(path.c_str(), RTLD_NOW);
                if (!handle) {
                    NativeLog::error("Failed to load library: " + std::string(dlerror()));
                    continue;
                }
                PluginInitFunc init = (PluginInitFunc)dlsym(handle, "CanalizePluginInit");
#endif

                if (init) {
                    try {
                        init(PluginManager::getInstance());
                        NativeLog::info("Successfully initialized plugin from: " + path);
                    } catch (const std::exception& e) {
                        NativeLog::error("Exception during plugin init: " + std::string(e.what()));
                    } catch (...) {
                        NativeLog::error("Unknown exception during plugin init: " + path);
                    }
                } else {
                    NativeLog::warn("Plugin " + path + " does not export 'CanalizePluginInit'");
                }
            }
        }
    }
}
