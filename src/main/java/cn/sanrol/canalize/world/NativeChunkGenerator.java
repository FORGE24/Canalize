package cn.sanrol.canalize.world;

import cn.sanrol.canalize.Canalize;
import com.mojang.serialization.MapCodec;
import com.mojang.serialization.codecs.RecordCodecBuilder;
import net.minecraft.core.BlockPos;
import net.minecraft.network.chat.Component;
import net.minecraft.server.level.WorldGenRegion;
import net.minecraft.world.level.LevelHeightAccessor;
import net.minecraft.world.level.NoiseColumn;
import net.minecraft.world.level.StructureManager;
import net.minecraft.world.level.biome.BiomeManager;
import net.minecraft.world.level.biome.BiomeSource;
import net.minecraft.world.level.block.Blocks;
import net.minecraft.world.level.block.state.BlockState;
import net.minecraft.world.level.chunk.ChunkAccess;
import net.minecraft.world.level.chunk.ChunkGenerator;
import net.minecraft.world.level.levelgen.GenerationStep;
import net.minecraft.world.level.levelgen.Heightmap;
import net.minecraft.world.level.levelgen.RandomState;
import net.minecraft.world.level.ChunkPos;

import java.util.Arrays;
import java.util.List;
import java.util.concurrent.CompletableFuture;

public class NativeChunkGenerator extends ChunkGenerator {

    // 16x16 chunk
    // Extended height: 1808
    // Start Y: -64
    // End Y: 1744
    private static final int CHUNK_WIDTH = 16;
    private static final int CHUNK_HEIGHT = 1808;
    private static final int MIN_Y = -64;
    private static final int BUFFER_SIZE = CHUNK_WIDTH * CHUNK_WIDTH * CHUNK_HEIGHT;

    public static final MapCodec<NativeChunkGenerator> CODEC = RecordCodecBuilder.mapCodec(instance ->
        instance.group(
            BiomeSource.CODEC.fieldOf("biome_source").forGetter(ChunkGenerator::getBiomeSource)
        ).apply(instance, NativeChunkGenerator::new)
    );

    public NativeChunkGenerator(BiomeSource biomeSource) {
        super(biomeSource);
        System.out.println("[Canalize] NativeChunkGenerator INSTANTIATED!");
    }

    @Override
    protected MapCodec<? extends ChunkGenerator> codec() {
        return CODEC;
    }

    @Override
    public void applyCarvers(WorldGenRegion region, long seed, RandomState random, BiomeManager biomeManager, StructureManager structureManager, ChunkAccess chunk, GenerationStep.Carving step) {
    }

    @Override
    public void buildSurface(WorldGenRegion region, StructureManager structureManager, RandomState random, ChunkAccess chunk) {
    }

    @Override
    public void spawnOriginalMobs(WorldGenRegion region) {
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
        return getHeightNative(x, z);
    }

    @Override
    public NoiseColumn getBaseColumn(int x, int z, LevelHeightAccessor level, RandomState random) {
        int height = getHeightNative(x, z);
        int colHeight = Math.max(0, height - MIN_Y);
        // Cap column height to chunk height to avoid array index out of bounds if noise goes higher
        if (colHeight > CHUNK_HEIGHT) colHeight = CHUNK_HEIGHT;
        
        BlockState[] states = new BlockState[colHeight];
        Arrays.fill(states, Blocks.STONE.defaultBlockState());
        
        return new NoiseColumn(MIN_Y, states);
    }

    @Override
    public void addDebugScreenInfo(List<String> info, RandomState random, BlockPos pos) {
        info.add("Native Chunk Generator: Active (Grand Terrain, 1808 Height)");
    }

    private void generateNoiseNative(ChunkAccess chunk) {
        ChunkPos chunkPos = chunk.getPos();
        
        if (Canalize.CHUNK_LOAD_LOG) {
            String logMsg = String.format("[Canalize] Generating Chunk: [%d, %d] | MinY: %d | Height: %d | Buffer: %d", 
                chunkPos.x, chunkPos.z, MIN_Y, CHUNK_HEIGHT, BUFFER_SIZE);
            Canalize.LOGGER.info(logMsg);
        }

        int[] blockData = new int[BUFFER_SIZE];
        
        long startTime = System.nanoTime();
        generateChunkData(chunkPos.x, chunkPos.z, blockData);
        long duration = System.nanoTime() - startTime;
        
        if (Canalize.CHUNK_LOAD_LOG) {
            Canalize.LOGGER.info("[Canalize] Native generation took {} ns for chunk [{}, {}]", duration, chunkPos.x, chunkPos.z);
        }
        
        BlockPos.MutableBlockPos pos = new BlockPos.MutableBlockPos();
        int index = 0;
        
        BlockState stone = Blocks.STONE.defaultBlockState();
        BlockState water = Blocks.WATER.defaultBlockState();
        BlockState dirt = Blocks.DIRT.defaultBlockState();
        BlockState grass = Blocks.GRASS_BLOCK.defaultBlockState();
        BlockState bedrock = Blocks.BEDROCK.defaultBlockState();
        BlockState sand = Blocks.SAND.defaultBlockState();
        BlockState snow = Blocks.SNOW_BLOCK.defaultBlockState();
        BlockState ice = Blocks.ICE.defaultBlockState();
        BlockState coalOre = Blocks.COAL_ORE.defaultBlockState();
        BlockState ironOre = Blocks.IRON_ORE.defaultBlockState();
        BlockState diamondOre = Blocks.DIAMOND_ORE.defaultBlockState();
        BlockState oakLog = Blocks.OAK_LOG.defaultBlockState();
        BlockState oakLeaves = Blocks.OAK_LEAVES.defaultBlockState();
        
        for (int x = 0; x < 16; x++) {
            for (int z = 0; z < 16; z++) {
                for (int y = 0; y < CHUNK_HEIGHT; y++) {
                    int blockId = blockData[index++];
                    if (blockId != 0) {
                        int worldY = y + MIN_Y;
                        int absX = chunkPos.getMinBlockX() + x;
                        int absZ = chunkPos.getMinBlockZ() + z;
                        pos.set(absX, worldY, absZ);
                        
                        BlockState state = switch (blockId) {
                            case 1 -> stone;
                            case 2 -> water;
                            case 3 -> dirt;
                            case 4 -> grass;
                            case 5 -> bedrock;
                            case 6 -> sand;
                            case 7 -> snow;
                            case 8 -> ice;
                            case 9 -> coalOre;
                            case 10 -> ironOre;
                            case 11 -> diamondOre;
                            case 12 -> oakLog;
                            case 13 -> oakLeaves;
                            default -> stone;
                        };
                        chunk.setBlockState(pos, state, false);
                    }
                }
            }
        }
    }

    private native void generateChunkData(int chunkX, int chunkZ, int[] blockData);
    
    private native int getHeightNative(int x, int z);
}
