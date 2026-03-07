#pragma once
#include "../../CanalizeAPI.h"
#include "EntityType.h"

namespace Canalize {
    namespace Entity {

        class CANALIZE_API Entity {
        public:
            Entity(const EntityType& type, double x, double y, double z);
            virtual ~Entity() = default;

            const EntityType& getType() const { return mType; }
            
            double getX() const { return mX; }
            double getY() const { return mY; }
            double getZ() const { return mZ; }
            
            void setPos(double x, double y, double z);
            
            // Basic physics (placeholder)
            void tick();

        protected:
            const EntityType& mType;
            double mX, mY, mZ;
            double mMotionX, mMotionY, mMotionZ;
        };
    }
}
