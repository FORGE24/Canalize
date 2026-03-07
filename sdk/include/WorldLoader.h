#pragma once
#include <jni.h>
#include <string>
#include "CanalizeAPI.h"

class CANALIZE_API WorldLoader {
public:
    // Initialize native components (called from JNI_OnLoad or specific init method)
    static void init();

    // Main generation entry point (auto-records timing in NativeStatus)
    static void generate_chunk(int chunkX, int chunkZ, int* buffer);

    // Getters for specific data (used by BiomeSource etc)
    static int get_height(int x, int z);
    static int get_biome(int x, int z);

    // Returns a pipe-separated terrain info string for /canalize terrain <x> <z>
    // Format: "height=X|biome=NAME|biomeId=N|seaLevel=63"
    static std::string get_terrain_info(int blockX, int blockZ);

    // Returns the full status string from NativeStatus::buildStatusString()
    static std::string get_status();
};
