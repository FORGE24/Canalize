#include "canalize/forge/EventBus.h"

namespace Canalize {

    namespace Forge {

        EventBus& EventBus::getInstance() {
            static EventBus instance;
            return instance;
        }

    }
}
