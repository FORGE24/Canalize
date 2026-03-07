package cn.sanrol.canalize.world;

import com.mojang.serialization.MapCodec;
import com.mojang.serialization.codecs.RecordCodecBuilder;
import net.minecraft.core.Holder;
import net.minecraft.core.registries.Registries;
import net.minecraft.resources.ResourceKey;
import net.minecraft.resources.ResourceLocation;
import net.minecraft.world.level.biome.Biome;
import net.minecraft.world.level.biome.BiomeSource;
import net.minecraft.world.level.biome.Biomes;
import net.minecraft.world.level.biome.Climate;

import java.util.List;
import java.util.stream.Stream;

public class NativeBiomeSource extends BiomeSource {
    // Simplified approach: just store resource locations from JSON
    public static final MapCodec<NativeBiomeSource> CODEC = RecordCodecBuilder.mapCodec(instance ->
        instance.group(
            ResourceLocation.CODEC.listOf().fieldOf("biomes").forGetter(NativeBiomeSource::getBiomesAsResourceLocations)
        ).apply(instance, NativeBiomeSource::new)
    );

    private final List<ResourceLocation> biomeLocations;
    
    public NativeBiomeSource(List<ResourceLocation> biomeLocations) {
        super();
        this.biomeLocations = biomeLocations;
    }
    
    private List<ResourceLocation> getBiomesAsResourceLocations() {
        return biomeLocations;
    }

    @Override
    protected MapCodec<? extends BiomeSource> codec() {
        return CODEC;
    }

    @Override
    protected Stream<Holder<Biome>> collectPossibleBiomes() {
        // TODO: Properly resolve biomes from registry when available
        // For now, return empty stream - biomes will be resolved dynamically in getNoiseBiome
        return Stream.empty();
    }

    @Override
    public Holder<Biome> getNoiseBiome(int x, int y, int z, Climate.Sampler sampler) {
        // Since we switched to vanilla multi_noise biome source in the dimension JSON,
        // this shouldn't be called. But keep placeholder implementation.
        // Get biome from native function
        int blockX = x * 4;
        int blockZ = z * 4;
        int biomeId = getBiomeNative(blockX, blockZ);
        
        // Simple mapping - placeholder
        if (biomeId < 50) {
            return null; // Should not reach here with vanilla biome source
        } else if (biomeId < 150) {
            return null;
        } else {
            return null;
        }
    }

    private native int getBiomeNative(int x, int z);
}
