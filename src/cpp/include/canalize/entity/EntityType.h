#pragma once
#include <string>
#include <vector>
#include "../../CanalizeAPI.h"
#include "../world/ChunkAccess.h" // For BlockPos

namespace Canalize {
    namespace Entity {

        class CANALIZE_API EntityType {
        public:
            EntityType(std::string id, float width, float height)
                : mId(id), mWidth(width), mHeight(height) {}

            std::string getId() const { return mId; }
            float getWidth() const { return mWidth; }
            float getHeight() const { return mHeight; }

            // Static registry (simplified)
            static const EntityType PLAYER;
            static const EntityType ZOMBIE;
            static const EntityType SKELETON;
            static const EntityType CREEPER;
            static const EntityType SPIDER;
            static const EntityType COW;
            static const EntityType SHEEP;
            static const EntityType PIG;
            static const EntityType CHICKEN;

        private:
            std::string mId;
            float mWidth;
            float mHeight;
        };
    }
}
