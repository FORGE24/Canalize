#include "../include/canalize/item/ItemStack.h"
#include <algorithm>

namespace Canalize {
    namespace Item {

        ItemStack::ItemStack(const Item& item, int count) 
            : mItem(item), mCount(count) {}

        void ItemStack::setCount(int count) {
            mCount = std::max(0, std::min(count, mItem.getMaxStackSize()));
        }

    }
}
