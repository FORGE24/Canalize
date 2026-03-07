#include "../include/canalize/entity/Entity.h"

namespace Canalize {
    namespace Entity {

        Entity::Entity(const EntityType& type, double x, double y, double z)
            : mType(type), mX(x), mY(y), mZ(z), mMotionX(0), mMotionY(0), mMotionZ(0) {}

        void Entity::setPos(double x, double y, double z) {
            mX = x;
            mY = y;
            mZ = z;
        }

        void Entity::tick() {
            // Very basic movement simulation
            mX += mMotionX;
            mY += mMotionY;
            mZ += mMotionZ;
            
            // Gravity
            mMotionY -= 0.08;
            mMotionY *= 0.98;
            mMotionX *= 0.91;
            mMotionZ *= 0.91;
        }

    }
}
