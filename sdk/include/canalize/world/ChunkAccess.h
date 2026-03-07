#pragma once
#include "../../CanalizeAPI.h"
#include "../block/BlockState.h"
#include "BlockPos.h"

namespace Canalize {
    namespace World {

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
