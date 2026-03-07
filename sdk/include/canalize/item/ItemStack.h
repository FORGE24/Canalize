#pragma once
#include "../../CanalizeAPI.h"
#include "Item.h"

namespace Canalize {
    namespace Item {

        class CANALIZE_API ItemStack {
        public:
            ItemStack(const Item& item, int count = 1);
            
            const Item& getItem() const { return mItem; }
            int getCount() const { return mCount; }
            void setCount(int count);
            
            bool isEmpty() const { return mCount <= 0 || mItem.getId() == 0; }
            
        private:
            const Item& mItem;
            int mCount;
        };
    }
}
