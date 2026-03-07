#include <CanalizeSDK.h>
#include <canalize/forge/EventBus.h>
#include <canalize/forge/ChunkEvents.h>
#include <canalize/world/ChunkAccess.h>
#include <canalize/block/BlockState.h>
#include "../include/FastNoiseLite.h" // Local include
#include <memory>

using namespace Canalize;
using namespace Canalize::Forge;
using namespace Canalize::World;
using namespace Canalize::Block;

class TerrainOverhaulPlugin : public IPlugin {
    FastNoiseLite noise;

public:
    std::string getName() const override { return "TerrainOverhaul"; }
    std::string getVersion() const override { return "2.0.0-FORGE"; }

    void onInit() override {
        NativeLog::info("[TerrainOverhaul] Initialized with Forge-like API.");
        
        noise.SetFrequency(0.005f);
        noise.SetNoiseType(FastNoiseLite::NoiseType::OpenSimplex2);

        // Subscribe to generation event
        EventBus::getInstance().subscribe<ChunkGeneratorEvent>([this](ChunkGeneratorEvent& event) {
            this->onGenerate(event);
        });
    }

    // Since we use the EventBus now, we can technically ignore onPreGenerateChunk
    // OR we can fire the event FROM onPreGenerateChunk in the main mod.
    // BUT since we are a plugin, we can just use the hook directly if we want speed.
    // However, to demonstrate "NeoForge for C++", let's assume the main mod fires events.
    
    // Wait, the main mod doesn't fire events yet. We need to modify WorldLoader to fire events.
    // So for now, we will fire it ourselves or just use the hook to simulate it.
    
    bool onPreGenerateChunk(int chunkX, int chunkZ, int* buffer) override {
        // Create wrapper
        ChunkAccess chunkAccess(chunkX, chunkZ, buffer);
        
        // Fire the event manually to simulate the framework
        ChunkGeneratorEvent event(&chunkAccess, chunkX, chunkZ, buffer);
        EventBus::getInstance().post(event);
        
        return event.isCanceled(); // If canceled, it means "handled"
    }

    void onGenerate(ChunkGeneratorEvent& event) {
        // Use high-level API
        ChunkAccess* chunk = event.getChunk();
        if (!chunk) return;
        
        int width = 16;
        int totalHeight = 1808; 
        int minY = -64;

        for (int x = 0; x < width; x++) {
            for (int z = 0; z < width; z++) {
                // Generate column
                float n = noise.GetNoise((float)(chunk->getChunkX()*16 + x), (float)(chunk->getChunkZ()*16 + z));
                int height = 100 + (int)(n * 50.0f);

                for (int y = 0; y < totalHeight; y++) {
                    int currentY = minY + y;
                    BlockPos pos(x, currentY, z); 
                    
                    if (currentY <= height) {
                        if (currentY == height) 
                            chunk->setBlockState(pos, Blocks::GRASS);
                        else if (currentY > height - 4)
                            chunk->setBlockState(pos, Blocks::DIRT);
                        else
                            chunk->setBlockState(pos, Blocks::STONE);
                    } else if (currentY < 63) {
                         chunk->setBlockState(pos, Blocks::WATER);
                    }
                }
            }
        }
        
        // Mark as handled
        event.setCanceled(true); 
    }
};

CANALIZE_PLUGIN_ENTRY {
    manager.registerPlugin(std::make_shared<TerrainOverhaulPlugin>());
}
