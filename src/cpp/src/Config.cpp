#include "../include/canalize/config/Config.h"
#include <fstream>
#include <iostream>

namespace Canalize {
    namespace Config {

        Config& Config::getInstance() {
            static Config instance;
            return instance;
        }

        void Config::set(const std::string& key, const ConfigValue& value) {
            values[key] = value;
        }

        void Config::load(const std::string& path) {
            // Placeholder for file loading
            // In a real implementation, we would parse JSON/TOML/Properties
            // For now, we just log
            std::cout << "[Config] Loading config from " << path << std::endl;
        }

        void Config::save(const std::string& path) {
            std::cout << "[Config] Saving config to " << path << std::endl;
        }

    }
}
