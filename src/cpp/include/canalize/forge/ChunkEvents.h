#pragma once
#include "EventBus.h"
#include "../world/ChunkAccess.h"

namespace Canalize {
    namespace Forge {

        // Event fired when a chunk is about to be generated
        class ChunkGeneratorEvent : public Event {
        public:
            ChunkGeneratorEvent(Canalize::World::ChunkAccess* chunk, int x, int z, int* buffer) 
                : chunk(chunk), chunkX(x), chunkZ(z), buffer(buffer) {}
            
            Canalize::World::ChunkAccess* getChunk() const { return chunk; }
            int getChunkX() const { return chunkX; }
            int getChunkZ() const { return chunkZ; }
            int* getBuffer() { return buffer; }

        private:
            Canalize::World::ChunkAccess* chunk;
            int chunkX;
            int chunkZ;
            int* buffer;
        };

        // Equivalent to net.minecraftforge.event.level.ChunkEvent.Load but for generation
        // Or "ReplaceBiomeBlocks"
    }
}
