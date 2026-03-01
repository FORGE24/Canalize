#include "../include/WorldLoader.h"
#include "../include/TerrainGen.h"
#include "../include/Carver.h"
#include "../include/Decorator.h"
#include <iostream>

void WorldLoader::init() {
    std::cout << "[Native] WorldLoader Initialized. Ready for generation." << std::endl;
    // Load config, initialize random seeds if needed
}

void WorldLoader::generate_chunk(int chunkX, int chunkZ, int* buffer) {
    // 1. Base Terrain
    TerrainGen::generate_base_chunk(chunkX, chunkZ, buffer);
    
    // 2. Carvers (Caves)
    Carver::carve_chunk(chunkX, chunkZ, buffer);
    
    // 3. Decoration (Ores, Trees)
    Decorator::decorate_chunk(chunkX, chunkZ, buffer);
}

int WorldLoader::get_height(int x, int z) {
    return TerrainGen::calculate_pixel(x, z).height;
}

int WorldLoader::get_biome(int x, int z) {
    return TerrainGen::calculate_pixel(x, z).biomeId;
}
