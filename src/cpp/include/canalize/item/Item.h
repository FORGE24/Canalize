#pragma once
#include <string>
#include "../../CanalizeAPI.h"

// Disable MSVC warning C4251 for STL containers in DLL-exported classes
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4251)
#endif

namespace Canalize {
    namespace Item {

        class CANALIZE_API Item {
        public:
            Item(int id, std::string name, int maxStackSize = 64) 
                : mId(id), mName(name), mMaxStackSize(maxStackSize) {}
            
            virtual ~Item() = default;

            int getId() const { return mId; }
            std::string getName() const { return mName; }
            int getMaxStackSize() const { return mMaxStackSize; }

        private:
            int mId;
            std::string mName;
            int mMaxStackSize;
        };
    }
}

// Re-enable warnings
#ifdef _MSC_VER
#pragma warning(pop)
#endif
