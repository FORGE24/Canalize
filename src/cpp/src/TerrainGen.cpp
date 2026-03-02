// =============================================================================
// TerrainGen.cpp — JJTH Terrain System (Faithful C++ Port)
//
// JJTH is an independent Minecraft datapack project. This file ports its
// terrain generation density-function architecture to a native C++ height-map
// calculator used by Canalize via JNI.
//
// Architecture overview (mirrors JJTH density_function tree):
//
//  1. Noise channels
//     cave_n()     — minecraft:cave_layer  (firstOctave=-8,  amps=[1,1])
//     gravel_n()   — minecraft:gravel      (firstOctave=-8,  amps=[1,1])
//     jagged_n()   — minecraft:jagged      (firstOctave=-16, amps=[1])
//     erosion_n()  — minecraft:erosion_med (firstOctave=-9,  amps=[1,1,0,1,1])
//
//  2. Per-terrain-type surface density
//     height_extreme_mountains / mountains / extreme_hills / hills / plains / ocean
//     Each = offset_noise + eroded_noise_tuned + jagged_adjusted
//     (eroded_noise uses the divergence-based erosion_trick from
//      realism/*/factor/erosion_trick.json  ≈  1 / (1 + div_sq * 0.2))
//
//  3. Smart determiner (determiner/overworld/smart.json)
//     Low-frequency cave noise with added detail noise in transition bands.
//
//  4. Unrivered height spline (new_surface/height/unprocessed/unrivered.json)
//     Maps abs(smart_det) → interpolated terrain surface density:
//       0.000 → extreme_mountains
//       0.075 → mountains
//       0.150 → extreme_hills
//       0.225 → hills
//       0.300 → plains
//       0.400 → ocean
//
//  5. River carving (new_surface/height/unprocessed/rivered.json)
//     At abs_det ≈ 0.30–0.34, density dips to -0.91 (river trough).
//
//  6. Block height conversion
//     h = (1 + surface_density) * (CHUNK_HEIGHT / 2) + MIN_Y
//
// Noise frequency calibration:
//   actual_freq(per block) = xz_scale * 2^(firstOctave)
//   cave/gravel : /256   (firstOctave = -8)
//   jagged      : /65536 (firstOctave = -16)
//   erosion_med : /512   (firstOctave = -9)
// =============================================================================

#include "../include/TerrainGen.h"
#include "../include/MathUtil.h"
#include <cmath>
#include <algorithm>
#include <immintrin.h>

// ----------------------------------------------------------------------------
// Block IDs
// ----------------------------------------------------------------------------
static constexpr int BLOCK_AIR     = 0;
static constexpr int BLOCK_STONE   = 1;
static constexpr int BLOCK_WATER   = 2;
static constexpr int BLOCK_DIRT    = 3;
static constexpr int BLOCK_GRASS   = 4;
static constexpr int BLOCK_BEDROCK = 5;
static constexpr int BLOCK_SAND    = 6;
static constexpr int BLOCK_SNOW    = 7;

// ----------------------------------------------------------------------------
// Noise base frequencies (firstOctave to block-unit conversion)
// ----------------------------------------------------------------------------
static constexpr float CAVE_BASE    = 1.0f / 256.0f;   // firstOctave = -8
static constexpr float GRAVEL_BASE  = 1.0f / 256.0f;   // firstOctave = -8
static constexpr float JAGGED_BASE  = 1.0f / 65536.0f; // firstOctave = -16
static constexpr float EROSION_BASE = 1.0f / 512.0f;   // firstOctave = -9

// Seed offsets — decouple each named noise channel so they produce
// independent fields despite sharing the same permutation table.
static constexpr float CAVE_OX    =    0.0f, CAVE_OZ    =    0.0f;
static constexpr float GRAVEL_OX  =  317.3f, GRAVEL_OZ  =  179.1f;
static constexpr float JAGGED_OX  =  423.7f, JAGGED_OZ  =   83.5f;
static constexpr float EROSION_OX = 1231.7f, EROSION_OZ =  947.3f;

// ----------------------------------------------------------------------------
// Noise helpers
// Each maps a JJTH "noise density function" call:
//   { type: "noise", noise: "minecraft:<name>", xz_scale: S, y_scale: 0 }
// to a 2-D evaluation using our simplex/fbm primitives.
// ----------------------------------------------------------------------------

// cave_layer: 2-octave FBM (amplitudes [1,1] → persistence ≈ 0.5)
static inline float cave_n(float x, float z, float xz_scale) {
    float f = xz_scale * CAVE_BASE;
    return fbm((x + CAVE_OX) * f, (z + CAVE_OZ) * f, 2, 0.5f, 2.0f);
}

// gravel: same structure as cave_layer but different spatial seed
static inline float gravel_n(float x, float z, float xz_scale) {
    float f = xz_scale * GRAVEL_BASE;
    return fbm((x + GRAVEL_OX) * f, (z + GRAVEL_OZ) * f, 2, 0.5f, 2.0f);
}

// jagged: single-octave simplex (firstOctave=-16, amplitudes=[1])
static inline float jagged_n(float x, float z, float xz_scale) {
    float f = xz_scale * JAGGED_BASE;
    return simplex2((x + JAGGED_OX) * f, (z + JAGGED_OZ) * f);
}

// erosion_med: 4-octave FBM (amplitudes [1,1,0,1,1] ≈ 4 active octaves)
static inline float erosion_n(float x, float z, float xz_scale) {
    float f = xz_scale * EROSION_BASE;
    return fbm((x + EROSION_OX) * f, (z + EROSION_OZ) * f, 4, 0.5f, 2.0f);
}

// ----------------------------------------------------------------------------
// eroded_noise_tuned
//
// Ports: realism/*/factor/eroded_noise_tuned.json
//
// Algorithm (from realism/mountains/factor/ sub-tree):
//   original          = gravel_n(x, z, gravel_scale)
//   derivative_x/z    = central-difference gradient of original
//   divergence_sq     = (dx² + dz²) * 5000
//   erosion_trick     ≈ 1 / (1 + divergence_sq * 0.2)   [matches the spline]
//   eroded_noise      = original * erosion_trick
//   eroded_noise_tuned= eroded_noise * tune_factor
//
// The erosion_trick spline values match 1/(1+x) very closely over [0..1.5]:
//   x=0 → 1.0,  x=0.1 → 0.909,  x=0.5 → 0.667,  x=1.0 → 0.5  ✓
// ----------------------------------------------------------------------------
static float eroded_noise_tuned(float x, float z, float gravel_scale, float tune) {
    float gc = gravel_n(x,     z,     gravel_scale);
    float gx = gravel_n(x + 1, z,     gravel_scale);
    float gz = gravel_n(x,     z + 1, gravel_scale);
    float dx = gx - gc;   // forward difference, step = 1 block
    float dz = gz - gc;
    float div_sq = (dx * dx + dz * dz) * 5000.0f;
    float erosion_trick = 1.0f / (1.0f + div_sq * 0.2f);
    return gc * erosion_trick * tune;
}

// ----------------------------------------------------------------------------
// jagged_adjusted
//
// Ports: realism/*/jagged/adjusted.json
//
// The spline maps factor/original (gravel) ∈ [-0.3, 0.3] → a derivative
// that scales the jagged noise:
//   D = lerp(t, min_deriv, max_deriv),  t = clamp((gravel+0.3)/0.6, 0, 1)
//   result = jagged_noise * D
// ----------------------------------------------------------------------------
static inline float jagged_adjusted(float x, float z,
                                     float gravel_scale, float jagged_scale,
                                     float min_deriv, float max_deriv) {
    float g = gravel_n(x, z, gravel_scale);
    float t = clamp((g + 0.3f) / 0.6f, 0.0f, 1.0f);
    float D = lerp(t, min_deriv, max_deriv);
    return jagged_n(x, z, jagged_scale) * D;
}

// ============================================================================
// Per-terrain surface densities
// Returns a raw density value d where:
//   block_height = (1 + d) * (CHUNK_HEIGHT/2) + MIN_Y
// ============================================================================

// extreme_mountains/slope/height.json
//   offset/extreme_mountains : 0.3 + cave(0.25)*0.2
//   eroded_noise_tuned       : gravel(0.12) * erosion_trick * 0.8
//   jagged/adjusted          : gravel(0.12), jagged(600), deriv [0.04, 0.05]
static float height_extreme_mountains(float x, float z) {
    float off = 0.3f + cave_n(x, z, 0.25f) * 0.2f;
    float fac = eroded_noise_tuned(x, z, 0.12f, 0.8f);
    float jag = jagged_adjusted(x, z, 0.12f, 600.0f, 0.04f, 0.05f);
    return off + fac + jag;
}

// mountains/slope/height.json
//   offset/mountains   : 0.0 + cave(0.3)*0.2
//   eroded_noise_tuned : gravel(0.11) * erosion_trick * 0.7
//   jagged/adjusted    : gravel(0.11), jagged(501), deriv [0.035, 0.045]
static float height_mountains(float x, float z) {
    float off = 0.0f + cave_n(x, z, 0.3f) * 0.2f;
    float fac = eroded_noise_tuned(x, z, 0.11f, 0.7f);
    float jag = jagged_adjusted(x, z, 0.11f, 501.0f, 0.035f, 0.045f);
    return off + fac + jag;
}

// extreme_hills/slope/height.json
//   offset/extreme_hills : -0.2 + cave(0.2)*0.4
//   eroded_noise_tuned   : gravel(0.25) * erosion_trick * 0.3
//   jagged/adjusted      : gravel(0.25), jagged(300), deriv [0.03, 0.06]
static float height_extreme_hills(float x, float z) {
    float off = -0.2f + cave_n(x, z, 0.2f) * 0.4f;
    float fac = eroded_noise_tuned(x, z, 0.25f, 0.3f);
    float jag = jagged_adjusted(x, z, 0.25f, 300.0f, 0.03f, 0.06f);
    return off + fac + jag;
}

// hills/slope/height.json
//   offset/hills       : -0.5 + cave(0.15)*0.25
//   eroded_noise_tuned : (extreme_hills gravel scale 0.25) * erosion_trick * 0.2
//                        (JJTH reuses extreme_hills eroded_noise in hills)
//   jagged/adjusted    : gravel(0.2), jagged(250), deriv [0.03, 0.06]
static float height_hills(float x, float z) {
    float off = -0.5f + cave_n(x, z, 0.15f) * 0.25f;
    float fac = eroded_noise_tuned(x, z, 0.25f, 0.2f);   // same gravel freq as extreme_hills
    float jag = jagged_adjusted(x, z, 0.2f, 250.0f, 0.03f, 0.06f);
    return off + fac + jag;
}

// Plains: offset/plains + factor/plains + jagged/plains
//   offset/plains  : -0.8 + cave(0.1)*0.1
//   factor/plains  : cave(0.1)*0  → 0
//   jagged/plains  : jagged(500)*0.015
static float height_plains(float x, float z) {
    float off = -0.8f + cave_n(x, z, 0.1f) * 0.1f;
    float jag = jagged_n(x, z, 500.0f) * 0.015f;
    return off + jag;
}

// Ocean: offset/ocean + factor/ocean + jagged/ocean
//   offset/ocean : -0.9 + cave(0.2)*0.09
//   factor/ocean : cave(0.05)*0.05
//   jagged/ocean : jagged(175)*0.015
static float height_ocean(float x, float z) {
    float off = -0.9f + cave_n(x, z, 0.2f) * 0.09f;
    float fac = cave_n(x, z, 0.05f) * 0.05f;
    float jag = jagged_n(x, z, 175.0f) * 0.015f;
    return off + fac + jag;
}

// ============================================================================
// Smart Determiner
// Ports: determiner/overworld/smart.json
//
// det1  = cave_layer at xz_scale=0.006  (large-scale continental noise)
// smart = det1 in stable zones, det1 + gravel(0.008)*0.5 in transition bands
//
// Transition bands where noise detail is blended in:
//   |det1| < 0.20 → fully noisy  (det1 + detail)
//   |det1| ∈ [0.20, 0.35] → fade from noisy to raw
//   |det1| > 0.35 → raw det1
// ============================================================================
static float smart_determiner(float x, float z) {
    float det1   = cave_n(x, z, 0.006f);
    float detail = gravel_n(x, z, 0.008f) * 0.5f;

    float abs1  = fabsf(det1);
    float blend = 0.0f;
    if      (abs1 < 0.20f) blend = 1.0f;
    else if (abs1 < 0.35f) blend = 1.0f - smoothstep(0.20f, 0.35f, abs1);

    return det1 + detail * blend;
}

// ============================================================================
// Unrivered height spline
// Ports: new_surface/height/unprocessed/unrivered.json
//
// Spline over abs(smart_det):
//   abs_det  terrain type          breakpoint
//   0.000    extreme_mountains     0.000
//   0.075    mountains             0.075
//   0.150    extreme_hills         0.150
//   0.225    hills                 0.225
//   0.300    plains                0.300
//   0.400    ocean                 0.400
//
// Only the two bounding terrain types are evaluated (lazy evaluation).
// ============================================================================
static float unrivered_height(float x, float z, float abs_det) {
    static constexpr float BPT[6] = { 0.000f, 0.075f, 0.150f, 0.225f, 0.300f, 0.400f };

    if (abs_det <= BPT[0]) return height_extreme_mountains(x, z);
    if (abs_det >= BPT[5]) return height_ocean(x, z);

    float h_lo, h_hi, lo, hi;

    if (abs_det < BPT[1]) {
        lo = BPT[0]; hi = BPT[1];
        h_lo = height_extreme_mountains(x, z);
        h_hi = height_mountains(x, z);
    } else if (abs_det < BPT[2]) {
        lo = BPT[1]; hi = BPT[2];
        h_lo = height_mountains(x, z);
        h_hi = height_extreme_hills(x, z);
    } else if (abs_det < BPT[3]) {
        lo = BPT[2]; hi = BPT[3];
        h_lo = height_extreme_hills(x, z);
        h_hi = height_hills(x, z);
    } else if (abs_det < BPT[4]) {
        lo = BPT[3]; hi = BPT[4];
        h_lo = height_hills(x, z);
        h_hi = height_plains(x, z);
    } else {
        lo = BPT[4]; hi = BPT[5];
        h_lo = height_plains(x, z);
        h_hi = height_ocean(x, z);
    }

    float t = (abs_det - lo) / (hi - lo);
    return lerp(t, h_lo, h_hi);
}

// ============================================================================
// Rivered height
// Ports: new_surface/height/unprocessed/rivered.json
//
// Rivers are carved where abs_det ∈ [0.30, 0.34] (plains/ocean transition):
//   abs_det = 0.30 → unrivered (plains surface)
//   abs_det = 0.32 → -0.91     (river trough, below sea level)
//   abs_det = 0.34 → unrivered (back to terrain)
// ============================================================================
static float rivered_height(float x, float z, float abs_det) {
    float unriv = unrivered_height(x, z, abs_det);

    if (abs_det <= 0.300f || abs_det >= 0.340f) return unriv;

    static constexpr float RIVER_TROUGH = -0.91f;
    float river_blend;
    if (abs_det < 0.320f)
        river_blend = smoothstep(0.300f, 0.320f, abs_det);
    else
        river_blend = 1.0f - smoothstep(0.320f, 0.340f, abs_det);

    return lerp(river_blend, unriv, RIVER_TROUGH);
}

// ============================================================================
// calculate_pixel  —  main terrain entry point
// ============================================================================
TerrainResult TerrainGen::calculate_pixel(int worldX, int worldZ) {
    const float fx = (float)worldX;
    const float fz = (float)worldZ;

    // 1. Continental-scale smart determiner
    float det     = smart_determiner(fx, fz);
    float abs_det = fabsf(det);

    // 2. Rivered surface density  (unit-less, roughly in [-1.0, 0.8])
    float sd = rivered_height(fx, fz, abs_det);

    // 3. Density → block height
    //    h = (1 + sd) * (CHUNK_HEIGHT / 2) + MIN_Y
    //    calibrated for Canalize's 1808-block world (half = 904)
    const float HALF = (float)CHUNK_HEIGHT * 0.5f;
    int height = (int)((1.0f + sd) * HALF) + MIN_Y;
    height = std::max(MIN_Y + 1, std::min(height, MIN_Y + CHUNK_HEIGHT - 1));

    // 4. Biome — derived from surface density and erosion signal
    //    erosion = erosion_med(0.15) + cave_layer(20)*0.1
    float erosion = erosion_n(fx, fz, 0.15f) + cave_n(fx, fz, 20.0f) * 0.1f;

    int biomeId;
    if      (sd <  -0.88f)                       biomeId = 0;   // deep ocean
    else if (sd <  -0.50f)                       biomeId = 0;   // ocean
    else if (sd <  -0.30f)                       biomeId = 1;   // plains (coastal)
    else if (sd <   0.05f)                       biomeId = 1;   // plains
    else if (sd <   0.20f)                       biomeId = 100; // extreme hills
    else if (erosion < 0.0f && sd >= 0.05f)      biomeId = 200; // mountains (sharp)
    else                                          biomeId = 100; // rounded hills

    return { height, biomeId };
}

// ============================================================================
// generate_base_chunk  —  fills a 16×16×CHUNK_HEIGHT block buffer
// Layout: buffer[(x*16+z)*CHUNK_HEIGHT + (y - MIN_Y)]
// ============================================================================
void TerrainGen::generate_base_chunk(int chunkX, int chunkZ, int* buffer) {
    const int startX = chunkX * 16, startZ = chunkZ * 16;

    // Evaluate 18×18 grid (1-block border for 3×3 smoothing)
    int rawHeight[18][18], rawBiome[18][18];
    for (int x = -1; x < 17; x++) {
        for (int z = -1; z < 17; z++) {
            TerrainResult tr = calculate_pixel(startX + x, startZ + z);
            rawHeight[x + 1][z + 1] = tr.height;
            rawBiome [x + 1][z + 1] = tr.biomeId;
        }
    }

    // 3×3 box-filter smoothing to reduce blocky transitions
    int smoothHeight[16][16];
    for (int x = 0; x < 16; x++) {
        for (int z = 0; z < 16; z++) {
            int sum = 0;
            for (int dx = -1; dx <= 1; dx++)
                for (int dz = -1; dz <= 1; dz++)
                    sum += rawHeight[x + 1 + dx][z + 1 + dz];
            smoothHeight[x][z] = sum / 9;
        }
    }

    // Fill block columns
    for (int x = 0; x < 16; x++) {
        for (int z = 0; z < 16; z++) {
            int height = smoothHeight[x][z];
            int biome  = rawBiome[x + 1][z + 1];

            height = std::max(MIN_Y + 1,           height);
            height = std::min(MIN_Y + CHUNK_HEIGHT - 1, height);

            const int col  = (x * 16 + z) * CHUNK_HEIGHT;
            const int topY = height - MIN_Y;

            // Bedrock floor
            buffer[col] = BLOCK_BEDROCK;

            // Stone interior (AVX2 SIMD fill for performance)
            __m256i vs = _mm256_set1_epi32(BLOCK_STONE);
            int y = 1;
            for (; y <= topY - 8; y += 8)
                _mm256_storeu_si256((__m256i*)&buffer[col + y], vs);
            for (; y < topY; y++)
                buffer[col + y] = BLOCK_STONE;

            // Surface block selection (height thresholds scaled for 1808-tall world)
            //   ≥1200 blocks : snow caps
            //   ≥ 400        : bare stone (alpine)
            //   60–65        : sand (beach / shoreline)
            //   otherwise    : grass
            int surf;
            if      (height >= 1200)               surf = BLOCK_SNOW;
            else if (height >=  400)               surf = BLOCK_STONE;
            else if (height >=  60 && height < 65) surf = BLOCK_SAND;
            else                                    surf = BLOCK_GRASS;

            if (height >= 63) {
                buffer[col + topY] = surf;
                if (surf == BLOCK_GRASS && topY > 3) {
                    buffer[col + topY - 1] = BLOCK_DIRT;
                    buffer[col + topY - 2] = BLOCK_DIRT;
                }
            } else {
                buffer[col + topY] = BLOCK_DIRT;
            }

            // Sea-level water fill
            const int seaY = 63 - MIN_Y;
            for (int wy = topY + 1; wy <= seaY; wy++)
                buffer[col + wy] = BLOCK_WATER;
        }
    }
}
