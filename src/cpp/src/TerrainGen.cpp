#include "../include/TerrainGen.h"
#include "../include/MathUtil.h"
#include <vector>
#include <cmath>

// Constants for block IDs
const int BLOCK_AIR     = 0;
const int BLOCK_STONE   = 1;
const int BLOCK_WATER   = 2;
const int BLOCK_DIRT    = 3;
const int BLOCK_GRASS   = 4;
const int BLOCK_BEDROCK = 5;
const int BLOCK_SAND    = 6;
const int BLOCK_SNOW    = 7;
const int BLOCK_ICE     = 8;

TerrainResult TerrainGen::calculate_pixel(int worldX, int worldZ){

    // -----------------------------------------------------------------------
    // SCALE CONSTANTS
    // -----------------------------------------------------------------------
    const float CONT_FREQ   = 0.00003f;
    const float MOUNT_FREQ  = 0.000025f;   
    const float SAMPLE_STEP = 24.0f;
    const float PEAK_POW    = 2.2f;        

    // -----------------------------------------------------------------------
    // 1. CONTINENTALNESS 
    // -----------------------------------------------------------------------
    float warpX = fbm(worldX*CONT_FREQ+31.7f,  worldZ*CONT_FREQ+71.3f,  2,0.4f,2.01f);
    float warpZ = fbm(worldX*CONT_FREQ+200.3f, worldZ*CONT_FREQ+143.7f, 2,0.4f,2.01f);
    float contWarped = fbm(
        worldX*CONT_FREQ + warpX*0.3f,
        worldZ*CONT_FREQ + warpZ*0.3f,
        3, 0.5f, 2.01f
    );

    // -----------------------------------------------------------------------
    // 2. REGIONAL VARIETY
    // -----------------------------------------------------------------------
    const int REGION_SIZE = 5000;
    int regX = (int)floorf((float)worldX / (float)REGION_SIZE);
    int regZ = (int)floorf((float)worldZ / (float)REGION_SIZE);

    float reg_pers = region_val(regX, regZ, 7,  0.25f, 0.45f);
    float reg_lac  = region_val(regX, regZ, 13, 1.90f, 2.30f);

    float cellFracX = (float)(worldX - regX*REGION_SIZE) / (float)REGION_SIZE;
    float cellFracZ = (float)(worldZ - regZ*REGION_SIZE) / (float)REGION_SIZE;
    float bx = smoothstep(0.4f, 0.6f, cellFracX);
    float bz = smoothstep(0.4f, 0.6f, cellFracZ);

    float reg_pers_xp = region_val(regX+1, regZ,   7,  0.25f, 0.45f);
    float reg_pers_zp = region_val(regX,   regZ+1, 7,  0.25f, 0.45f);
    float reg_pers_xz = region_val(regX+1, regZ+1, 7,  0.25f, 0.45f);
    float reg_lac_xp  = region_val(regX+1, regZ,   13, 1.90f, 2.30f);
    float reg_lac_zp  = region_val(regX,   regZ+1, 13, 1.90f, 2.30f);
    float reg_lac_xz  = region_val(regX+1, regZ+1, 13, 1.90f, 2.30f);

    float pers = lerp(bz, lerp(bx, reg_pers, reg_pers_xp),
                          lerp(bx, reg_pers_zp, reg_pers_xz));
    float lac  = lerp(bz, lerp(bx, reg_lac,  reg_lac_xp),
                          lerp(bx, reg_lac_zp,  reg_lac_xz));

    // -----------------------------------------------------------------------
    // 3. OCEAN
    // -----------------------------------------------------------------------
    float hOcean = 40.0f + fbm(worldX*0.001f, worldZ*0.001f, 3, 0.5f, 2.0f)*20.0f;

    // -----------------------------------------------------------------------
    // 4. PLAINS
    // -----------------------------------------------------------------------
    float hPlains = 75.0f + fbm(worldX*0.0005f, worldZ*0.0005f, 4, 0.5f, 2.0f)*30.0f;

    // -----------------------------------------------------------------------
    // 5. MOUNTAIN SHAPE
    // -----------------------------------------------------------------------
    float mountBase = ridged_fbm(
        (float)worldX * MOUNT_FREQ,
        (float)worldZ * MOUNT_FREQ,
        3, pers, lac
    );

    float mbDx = ridged_fbm((worldX+SAMPLE_STEP)*MOUNT_FREQ, worldZ*MOUNT_FREQ,
                             3, pers, lac) - mountBase;
    float mbDz = ridged_fbm(worldX*MOUNT_FREQ, (worldZ+SAMPLE_STEP)*MOUNT_FREQ,
                             3, pers, lac) - mountBase;
    float slope      = sqrtf(mbDx*mbDx + mbDz*mbDz) / (MOUNT_FREQ*SAMPLE_STEP);
    float detailMask = clamp(1.0f - slope*1.5f, 0.0f, 1.0f);

    float dist     = worley(worldX*MOUNT_FREQ*4.0f, worldZ*MOUNT_FREQ*4.0f);
    float cellular = clamp(1.0f-dist, 0.0f, 1.0f);
    cellular       = cellular*cellular * smoothstep(0.55f, 1.0f, mountBase);

    float mountainShape = clamp(mountBase + cellular*0.15f, 0.0f, 1.0f);

    float valley    = fbm(worldX*MOUNT_FREQ*2.5f, worldZ*MOUNT_FREQ*2.5f, 2, 0.5f, 2.0f);
    float carveMask = smoothstep(0.2f, 0.7f, mountBase);
    mountainShape  -= fabsf(valley)*0.07f*carveMask;
    mountainShape   = clamp(mountainShape, 0.0f, 1.0f);

    float shapeCurved    = powf(mountainShape, PEAK_POW);
    float hMountainFloat = 120.0f + shapeCurved * 1500.0f;

    hMountainFloat += simplex2(worldX*0.003f, worldZ*0.003f)*5.0f*detailMask;

    // -----------------------------------------------------------------------
    // 6. PLAINS DETAIL
    // -----------------------------------------------------------------------
    float plainsMask = clamp(1.0f - mountainShape*2.5f, 0.0f, 1.0f);
    hPlains += simplex2(worldX*0.05f, worldZ*0.05f)*1.5f*plainsMask;

    // -----------------------------------------------------------------------
    // 7. BIOME BLENDING
    // -----------------------------------------------------------------------
    float tPlainsMount = smoothstep(0.4f, 0.9f, contWarped);
    float tOceanPlains = smoothstep(-0.2f, 0.0f, contWarped);
    float hOp         = lerp(tOceanPlains, hOcean, hPlains);
    float hFinalFloat = lerp(tPlainsMount, hOp, hMountainFloat);

    // -----------------------------------------------------------------------
    // 8. BIOME ID
    // -----------------------------------------------------------------------
    int biomeId;
    if      (hFinalFloat < 63.0f)                          biomeId = 0;
    else if (tPlainsMount > 0.8f)                          biomeId = 200;
    else if (tOceanPlains < 0.1f && hFinalFloat < 70.0f)  biomeId = 0;
    else                                                    biomeId = 1;

    return { (int)hFinalFloat, biomeId };
}

void TerrainGen::generate_base_chunk(int chunkX, int chunkZ, int* buffer){
    const int startX=chunkX*16, startZ=chunkZ*16;

    int rawHeight[18][18], rawBiome[18][18];
    for(int x=-1;x<17;x++) for(int z=-1;z<17;z++){
        TerrainResult tr=calculate_pixel(startX+x, startZ+z);
        rawHeight[x+1][z+1]=tr.height;
        rawBiome [x+1][z+1]=tr.biomeId;
    }

    int smoothHeight[16][16];
    for(int x=0;x<16;x++) for(int z=0;z<16;z++){
        int sum=0;
        for(int dx=-1;dx<=1;dx++) for(int dz=-1;dz<=1;dz++)
            sum+=rawHeight[x+1+dx][z+1+dz];
        smoothHeight[x][z]=sum/9;
    }

    for(int x=0;x<16;x++) for(int z=0;z<16;z++){
        int height=smoothHeight[x][z], biome=rawBiome[x+1][z+1];
        if(height<MIN_Y+1)             height=MIN_Y+1;
        if(height>=MIN_Y+CHUNK_HEIGHT) height=MIN_Y+CHUNK_HEIGHT-1;

        const int col=(x*16+z)*CHUNK_HEIGHT, topY=height-MIN_Y;
        buffer[col]=BLOCK_BEDROCK;

        __m256i vs=_mm256_set1_epi32(BLOCK_STONE);
        int y=1;
        for(;y<=topY-8;y+=8) _mm256_storeu_si256((__m256i*)&buffer[col+y],vs);
        for(;y<topY;y++) buffer[col+y]=BLOCK_STONE;

        int surf=BLOCK_GRASS;
        if(height>800) surf=BLOCK_SNOW;
        else if(height>250) surf=BLOCK_STONE;
        if(height>=60&&height<65) surf=BLOCK_SAND;

        if(height>=63){
            buffer[col+topY]=surf;
            if(surf==BLOCK_GRASS&&topY>3){
                buffer[col+topY-1]=BLOCK_DIRT;
                buffer[col+topY-2]=BLOCK_DIRT;
            }
        } else buffer[col+topY]=BLOCK_DIRT;

        const int seaY=63-MIN_Y;
        for(int wy=topY+1;wy<=seaY;wy++) buffer[col+wy]=BLOCK_WATER;
    }
}
