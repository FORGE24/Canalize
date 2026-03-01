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

// Block IDs
const int BLOCK_AIR = 0;
const int BLOCK_STONE = 1;
const int BLOCK_WATER = 2;
const int BLOCK_DIRT = 3;
const int BLOCK_GRASS = 4;
const int BLOCK_BEDROCK = 5;

// --- Smooth Noise Implementation ---

// Smooth interpolation function
inline float fade(float t) {
    return t * t * t * (t * (t * 6 - 15) + 10);
}

// Linear interpolation
inline float lerp(float t, float a, float b) {
    return a + t * (b - a);
}

// Gradient function for 2D Perlin Noise
inline float grad(int hash, float x, float y) {
    int h = hash & 15;
    float u = h < 8 ? x : y;
    float v = h < 4 ? y : h == 12 || h == 14 ? x : 0; // Simplified for 2D
    return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

// Permutation table (standard Perlin)
// We'll generate it deterministically based on seed
static int p[512];
static bool p_initialized = false;

void init_noise(int seed) {
    if (p_initialized) return;
    srand(seed);
    for(int i=0; i<256; i++) p[i] = i;
    // Shuffle
    for(int i=0; i<256; i++) {
        int j = rand() % 256;
        int temp = p[i];
        p[i] = p[j];
        p[j] = temp;
    }
    // Duplicate
    for(int i=0; i<256; i++) p[256+i] = p[i];
    p_initialized = true;
}

// 2D Perlin Noise
// Returns value roughly between -1.0 and 1.0
float perlin(float x, float y) {
    if (!p_initialized) init_noise(12345);

    int X = (int)floor(x) & 255;
    int Y = (int)floor(y) & 255;

    x -= floor(x);
    y -= floor(y);

    float u = fade(x);
    float v = fade(y);

    int A = p[X] + Y;
    int B = p[X + 1] + Y;

    return lerp(v, lerp(u, grad(p[A], x, y), grad(p[B], x - 1, y)),
                   lerp(u, grad(p[A + 1], x, y - 1), grad(p[B + 1], x - 1, y - 1)));
}

// Fractal Brownian Motion (Octaves)
float fbm(float x, float y, int octaves, float persistence, float lacunarity) {
    float total = 0;
    float frequency = 1;
    float amplitude = 1;
    float maxValue = 0;
    for(int i=0; i<octaves; i++) {
        total += perlin(x * frequency, y * frequency) * amplitude;
        maxValue += amplitude;
        amplitude *= persistence;
        frequency *= lacunarity;
    }
    return total / maxValue;
}

// 3D Noise approximation using SIMD (AVX2)
void generate_chunk_simd(int chunkX, int chunkZ, jint* buffer) {
    int seed = 12345; 
    init_noise(seed); // Ensure noise table is ready
    
    // Base coordinates
    int startX = chunkX * 16;
    int startZ = chunkZ * 16;

    // Noise parameters
    float scale = 0.01f; // Low frequency for large hills
    int octaves = 4;
    float persistence = 0.5f;
    float lacunarity = 2.0f;

    for (int x = 0; x < 16; x++) {
        for (int z = 0; z < 16; z++) {
            int worldX = startX + x;
            int worldZ = startZ + z;
            
            // Generate base height using Perlin Noise
            // fbm returns -1 to 1. Map to reasonable height range.
            // Let's map -1..1 to 40..120
            float noiseVal = fbm(worldX * scale, worldZ * scale, octaves, persistence, lacunarity);
            
            int height = (int)(70 + noiseVal * 40); // Base level 70, variation +/- 40
            
            // Clamp height to world bounds
            if (height < MIN_Y + 1) height = MIN_Y + 1;
            if (height >= MIN_Y + CHUNK_HEIGHT) height = MIN_Y + CHUNK_HEIGHT - 1;

            int colIndex = (x * 16 + z) * CHUNK_HEIGHT;

            // 1. Bedrock floor
            buffer[colIndex + 0] = BLOCK_BEDROCK; 
            
            // 2. Fill Stone up to Height using AVX2
            int localY_Height = height - MIN_Y; 
            
            __m256i v_stone = _mm256_set1_epi32(BLOCK_STONE);
            
            int y = 1;
            for (; y <= localY_Height - 8; y += 8) {
                 _mm256_storeu_si256((__m256i*)&buffer[colIndex + y], v_stone);
            }
            for (; y < localY_Height; y++) {
                buffer[colIndex + y] = BLOCK_STONE;
            }
            
            // 3. Top layer (Grass/Dirt)
            if (height >= 62) { // Above or near water
                 buffer[colIndex + localY_Height] = BLOCK_GRASS;
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
        }
    }
}

// Function to simulate the "Hijack" process
void perform_hijack() {
    std::cout << "[Native] Performing Hijack..." << std::endl;
    // std::this_thread::sleep_for(std::chrono::milliseconds(500));
    std::cout << "[Native] Hijack Successful!" << std::endl;
}

// Function to simulate "CPP Loading"
void load_model_block() {
    std::cout << "[Native] Loading MODEL Block..." << std::endl;
    // std::this_thread::sleep_for(std::chrono::milliseconds(500));
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
    }

    JNIEXPORT void JNICALL Java_cn_sanrol_canalize_world_NativeChunkGenerator_generateSurfaceNative(JNIEnv *env, jobject thiz, jobject chunk) {
        // Placeholder
    }

    // New optimized generation method
    // Signature: (II[I)V
    JNIEXPORT void JNICALL Java_cn_sanrol_canalize_world_NativeChunkGenerator_generateChunkData(JNIEnv *env, jobject thiz, jint chunkX, jint chunkZ, jintArray blockDataArray) {
        
        jboolean isCopy;
        jint* buffer = env->GetIntArrayElements(blockDataArray, &isCopy);
        
        if (buffer == nullptr) {
            std::cerr << "[Native] Error: Could not get array elements!" << std::endl;
            return;
        }

        // Generate terrain using Perlin + SIMD
        generate_chunk_simd(chunkX, chunkZ, buffer);
        
        env->ReleaseIntArrayElements(blockDataArray, buffer, 0);
    }

}
