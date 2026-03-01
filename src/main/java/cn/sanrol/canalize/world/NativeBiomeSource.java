package cn.sanrol.canalize.world;

import com.mojang.serialization.MapCodec;
import com.mojang.serialization.codecs.RecordCodecBuilder;
import net.minecraft.core.Holder;
import net.minecraft.core.registries.Registries;
import net.minecraft.resources.ResourceKey;
import net.minecraft.world.level.biome.Biome;
import net.minecraft.world.level.biome.BiomeSource;
import net.minecraft.world.level.biome.Biomes;
import net.minecraft.world.level.biome.Climate;

import java.util.List;
import java.util.stream.Stream;

public class NativeBiomeSource extends BiomeSource {
    public static final MapCodec<NativeBiomeSource> CODEC = RecordCodecBuilder.mapCodec(instance ->
        instance.group(
            Biome.CODEC.listOf().fieldOf("biomes").forGetter(NativeBiomeSource::getBiomesList)
        ).apply(instance, NativeBiomeSource::new)
    );

    private final List<Holder<Biome>> biomes;
    
    // Cache common holders
    private Holder<Biome> plains;
    private Holder<Biome> ocean;
    private Holder<Biome> hills; // Windswept Hills or similar
    private Holder<Biome> mountains; // Jagged Peaks

    public NativeBiomeSource(List<Holder<Biome>> biomes) {
        super();
        this.biomes = biomes;
        // Initialize with default if list is empty or find specific ones?
        // For simplicity, we assume the list contains what we need or we look them up dynamically if possible?
        // BiomeSource constructor requires a list of possible biomes for features to know what to expect.
    }
    
    // Helper to construct with standard biomes
    public static NativeBiomeSource create(Holder.Reference<Biome> plains, Holder.Reference<Biome> ocean, Holder.Reference<Biome> mountains) {
        return new NativeBiomeSource(List.of(plains, ocean, mountains));
    }
    
    private List<Holder<Biome>> getBiomesList() {
        return this.biomes;
    }

    @Override
    protected MapCodec<? extends BiomeSource> codec() {
        return CODEC;
    }

    @Override
    protected Stream<Holder<Biome>> collectPossibleBiomes() {
        return biomes.stream();
    }

    @Override
    public Holder<Biome> getNoiseBiome(int x, int y, int z, Climate.Sampler sampler) {
        // x, y, z are in QUART_BLOCKS (4 blocks per unit) usually for getNoiseBiome, 
        // OR block coords depending on context. 
        // BiomeSource.getNoiseBiome receives quart coords.
        // We need to convert to block coords for our native function: x * 4, z * 4.
        
        int blockX = x * 4;
        int blockZ = z * 4;
        
        int biomeId = getBiomeNative(blockX, blockZ);
        
        // Map ID to Biome Holder
        // 0-50=Ocean, 51-150=Plains, 151-200=Hills, 201+=Mountains
        
        // We need to find the Holders in our list or have them injected.
        // Since we can't easily look up registries here without reference, 
        // we rely on the list passed in constructor containing them in order or by checking tags/keys.
        
        // For this proof of concept, we'll try to match by key if available, or just index.
        // Better: Pass specific holders in constructor.
        
        // Fallback
        if (biomes.isEmpty()) return null; // Should not happen
        
        // Simple mapping based on our known logic:
        if (biomeId < 50) {
            return findBiome(Biomes.OCEAN);
        } else if (biomeId < 150) {
            return findBiome(Biomes.PLAINS);
        } else {
            return findBiome(Biomes.JAGGED_PEAKS);
        }
    }
    
    private Holder<Biome> findBiome(ResourceKey<Biome> key) {
        for (Holder<Biome> b : biomes) {
            if (b.is(key)) return b;
        }
        return biomes.get(0); // Fallback
    }

    private native int getBiomeNative(int x, int z);
}
