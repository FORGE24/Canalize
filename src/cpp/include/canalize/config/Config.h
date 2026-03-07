#pragma once
#include <string>
#include <map>
#include <variant>
#include "../../CanalizeAPI.h"

namespace Canalize {
    namespace Config {

        using ConfigValue = std::variant<int, double, bool, std::string>;

        class CANALIZE_API Config {
        public:
            static Config& getInstance();

            void set(const std::string& key, const ConfigValue& value);
            
            template<typename T>
            T get(const std::string& key, const T& defaultValue) {
                if (values.find(key) != values.end()) {
                    try {
                        return std::get<T>(values[key]);
                    } catch (const std::bad_variant_access&) {
                        return defaultValue;
                    }
                }
                return defaultValue;
            }

            // Load/Save (Mock implementation, would use file I/O)
            void load(const std::string& path);
            void save(const std::string& path);

        private:
            std::map<std::string, ConfigValue> values;
        };
    }
}
