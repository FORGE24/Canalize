#include "../include/WorldLoader.h"
#include "../include/TerrainGen.h"
#include "../include/Carver.h"
#include "../include/Decorator.h"
#include "../include/NativeStatus.h"
#include "../include/NativeLog.h"
#include <iostream>
#include <chrono>
#include <sstream>

// Biome ID -> human-readable name (mirrors TerrainGen biome assignments)
static const char* biome_name(int id) {
    switch (id) {
        case   0: return "ocean";
        case   1: return "plains";
        case 100: return "extreme_hills";
        case 200: return "mountains";
        default:  return "unknown";
    }
}

void WorldLoader::init() {
    std::cout << "[Native] WorldLoader Initialized. Ready for generation." << std::endl;
    NativeStatus::reset();
    NativeLog::clear();
    NativeLog::info("WorldLoader init OK — NativeStatus reset.");
}

void WorldLoader::generate_chunk(int chunkX, int chunkZ, int* buffer) {
    auto t0 = std::chrono::high_resolution_clock::now();

    if (NativeLog::is_enabled()) {
        char buf[64];
        std::snprintf(buf, sizeof(buf), "[Gen] Chunk [%d,%d] START", chunkX, chunkZ);
        NativeLog::debug(buf);
    }

    // 1. Base Terrain
    TerrainGen::generate_base_chunk(chunkX, chunkZ, buffer);

    // 2. Carvers (Caves)
    Carver::carve_chunk(chunkX, chunkZ, buffer);

    // 3. Decoration (Ores, Trees)
    Decorator::decorate_chunk(chunkX, chunkZ, buffer);

    auto t1 = std::chrono::high_resolution_clock::now();
    int64_t ns = std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count();
    NativeStatus::recordChunkGen(chunkX, chunkZ, ns);

    if (NativeLog::is_enabled()) {
        // Only log timing once in a while to avoid spam (every 16 chunks)
        int64_t total = NativeStatus::chunksGenerated.load(std::memory_order_relaxed);
        if (total % 16 == 0) {
            char buf[128];
            std::snprintf(buf, sizeof(buf),
                "[Gen] #%lld chunks | last [%d,%d] %.2f ms | avg %.2f ms",
                (long long)total, chunkX, chunkZ,
                ns / 1e6, NativeStatus::getAvgGenTimeNs() / 1e6);
            NativeLog::info(buf);
        }
    }
}

int WorldLoader::get_height(int x, int z) {
    return TerrainGen::calculate_pixel(x, z).height;
}

int WorldLoader::get_biome(int x, int z) {
    return TerrainGen::calculate_pixel(x, z).biomeId;
}

std::string WorldLoader::get_terrain_info(int blockX, int blockZ) {
    TerrainResult tr = TerrainGen::calculate_pixel(blockX, blockZ);
    std::ostringstream oss;
    oss << "height="   << tr.height
        << "|biome="   << biome_name(tr.biomeId)
        << "|biomeId=" << tr.biomeId
        << "|seaLevel=" << 63
        << "|minY="     << TerrainGen::MIN_Y
        << "|maxY="     << (TerrainGen::MIN_Y + TerrainGen::CHUNK_HEIGHT - 1);
    return oss.str();
}

std::string WorldLoader::get_status() {
    return NativeStatus::buildStatusString();
}
