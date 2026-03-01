package cn.sanrol.canalize.mixin;

import cn.sanrol.canalize.world.NativeChunkGenerator;
import com.mojang.serialization.Lifecycle;
import net.minecraft.core.MappedRegistry;
import net.minecraft.core.RegistrationInfo;
import net.minecraft.core.Registry;
import net.minecraft.core.RegistryAccess;
import net.minecraft.core.registries.Registries;
import net.minecraft.resources.ResourceKey;
import net.minecraft.world.level.biome.Biome;
import net.minecraft.world.level.biome.Biomes;
import net.minecraft.world.level.biome.FixedBiomeSource;
import net.minecraft.world.level.dimension.LevelStem;
import net.minecraft.world.level.levelgen.WorldDimensions;
import net.minecraft.world.level.levelgen.presets.WorldPresets;
import org.spongepowered.asm.mixin.Mixin;
import org.spongepowered.asm.mixin.injection.At;
import org.spongepowered.asm.mixin.injection.Inject;
import org.spongepowered.asm.mixin.injection.callback.CallbackInfoReturnable;

import java.util.Map;
import java.util.Optional;

@Mixin(WorldPresets.class)
public class MixinWorldPresets {

    @Inject(method = "createNormalWorldDimensions", at = @At("RETURN"), cancellable = true)
    private static void canalize$hijackWorldDimensions(RegistryAccess registryAccess, CallbackInfoReturnable<WorldDimensions> cir) {
        WorldDimensions original = cir.getReturnValue();
        if (original == null) return;

        // Use 'var' because the return type might be Map or Registry depending on mappings/version
        // Error log indicated it returns Map<ResourceKey<LevelStem>, LevelStem>
        var originalDimensions = original.dimensions();
        
        // Create a new mutable registry to replace the original one
        MappedRegistry<LevelStem> newRegistry = new MappedRegistry<>(Registries.LEVEL_STEM, Lifecycle.experimental());

        // Iterate entries
        for (var entry : originalDimensions.entrySet()) {
            ResourceKey<LevelStem> key = entry.getKey();
            LevelStem stem = entry.getValue();

            if (key.equals(LevelStem.OVERWORLD)) {
                System.out.println("[Canalize] HIJACKING Overworld generation with NativeChunkGenerator!");
                
                // Fetch a valid registered biome (Plains) to avoid "Unregistered holder" crash on save
                var biomeRegistry = registryAccess.lookupOrThrow(Registries.BIOME);
                var plainsHolder = biomeRegistry.getOrThrow(Biomes.PLAINS);
                
                // Create NativeChunkGenerator with FixedBiomeSource using the valid holder
                NativeChunkGenerator nativeGen = new NativeChunkGenerator(new FixedBiomeSource(plainsHolder));
                
                // Create a new LevelStem with the original DimensionType but our NativeChunkGenerator
                LevelStem newStem = new LevelStem(stem.type(), nativeGen);
                
                newRegistry.register(key, newStem, new RegistrationInfo(Optional.empty(), Lifecycle.stable()));
            } else {
                newRegistry.register(key, stem, new RegistrationInfo(Optional.empty(), Lifecycle.stable()));
            }
        }
        
        // Freeze the registry to make it safe for use
        newRegistry.freeze();

        // Return the new WorldDimensions containing our hijacked registry
        cir.setReturnValue(new WorldDimensions(newRegistry));
    }
}
