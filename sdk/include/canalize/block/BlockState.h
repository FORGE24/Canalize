#pragma once
#include <vector>
#include <string>
#include "../../CanalizeAPI.h"

namespace Canalize {
    namespace Block {

        class CANALIZE_API BlockState {
        public:
            BlockState(int id) : mId(id) {}
            int getId() const { return mId; }
            bool isAir() const { return mId == 0; }
            bool isSolid() const { return mId != 0 && mId != 9; } // Simplified logic
            // In a real scenario, this would check properties map
            
        private:
            int mId;
        };

        class CANALIZE_API Blocks {
        public:
            static const BlockState AIR;
            static const BlockState STONE;
            static const BlockState GRASS;
            static const BlockState DIRT;
            static const BlockState WATER;
        };
    }
}
