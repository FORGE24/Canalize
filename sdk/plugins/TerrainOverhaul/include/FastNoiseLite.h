// FastNoiseLite.h - Simplified for Canalize Overhaul (Conceptual Implementation)
// In a real scenario, this would be the full FastNoiseLite library.
// Here we implement a basic high-performance 3D Simplex-like noise for demonstration.

#pragma once
#include <cmath>
#include <cstdint>

class FastNoiseLite {
public:
    enum NoiseType { OpenSimplex2, Perlin };
    
    FastNoiseLite(int seed = 1337) : mSeed(seed) {}

    void SetNoiseType(NoiseType type) { mType = type; }
    void SetFrequency(float freq) { mFrequency = freq; }

    // 2D Noise
    float GetNoise(float x, float y) {
        return GetNoise(x, y, 0.0f);
    }

    // 3D Noise (Simplified Gradient Noise for Demo)
    float GetNoise(float x, float y, float z) {
        x *= mFrequency;
        y *= mFrequency;
        z *= mFrequency;
        
        // Very basic pseudo-noise for demonstration purposes
        // Real implementation would use lookups and gradient vectors
        return (sin(x * 12.9898 + mSeed) * 43758.5453 
              + cos(y * 78.233 + z * 32.1) * 21321.2) - floor((sin(x) + cos(y))*10000);
    }
    
    // Fractal Brownian Motion
    float GetNoiseFBM(float x, float y, float z, int octaves, float lacunarity, float gain) {
        float sum = 0.0f;
        float amp = 1.0f;
        float max = 0.0f;
        
        for(int i=0; i<octaves; i++) {
            sum += GetNoise(x, y, z) * amp;
            max += amp;
            x *= lacunarity;
            y *= lacunarity;
            z *= lacunarity;
            amp *= gain;
        }
        return sum / max; // Normalize
    }

private:
    int mSeed;
    float mFrequency = 0.01f;
    NoiseType mType = OpenSimplex2;
};
