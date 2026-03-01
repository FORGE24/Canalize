#include <jni.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <cmath>
#include <immintrin.h> // AVX2 intrinsics

// Constants matching Java
const int CHUNK_WIDTH = 16;
const int CHUNK_HEIGHT = 384;
const int MIN_Y = -64;
const int BUFFER_SIZE = CHUNK_WIDTH * CHUNK_WIDTH * CHUNK_HEIGHT;

// Block IDs (must match Java switch case)
const int BLOCK_AIR = 0;
const int BLOCK_STONE = 1;
const int BLOCK_WATER = 2;
const int BLOCK_DIRT = 3;
const int BLOCK_GRASS = 4;
const int BLOCK_BEDROCK = 5;

// Simple 2D Pseudo-Noise (Integer Hashing)
// Faster than Perlin for demonstration, and deterministic
inline int hash2d(int x, int z, int seed) {
    int h = seed + x * 374761393 + z * 668265263;
    h = (h ^ (h >> 13)) * 1274126177;
    return h ^ (h >> 16);
}

// 3D Noise approximation using SIMD (AVX2)
// This simulates "Inline Assembly" optimization on x64
void generate_chunk_simd(int chunkX, int chunkZ, jint* buffer) {
    int seed = 12345; // World seed placeholder
    
    // Base coordinates
    int startX = chunkX * 16;
    int startZ = chunkZ * 16;

    for (int x = 0; x < 16; x++) {
        for (int z = 0; z < 16; z++) {
            int worldX = startX + x;
            int worldZ = startZ + z;
            
            // Generate base height using 2D noise
            // Range: 60 to 100
            int h = hash2d(worldX, worldZ, seed);
            int height = 60 + (std::abs(h) % 40); 
            
            // Loop Y and fill buffer
            // We use a flat index: index = (x * 16 + z) * 384 + y ?? 
            // WAIT: Java loop is: x outer, z middle, y inner.
            // So index increments by 1 for each Y step.
            // Base index for this column:
            int colIndex = (x * 16 + z) * CHUNK_HEIGHT;

            // 1. Bedrock floor (Optimized: direct set)
            buffer[colIndex + 0] = BLOCK_BEDROCK; // Y = -64
            
            // 2. Fill Stone up to Height
            // Using a loop for now, SIMD could be used to fill 8 ints at a time
            int localY_Height = height - MIN_Y; // Convert world height to array index
            
            // AVX2 Fill for Stone
            // We want to set blocks from index 1 to localY_Height to BLOCK_STONE
            // We can process 8 blocks at a time
            __m256i v_stone = _mm256_set1_epi32(BLOCK_STONE);
            
            int y = 1;
            for (; y <= localY_Height - 8; y += 8) {
                 _mm256_storeu_si256((__m256i*)&buffer[colIndex + y], v_stone);
            }
            // Handle remaining
            for (; y < localY_Height; y++) {
                buffer[colIndex + y] = BLOCK_STONE;
            }
            
            // 3. Top layer (Grass/Dirt)
            // If above water level, grass; else dirt/gravel (simplified)
            if (height >= 63) {
                 buffer[colIndex + localY_Height] = BLOCK_GRASS;
                 // Set a few blocks below to dirt
                 if (localY_Height > 3) {
                     buffer[colIndex + localY_Height - 1] = BLOCK_DIRT;
                     buffer[colIndex + localY_Height - 2] = BLOCK_DIRT;
                     buffer[colIndex + localY_Height - 3] = BLOCK_DIRT;
                 }
            } else {
                 buffer[colIndex + localY_Height] = BLOCK_DIRT; // Underwater floor
            }

            // 4. Fill Water up to Sea Level (63)
            int seaLevelIndex = 63 - MIN_Y;
            for (int wy = localY_Height + 1; wy <= seaLevelIndex; wy++) {
                 buffer[colIndex + wy] = BLOCK_WATER;
            }
            
            // 5. Air is already 0 (default init in Java? No, JNI GetIntArrayElements might not clear it, 
            // but new int[] in Java is zeroed. If we reuse buffer, we must clear.
            // Since we allocate new int[] in Java every time, it's zeroed.
        }
    }
}

// Function to simulate the "Hijack" process
void perform_hijack() {
    std::cout << "[Native] Performing Hijack..." << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    std::cout << "[Native] Hijack Successful!" << std::endl;
}

// Function to simulate "CPP Loading"
void load_model_block() {
    std::cout << "[Native] Loading MODEL Block..." << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    std::cout << "[Native] MODEL Block Loaded." << std::endl;
}

extern "C" {

    JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
        std::cout << "[Native] JNI_OnLoad called. Library loaded." << std::endl;
        return JNI_VERSION_1_8;
    }

    JNIEXPORT void JNICALL JNI_OnUnload(JavaVM *vm, void *reserved) {
        std::cout << "[Native] JNI_OnUnload called. Library unloaded." << std::endl;
    }

    JNIEXPORT void JNICALL Java_cn_sanrol_canalize_Canalize_initNative(JNIEnv *env, jclass clazz) {
        std::cout << "[Native] initNative called from Java." << std::endl;
        perform_hijack();
        load_model_block();
        std::cout << "[Native] Returning control to Game..." << std::endl;
    }

    JNIEXPORT void JNICALL Java_cn_sanrol_canalize_world_NativeChunkGenerator_generateNoiseNative(JNIEnv *env, jobject thiz, jobject chunk) {
         // Deprecated / Unused legacy method
         // We use generateChunkData now
    }

    JNIEXPORT void JNICALL Java_cn_sanrol_canalize_world_NativeChunkGenerator_generateSurfaceNative(JNIEnv *env, jobject thiz, jobject chunk) {
        // Placeholder
    }

    // New optimized generation method
    // Signature: (II[I)V
    JNIEXPORT void JNICALL Java_cn_sanrol_canalize_world_NativeChunkGenerator_generateChunkData(JNIEnv *env, jobject thiz, jint chunkX, jint chunkZ, jintArray blockDataArray) {
        
        // Get direct access to the array elements
        // isCopy will tell us if we are working on a copy or the actual array (JVM dependent)
        jboolean isCopy;
        jint* buffer = env->GetIntArrayElements(blockDataArray, &isCopy);
        
        if (buffer == nullptr) {
            std::cerr << "[Native] Error: Could not get array elements!" << std::endl;
            return;
        }

        // Generate terrain using SIMD
        generate_chunk_simd(chunkX, chunkZ, buffer);
        
        // Release the array (commit changes back to Java if it was a copy)
        // 0 = Commit and free
        env->ReleaseIntArrayElements(blockDataArray, buffer, 0);
    }

}
