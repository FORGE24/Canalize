#pragma once

class Decorator {
public:
    // Add ores, trees, etc. to the generated chunk buffer
    static void decorate_chunk(int chunkX, int chunkZ, int* buffer);
    
private:
    static void place_ores(int worldX, int worldY, int worldZ, int& blockId);
    static void place_tree(int localX, int worldY, int localZ, int* buffer);
};
