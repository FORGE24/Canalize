#pragma once
#include "MathUtil.h"

struct TerrainResult { 
    int height; 
    int biomeId; 
};

class TerrainGen {
public:
    static const int CHUNK_WIDTH  = 16;
    static const int CHUNK_HEIGHT = 1808;
    static const int MIN_Y        = -64;
    
    // Calculate single pixel height and biome
    static TerrainResult calculate_pixel(int worldX, int worldZ);
    
    // Generate base terrain for a chunk (filling buffers)
    static void generate_base_chunk(int chunkX, int chunkZ, int* buffer);
};
