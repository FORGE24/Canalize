#pragma once
#include <vector>
#include <functional>
#include <map>
#include <typeindex>
#include <memory>
#include "../../CanalizeAPI.h"

// Disable MSVC warning C4251 for STL containers in DLL-exported classes
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4251)
#endif

namespace Canalize {
    namespace Forge {

        class Event {
        public:
            virtual ~Event() = default;
            bool isCanceled() const { return mCanceled; }
            void setCanceled(bool canceled) { mCanceled = canceled; }
        private:
            bool mCanceled = false;
        };

        class CANALIZE_API IEventListener {
        public:
            virtual ~IEventListener() = default;
        };

        class CANALIZE_API EventBus {
        public:
            static EventBus& getInstance();

            template<typename T, typename Func>
            void subscribe(Func&& func) {
                // Simplified registration
                listeners[std::type_index(typeid(T))].push_back(
                    [func](Event& e) { func(static_cast<T&>(e)); }
                );
            }

            template<typename T>
            void post(T& event) {
                auto it = listeners.find(std::type_index(typeid(T)));
                if (it != listeners.end()) {
                    for (auto& handler : it->second) {
                        handler(event);
                        if (event.isCanceled()) break;
                    }
                }
            }

        private:
            std::map<std::type_index, std::vector<std::function<void(Event&)>>> listeners;
        };

    }
}

// Re-enable warnings
#ifdef _MSC_VER
#pragma warning(pop)
#endif
