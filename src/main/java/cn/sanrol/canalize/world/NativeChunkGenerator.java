package cn.sanrol.canalize.world;

import com.mojang.serialization.MapCodec;
import com.mojang.serialization.codecs.RecordCodecBuilder;
import net.minecraft.core.BlockPos;
import net.minecraft.core.Holder;
import net.minecraft.core.registries.BuiltInRegistries;
import net.minecraft.core.registries.Registries;
import net.minecraft.server.level.WorldGenRegion;
import net.minecraft.world.level.LevelHeightAccessor;
import net.minecraft.world.level.NoiseColumn;
import net.minecraft.world.level.StructureManager;
import net.minecraft.world.level.biome.Biome;
import net.minecraft.world.level.biome.BiomeGenerationSettings;
import net.minecraft.world.level.biome.BiomeManager;
import net.minecraft.world.level.biome.BiomeSource;
import net.minecraft.world.level.biome.Biomes;
import net.minecraft.world.level.biome.FixedBiomeSource;
import net.minecraft.world.level.biome.MobSpawnSettings;
import net.minecraft.world.level.block.Blocks;
import net.minecraft.world.level.block.state.BlockState;
import net.minecraft.world.level.chunk.ChunkAccess;
import net.minecraft.world.level.chunk.ChunkGenerator;
import net.minecraft.world.level.levelgen.GenerationStep;
import net.minecraft.world.level.levelgen.Heightmap;
import net.minecraft.world.level.levelgen.RandomState;
import net.minecraft.world.level.levelgen.blending.Blender;
import net.minecraft.world.level.ChunkPos;

import java.util.List;
import java.util.concurrent.CompletableFuture;

public class NativeChunkGenerator extends ChunkGenerator {

    // 16x16 chunk, height 384 (-64 to 320)
    // Size = 16 * 16 * 384 = 98304
    private static final int CHUNK_WIDTH = 16;
    private static final int CHUNK_HEIGHT = 384;
    private static final int MIN_Y = -64;
    private static final int BUFFER_SIZE = CHUNK_WIDTH * CHUNK_WIDTH * CHUNK_HEIGHT;

    // Codec that serializes the BiomeSource, ensuring registry safety
    public static final MapCodec<NativeChunkGenerator> CODEC = RecordCodecBuilder.mapCodec(instance ->
        instance.group(
            BiomeSource.CODEC.fieldOf("biome_source").forGetter(ChunkGenerator::getBiomeSource)
        ).apply(instance, NativeChunkGenerator::new)
    );

    public NativeChunkGenerator(BiomeSource biomeSource) {
        super(biomeSource);
    }

    @Override
    protected MapCodec<? extends ChunkGenerator> codec() {
        return CODEC;
    }

    @Override
    public void applyCarvers(WorldGenRegion region, long seed, RandomState random, BiomeManager biomeManager, StructureManager structureManager, ChunkAccess chunk, GenerationStep.Carving step) {
        // Hijack: Do nothing or native call
    }

    @Override
    public void buildSurface(WorldGenRegion region, StructureManager structureManager, RandomState random, ChunkAccess chunk) {
        // Surface building logic is usually biome-dependent (grass, sand, etc.)
        // We can do this in C++ too if we pass biome data, or keep it simple for now.
    }

    @Override
    public void spawnOriginalMobs(WorldGenRegion region) {
        // Hijack: Native spawn logic or skip
    }

    @Override
    public int getGenDepth() {
        return CHUNK_HEIGHT;
    }

    @Override
    public CompletableFuture<ChunkAccess> fillFromNoise(net.minecraft.world.level.levelgen.blending.Blender blender, net.minecraft.world.level.levelgen.RandomState random, net.minecraft.world.level.StructureManager structureManager, net.minecraft.world.level.chunk.ChunkAccess chunk) {
        return CompletableFuture.supplyAsync(() -> {
            generateNoiseNative(chunk);
            return chunk;
        });
    }

    @Override
    public int getSeaLevel() {
        return 63;
    }

    @Override
    public int getMinY() {
        return MIN_Y;
    }

    @Override
    public int getBaseHeight(int x, int z, Heightmap.Types type, LevelHeightAccessor level, RandomState random) {
        return 64; // Approximation for structure placement
    }

    @Override
    public NoiseColumn getBaseColumn(int x, int z, LevelHeightAccessor level, RandomState random) {
        return new NoiseColumn(0, new BlockState[0]);
    }

    @Override
    public void addDebugScreenInfo(List<String> info, RandomState random, BlockPos pos) {
        info.add("Native Chunk Generator: Active (Complex Terrain)");
    }

    // --- Native Integration ---

    // Java-side buffer to reduce allocation overhead if we reused it, 
    // but for thread safety in CompletableFuture, we allocate per chunk or use a ThreadLocal.
    // Allocating 400KB per chunk is fine.
    
    private void generateNoiseNative(ChunkAccess chunk) {
        ChunkPos chunkPos = chunk.getPos();
        int[] blockData = new int[BUFFER_SIZE];
        
        // Call C++ to fill the array with block IDs
        // 0 = AIR
        // 1 = STONE
        // 2 = WATER
        // 3 = DIRT
        // 4 = GRASS_BLOCK
        // 5 = BEDROCK
        generateChunkData(chunkPos.x, chunkPos.z, blockData);
        
        // Apply to chunk
        BlockPos.MutableBlockPos pos = new BlockPos.MutableBlockPos();
        int index = 0;
        
        BlockState stone = Blocks.STONE.defaultBlockState();
        BlockState water = Blocks.WATER.defaultBlockState();
        BlockState dirt = Blocks.DIRT.defaultBlockState();
        BlockState grass = Blocks.GRASS_BLOCK.defaultBlockState();
        BlockState bedrock = Blocks.BEDROCK.defaultBlockState();
        
        for (int x = 0; x < 16; x++) {
            for (int z = 0; z < 16; z++) {
                for (int y = 0; y < CHUNK_HEIGHT; y++) {
                    int blockId = blockData[index++];
                    if (blockId != 0) {
                        int worldY = y + MIN_Y;
                        pos.set(x, worldY, z); // Local coordinates in ChunkAccess?
                        // ChunkAccess.setBlockState takes global coordinates usually, 
                        // but let's check. Yes, BlockPos in setBlockState is usually absolute.
                        // However, ProtoChunk/ChunkAccess methods often handle conversion if using sections directly.
                        // Safe way: Use absolute coordinates.
                        int absX = chunkPos.getMinBlockX() + x;
                        int absZ = chunkPos.getMinBlockZ() + z;
                        pos.set(absX, worldY, absZ);
                        
                        BlockState state = switch (blockId) {
                            case 1 -> stone;
                            case 2 -> water;
                            case 3 -> dirt;
                            case 4 -> grass;
                            case 5 -> bedrock;
                            default -> stone;
                        };
                        chunk.setBlockState(pos, state, false);
                    }
                }
            }
        }
    }

    /**
     * Fills the provided integer array with block IDs.
     * Array layout: x first, then z, then y (or however C++ fills it).
     * Let's stick to: x outer, z middle, y inner (matching the loop above).
     * Size: 16 * 16 * 384
     */
    private native void generateChunkData(int chunkX, int chunkZ, int[] blockData);
}
