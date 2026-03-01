#pragma once
#include <jni.h>

class WorldLoader {
public:
    // Initialize native components (called from JNI_OnLoad or specific init method)
    static void init();
    
    // Main generation entry point
    static void generate_chunk(int chunkX, int chunkZ, int* buffer);
    
    // Getters for specific data (used by BiomeSource etc)
    static int get_height(int x, int z);
    static int get_biome(int x, int z);
};
