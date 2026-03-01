package cn.sanrol.canalize.world;

import com.mojang.serialization.MapCodec;
import net.minecraft.core.BlockPos;
import net.minecraft.core.Holder;
import net.minecraft.core.registries.Registries;
import net.minecraft.server.level.WorldGenRegion;
import net.minecraft.world.level.LevelHeightAccessor;
import net.minecraft.world.level.NoiseColumn;
import net.minecraft.world.level.StructureManager;
import net.minecraft.world.level.biome.Biome;
import net.minecraft.world.level.biome.BiomeGenerationSettings;
import net.minecraft.world.level.biome.BiomeSource;
import net.minecraft.world.level.block.Blocks;
import net.minecraft.world.level.block.state.BlockState;
import net.minecraft.world.level.chunk.ChunkAccess;
import net.minecraft.world.level.chunk.ChunkGenerator;
import net.minecraft.world.level.levelgen.GenerationStep;
import net.minecraft.world.level.levelgen.Heightmap;
import net.minecraft.world.level.levelgen.RandomState;
import net.minecraft.world.level.levelgen.blending.Blender;

import net.minecraft.world.level.biome.BiomeManager;
import net.minecraft.world.level.biome.FixedBiomeSource;
import net.minecraft.core.registries.BuiltInRegistries;
import net.minecraft.world.level.biome.Biomes;

import java.util.List;
import net.minecraft.world.level.ChunkPos;

import java.util.concurrent.CompletableFuture;
import java.util.concurrent.Executor;
import java.util.function.Function;

public class NativeChunkGenerator extends ChunkGenerator {

    // Simple codec that doesn't save state for now
    public static final MapCodec<NativeChunkGenerator> CODEC = MapCodec.unit(NativeChunkGenerator::new);

    public NativeChunkGenerator(BiomeSource biomeSource) {
        super(biomeSource);
    }

    public NativeChunkGenerator() {
        // Use a dummy fixed biome source (Plains)
        // Note: In 1.21, we cannot easily access Biome registry statically in constructor without context.
        // We will use a temporary placeholder or pass it in via codec.
        // For the no-arg constructor used by codec, we should probably throw an error or use a safe default if possible.
        // However, FixedBiomeSource requires a Holder<Biome>.
        // Let's use a raw approach or reflection if needed, but better to avoid no-arg constructor logic that depends on registries.
        // Actually, we can just pass null for now if we don't use it, but super() might crash.
        // The proper way is to have the codec provide the biome source.
        
        // WORKAROUND: Create a dummy biome source that doesn't rely on the registry yet, 
        // or just accept that this constructor is only for the codec which will overwrite fields.
        super(null); 
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
        generateSurfaceNative(chunk);
    }

    @Override
    public void spawnOriginalMobs(WorldGenRegion region) {
        // Hijack: Native spawn logic or skip
    }

    @Override
    public int getGenDepth() {
        return 384; // Standard Overworld depth
    }

    @Override
    public CompletableFuture<ChunkAccess> fillFromNoise(Executor executor, Blender blender, RandomState random, StructureManager structureManager, ChunkAccess chunk) {
        // HIJACK: This is the main terrain generation
        return CompletableFuture.supplyAsync(() -> {
            generateNoiseNative(chunk);
            return chunk;
        }, executor);
    }

    @Override
    public int getSeaLevel() {
        return 63;
    }

    @Override
    public int getMinY() {
        return -64;
    }

    @Override
    public int getBaseHeight(int x, int z, Heightmap.Types type, LevelHeightAccessor level, RandomState random) {
        return 64; // Dummy height
    }

    @Override
    public NoiseColumn getBaseColumn(int x, int z, LevelHeightAccessor level, RandomState random) {
        return new NoiseColumn(0, new BlockState[0]);
    }

    @Override
    public void addDebugScreenInfo(List<String> info, RandomState random, BlockPos pos) {
        info.add("Native Chunk Generator: Active");
    }

    // --- Native Methods ---

    // Helper method to be called from C++ to set a block easily
    public void setBlockNativeHelper(ChunkAccess chunk, int localX, int y, int localZ) {
        // Convert local chunk coordinates (0-15) to absolute world coordinates
        BlockPos pos = chunk.getPos().getBlockAt(localX, y, localZ);
        chunk.setBlockState(pos, Blocks.STONE.defaultBlockState(), false);
    }

    // Pass the chunk object reference to native code
    // Native code will need to use JNI to call methods on ChunkAccess or direct memory access if using Unsafe/raw pointers (dangerous but fast)
    private native void generateNoiseNative(ChunkAccess chunk);

    private native void generateSurfaceNative(ChunkAccess chunk);
}
