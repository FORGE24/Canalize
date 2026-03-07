#include "canalize/world/ChunkAccess.h"
#include "canalize/block/BlockState.h"
#include "TerrainGen.h" // For constants

namespace Canalize {

    namespace World {

        ChunkAccess::ChunkAccess(int chunkX, int chunkZ, int* buffer) 
            : mChunkX(chunkX), mChunkZ(chunkZ), mBuffer(buffer) {}

        void ChunkAccess::setBlockState(const BlockPos& pos, const Block::BlockState& state) {
            // Validate bounds relative to chunk
            // Local X, Z must be 0-15
            // Y must be MIN_Y to MAX_Y
            
            // Assuming pos is WORLD coordinate (absolute)
            // Or usually in generation, we work with local or relative coords.
            // Let's assume input pos is RELATIVE to chunk origin (0-15, Y, 0-15) for simplicity in this wrapper?
            // But BlockPos usually means absolute.
            
            // Let's implement absolute -> local conversion
            int localX = pos.x & 15;
            int localZ = pos.z & 15;
            
            // Check if pos is actually inside this chunk
            int cX = pos.x >> 4;
            int cZ = pos.z >> 4;
            
            // If strictly enforcing chunk bounds:
            // if (cX != mChunkX || cZ != mChunkZ) return; 
            
            // However, generation often fills just the local buffer.
            // We'll trust the caller passes valid local offsets or we mask them.
            
            int y = pos.y;
            int minY = -64; // Should use constant
            int totalHeight = 1808; 
            
            int localY = y - minY;
            
            if (localY >= 0 && localY < totalHeight) {
                 // Index: (y * 16 * 16) + (z * 16) + x
                 int index = (localY << 8) | (localZ << 4) | localX;
                 mBuffer[index] = state.getId();
            }
        }

        Block::BlockState ChunkAccess::getBlockState(const BlockPos& pos) const {
            int localX = pos.x & 15;
            int localZ = pos.z & 15;
            int y = pos.y;
            int minY = -64;
            int localY = y - minY;
            
            if (localY >= 0 && localY < 1808) {
                 int index = (localY << 8) | (localZ << 4) | localX;
                 return Block::BlockState(mBuffer[index]);
            }
            return Block::Blocks::AIR;
        }

    }
}
