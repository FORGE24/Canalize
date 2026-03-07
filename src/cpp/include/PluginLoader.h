#pragma once
#include <string>

namespace Canalize {
    class PluginLoader {
    public:
        static void loadPlugins(const std::string& directory);
    };
}
