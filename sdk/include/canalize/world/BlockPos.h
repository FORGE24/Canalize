#pragma once
#include "../../CanalizeAPI.h"

namespace Canalize {
    namespace World {

        class CANALIZE_API BlockPos {
        public:
            int x, y, z;
            BlockPos(int _x, int _y, int _z) : x(_x), y(_y), z(_z) {}
            BlockPos offset(int dx, int dy, int dz) const { return BlockPos(x+dx, y+dy, z+dz); }
        };
    }
}
