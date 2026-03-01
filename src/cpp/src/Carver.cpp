#include "../include/Carver.h"
#include "../include/MathUtil.h"
#include "../include/TerrainGen.h"

// Re-declare constants or include a common header
const int BLOCK_AIR     = 0;
const int BLOCK_STONE   = 1;
const int BLOCK_WATER   = 2;
const int BLOCK_BEDROCK = 5;

void Carver::carve_chunk(int chunkX, int chunkZ, int* buffer) {
    const int CHUNK_HEIGHT = TerrainGen::CHUNK_HEIGHT;
    const int MIN_Y = TerrainGen::MIN_Y;
    const int startX = chunkX * 16;
    const int startZ = chunkZ * 16;
    
    // Cave parameters
    const float CAVE_SCALE = 0.04f;
    const float CAVE_THRESHOLD = 0.6f;
    
    // Loop through the chunk
    for (int x = 0; x < 16; x++) {
        for (int z = 0; z < 16; z++) {
            int colIndex = (x * 16 + z) * CHUNK_HEIGHT;
            int worldX = startX + x;
            int worldZ = startZ + z;
            
            // Only carve up to a certain height (e.g., sea level or slightly above)
            // Carving mountain caves is also cool though.
            // Let's carve up to Y=200 for now.
            for (int yRel = 1; yRel < CHUNK_HEIGHT - 1; yRel++) {
                int worldY = yRel + MIN_Y;
                
                // Don't carve bedrock or too high
                if (worldY <= MIN_Y + 5) continue;
                if (worldY > 320) continue; 
                
                int index = colIndex + yRel;
                int currentBlock = buffer[index];
                
                // Only carve stone, dirt, etc. Don't carve water (unless we want flooded caves, but that requires water filling logic)
                // For now, simple air caves.
                if (currentBlock == BLOCK_WATER || currentBlock == BLOCK_BEDROCK || currentBlock == BLOCK_AIR) continue;
                
                // 3D Noise check
                float noiseVal = simplex3(worldX * CAVE_SCALE, worldY * CAVE_SCALE * 1.5f, worldZ * CAVE_SCALE);
                
                // Adjust threshold based on depth?
                // Deeper = more open?
                // float depthFactor = clamp((float)(100 - worldY) / 100.0f, 0.0f, 0.5f);
                
                if (noiseVal > CAVE_THRESHOLD) {
                    buffer[index] = BLOCK_AIR;
                }
            }
        }
    }
}

bool Carver::is_cave(int worldX, int worldY, int worldZ) {
    // Helper not strictly needed if inlined above, but good for modularity
    return false; 
}
