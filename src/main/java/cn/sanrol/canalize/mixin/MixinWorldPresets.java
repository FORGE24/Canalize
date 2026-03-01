package cn.sanrol.canalize.mixin;

import cn.sanrol.canalize.world.NativeBiomeSource;
import cn.sanrol.canalize.world.NativeChunkGenerator;
import com.mojang.serialization.Lifecycle;
import net.minecraft.core.Holder;
import net.minecraft.core.MappedRegistry;
import net.minecraft.core.RegistrationInfo;
import net.minecraft.core.RegistryAccess;
import net.minecraft.core.registries.Registries;
import net.minecraft.resources.ResourceKey;
import net.minecraft.world.level.biome.Biomes;
import net.minecraft.world.level.dimension.DimensionType;
import net.minecraft.world.level.dimension.LevelStem;
import net.minecraft.world.level.levelgen.WorldDimensions;
import net.minecraft.world.level.levelgen.presets.WorldPresets;
import org.spongepowered.asm.mixin.Mixin;
import org.spongepowered.asm.mixin.injection.At;
import org.spongepowered.asm.mixin.injection.Inject;
import org.spongepowered.asm.mixin.injection.callback.CallbackInfoReturnable;
import sun.misc.Unsafe;

import java.lang.reflect.Field;
import java.util.Optional;

@Mixin(WorldPresets.class)
public class MixinWorldPresets {

    @Inject(method = "createNormalWorldDimensions", at = @At("RETURN"), cancellable = true)
    private static void canalize$hijackWorldDimensions(RegistryAccess registryAccess, CallbackInfoReturnable<WorldDimensions> cir) {
        System.out.println("[Canalize] MixinWorldPresets: Hijack method entered!");
        WorldDimensions original = cir.getReturnValue();
        if (original == null) {
            System.out.println("[Canalize] Original WorldDimensions is null!");
            return;
        }

        var originalDimensions = original.dimensions();
        System.out.println("[Canalize] Found dimensions: " + originalDimensions.keySet());
        
        MappedRegistry<LevelStem> newRegistry = new MappedRegistry<>(Registries.LEVEL_STEM, Lifecycle.experimental());

        for (var entry : originalDimensions.entrySet()) {
            ResourceKey<LevelStem> key = entry.getKey();
            LevelStem stem = entry.getValue();

            if (key.equals(LevelStem.OVERWORLD)) {
                System.out.println("[Canalize] HIJACKING Overworld generation with NativeChunkGenerator!");
                
                var biomeRegistry = registryAccess.lookupOrThrow(Registries.BIOME);
                var plainsHolder = biomeRegistry.getOrThrow(Biomes.PLAINS);
                var oceanHolder = biomeRegistry.getOrThrow(Biomes.OCEAN);
                var mountainsHolder = biomeRegistry.getOrThrow(Biomes.JAGGED_PEAKS);
                
                // Use NativeBiomeSource to match C++ biome generation
                NativeBiomeSource nativeBiomeSource = NativeBiomeSource.create(plainsHolder, oceanHolder, mountainsHolder);
                System.out.println("[Canalize] Created NativeBiomeSource: " + nativeBiomeSource);
                
                NativeChunkGenerator nativeGen = new NativeChunkGenerator(nativeBiomeSource);
                System.out.println("[Canalize] Created NativeChunkGenerator: " + nativeGen);
                
                // HIJACK DIMENSION TYPE HEIGHT
                Holder<DimensionType> dimTypeHolder = stem.type();
                try {
                    DimensionType dimType = dimTypeHolder.value();
                    System.out.println("[Canalize] Modifying Overworld DimensionType height via Unsafe... Current: " + dimType.height());
                    
                    // Modify 'height' field (usually 384 -> 1808)
                    // Also 'logicalHeight' (usually 384 -> 1808)
                    // Using Unsafe to bypass 'final' restriction
                    
                    modifyFieldUnsafe(dimType, "height", 1808);
                    modifyFieldUnsafe(dimType, "logicalHeight", 1808);
                    modifyFieldUnsafe(dimType, "minY", -64); // Ensure min_y is correct too
                    
                    System.out.println("[Canalize] DimensionType height modified successfully to 1808! New Height: " + dimType.height());
                } catch (Exception e) {
                    System.err.println("[Canalize] Failed to modify DimensionType height!");
                    e.printStackTrace();
                }

                LevelStem newStem = new LevelStem(dimTypeHolder, nativeGen);
                
                newRegistry.register(key, newStem, new RegistrationInfo(Optional.empty(), Lifecycle.stable()));
            } else {
                newRegistry.register(key, stem, new RegistrationInfo(Optional.empty(), Lifecycle.stable()));
            }
        }
        
        newRegistry.freeze();

        cir.setReturnValue(new WorldDimensions(newRegistry));
        System.out.println("[Canalize] MixinWorldPresets: Hijack completed successfully!");
    }

    private static Unsafe getUnsafe() {
        try {
            Field f = Unsafe.class.getDeclaredField("theUnsafe");
            f.setAccessible(true);
            return (Unsafe) f.get(null);
        } catch (Exception e) {
            throw new RuntimeException("Could not get Unsafe", e);
        }
    }

    private static void modifyFieldUnsafe(Object target, String fieldName, int value) throws Exception {
        Class<?> clazz = target.getClass();
        Field field = null;
        
        // Try exact name first
        try {
            field = clazz.getDeclaredField(fieldName);
        } catch (NoSuchFieldException e) {
            System.err.println("Field not found: " + fieldName);
            // In obfuscated env, names might be different. 
            // But in NeoForge dev env, names should match mapping.
            // If this fails, we might need to search by type/value or use SRG names.
            throw e;
        }
        
        Unsafe unsafe = getUnsafe();
        long offset = unsafe.objectFieldOffset(field);
        unsafe.putInt(target, offset, value);
    }
}
