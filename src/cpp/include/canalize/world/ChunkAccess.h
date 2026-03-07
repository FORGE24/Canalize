#pragma once
#include "../../CanalizeAPI.h"
#include "../block/BlockState.h"

namespace Canalize {
    namespace World {

        class CANALIZE_API BlockPos {
        public:
            int x, y, z;
            BlockPos(int _x, int _y, int _z) : x(_x), y(_y), z(_z) {}
            BlockPos offset(int dx, int dy, int dz) const { return BlockPos(x+dx, y+dy, z+dz); }
        };

        // Represents a Chunk being generated. 
        // Wraps the raw buffer for safer access.
        class CANALIZE_API ChunkAccess {
        public:
            ChunkAccess(int chunkX, int chunkZ, int* buffer);
            
            void setBlockState(const BlockPos& pos, const Block::BlockState& state);
            Block::BlockState getBlockState(const BlockPos& pos) const;
            
            int getChunkX() const { return mChunkX; }
            int getChunkZ() const { return mChunkZ; }
            
        private:
            int mChunkX;
            int mChunkZ;
            int* mBuffer;
        };
    }
}
