#include "../include/canalize/entity/EntityType.h"

namespace Canalize {
    namespace Entity {
        const EntityType EntityType::PLAYER("player", 0.6f, 1.8f);
        const EntityType EntityType::ZOMBIE("zombie", 0.6f, 1.95f);
        const EntityType EntityType::SKELETON("skeleton", 0.6f, 1.99f);
        const EntityType EntityType::CREEPER("creeper", 0.6f, 1.7f);
        const EntityType EntityType::SPIDER("spider", 1.4f, 0.9f);
        const EntityType EntityType::COW("cow", 0.9f, 1.4f);
        const EntityType EntityType::SHEEP("sheep", 0.9f, 1.3f);
        const EntityType EntityType::PIG("pig", 0.9f, 0.9f);
        const EntityType EntityType::CHICKEN("chicken", 0.4f, 0.7f);
    }
}
