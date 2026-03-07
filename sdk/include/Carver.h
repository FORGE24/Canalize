#pragma once

class Carver {
public:
    // Apply caves to the generated chunk buffer
    static void carve_chunk(int chunkX, int chunkZ, int* buffer);
    
private:
    // Helper to determine if a block at (x, y, z) should be air
    static bool is_cave(int worldX, int worldY, int worldZ);
};
