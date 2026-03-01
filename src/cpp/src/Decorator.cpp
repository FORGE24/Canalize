#include "../include/Decorator.h"
#include "../include/MathUtil.h"
#include "../include/TerrainGen.h"

// Extended Block IDs
const int BLOCK_AIR         = 0;
const int BLOCK_STONE       = 1;
const int BLOCK_WATER       = 2;
const int BLOCK_DIRT        = 3;
const int BLOCK_GRASS       = 4;
const int BLOCK_BEDROCK     = 5;
const int BLOCK_SAND        = 6;
const int BLOCK_SNOW        = 7;
const int BLOCK_ICE         = 8;
const int BLOCK_COAL_ORE    = 9;
const int BLOCK_IRON_ORE    = 10;
const int BLOCK_DIAMOND_ORE = 11;
const int BLOCK_LOG         = 12;
const int BLOCK_LEAVES      = 13;

// Helper for stateless random
inline float hash_rand(int x, int y, int z) {
    int h = p[(x & 255) + p[(y & 255) + p[z & 255]]];
    return (float)h / 255.0f;
}

void Decorator::decorate_chunk(int chunkX, int chunkZ, int* buffer) {
    const int CHUNK_HEIGHT = TerrainGen::CHUNK_HEIGHT;
    const int MIN_Y = TerrainGen::MIN_Y;
    const int startX = chunkX * 16;
    const int startZ = chunkZ * 16;
    
    // 1. Ores
    // Iterate through all blocks (optimized: skip air/water)
    for (int x = 0; x < 16; x++) {
        for (int z = 0; z < 16; z++) {
            int colIndex = (x * 16 + z) * CHUNK_HEIGHT;
            int worldX = startX + x;
            int worldZ = startZ + z;
            
            for (int yRel = 0; yRel < CHUNK_HEIGHT; yRel++) {
                int index = colIndex + yRel;
                int block = buffer[index];
                
                if (block == BLOCK_STONE) {
                    int worldY = yRel + MIN_Y;
                    place_ores(worldX, worldY, worldZ, buffer[index]);
                }
            }
        }
    }
    
    // 2. Trees
    // Iterate surface blocks (Grass)
    for (int x = 2; x < 14; x++) { // Avoid chunk borders for simplicity (2 block margin)
        for (int z = 2; z < 14; z++) {
            int colIndex = (x * 16 + z) * CHUNK_HEIGHT;
            int worldX = startX + x;
            int worldZ = startZ + z;
            
            // Find surface Y
            // Search from sea level up? Or top down?
            // Let's search from top down, but skip air.
            // A better way is to find the highest non-air block.
            // Optimization: TerrainGen stores height map? We can re-calculate or scan.
            // Scanning is fast enough.
            
            int surfaceYRel = -1;
            // Start scan from a reasonable height (e.g. 320 down to sea level)
            // Or just scan up from sea level until air.
            
            // Heuristic: Check from Y=63 up.
            for (int yRel = 63 - MIN_Y; yRel < CHUNK_HEIGHT - 10; yRel++) {
                if (buffer[colIndex + yRel] == BLOCK_GRASS && buffer[colIndex + yRel + 1] == BLOCK_AIR) {
                    surfaceYRel = yRel;
                    break;
                }
            }
            
            if (surfaceYRel != -1) {
                // Check tree chance
                // Use a hash of coordinate
                float r = hash_rand(worldX, worldZ, 123);
                if (r < 0.005f) { // 0.5% chance per block -> ~1-2 trees per chunk
                     place_tree(x, surfaceYRel + 1, z, buffer); // +1 to start on top of grass
                }
            }
        }
    }
}

void Decorator::place_ores(int worldX, int worldY, int worldZ, int& blockId) {
    // Simple single-block ores for now. Veins require more complex logic (3D noise or walker).
    float r = hash_rand(worldX, worldY, worldZ);
    
    if (r < 0.05f) { // 5% chance for any ore
        // Determine type based on height
        if (worldY < -50 && r < 0.005f) {
            blockId = BLOCK_DIAMOND_ORE;
        } else if (worldY < 72 && r < 0.02f) {
            blockId = BLOCK_IRON_ORE;
        } else {
            blockId = BLOCK_COAL_ORE;
        }
    }
}

void Decorator::place_tree(int localX, int startYRel, int localZ, int* buffer) {
    const int CHUNK_HEIGHT = TerrainGen::CHUNK_HEIGHT;
    // Simple Oak Tree: 5 logs high, leaves around top 3.
    int height = 5;
    
    if (startYRel + height + 2 >= CHUNK_HEIGHT) return; // Too high
    
    int colIndex = (localX * 16 + localZ) * CHUNK_HEIGHT;
    
    // Trunk
    for (int i = 0; i < height; i++) {
        buffer[colIndex + startYRel + i] = BLOCK_LOG;
    }
    
    // Leaves
    // A simple 3x3x2 blob at top
    // Warning: Writing to neighbor columns in buffer
    // Since we restricted localX/Z to [2, 13], we are safe (margin 2).
    
    int leafStart = startYRel + height - 2;
    int leafEnd = startYRel + height + 1;
    
    for (int ly = leafStart; ly <= leafEnd; ly++) {
        for (int lx = localX - 2; lx <= localX + 2; lx++) {
            for (int lz = localZ - 2; lz <= localZ + 2; lz++) {
                // Skip corners for rounder shape
                int dx = lx - localX;
                int dz = lz - localZ;
                if (abs(dx) + abs(dz) > 3) continue; // Diamond shape
                
                // Don't overwrite log
                int idx = (lx * 16 + lz) * CHUNK_HEIGHT + ly;
                if (buffer[idx] == BLOCK_AIR) {
                    buffer[idx] = BLOCK_LEAVES;
                }
            }
        }
    }
}
