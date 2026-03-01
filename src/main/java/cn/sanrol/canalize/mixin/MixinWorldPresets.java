package cn.sanrol.canalize.mixin;

import cn.sanrol.canalize.world.NativeChunkGenerator;
import net.minecraft.core.Holder;
import net.minecraft.world.level.chunk.ChunkGenerator;
import net.minecraft.world.level.dimension.DimensionType;
import net.minecraft.world.level.dimension.LevelStem;
import net.minecraft.world.level.levelgen.presets.WorldPresets;
import org.spongepowered.asm.mixin.Mixin;
import org.spongepowered.asm.mixin.injection.At;
import org.spongepowered.asm.mixin.injection.Redirect;

@Mixin(WorldPresets.class)
public class MixinWorldPresets {
    /**
     * Redirects the construction of the LevelStem for the Overworld.
     * This intercepts the point where the Overworld dimension is assembled.
     */
    @Redirect(
        method = "createNormalWorldDimensions",
        at = @At(
            value = "NEW",
            target = "Lnet/minecraft/world/level/dimension/LevelStem;"
        )
    )
    private static LevelStem canalize$replaceChunkGenerator(Holder<DimensionType> dimensionType, ChunkGenerator originalGenerator) {
        // Since createNormalWorldDimensions calls this specifically for Overworld, Nether, and End,
        // we can filter based on dimensionType or just check if it's the Overworld generator being replaced.
        
        // A simple check: if the original generator is the standard NoiseBasedChunkGenerator, 
        // and we are creating the Overworld (which is the first one typically), we replace it.
        // Or better: check dimension type tags if possible, but here we only have the Holder.
        // For simplicity in this specific method, the first call is Overworld.
        
        // However, a safer bet is to check if the dimension type is "minecraft:overworld"
        // But the Holder might not be bound yet or we might not have easy access to the key.
        
        // Let's rely on the fact that we want to replace the Overworld generator.
        // The standard Overworld generator is passed here.
        
        // To be safe and target only Overworld, we can check if the generator is an instance of NoiseBasedChunkGenerator
        // and if the dimension type is capable of supporting it.
        
        // For now, let's just log and replace if it looks like the Overworld.
        // The createNormalWorldDimensions method structure is:
        // LevelStem overworld = new LevelStem(overworldType, overworldGen);
        // LevelStem nether = new LevelStem(netherType, netherGen);
        // LevelStem end = new LevelStem(endType, endGen);
        
        // We can check the generator settings or just blind replace.
        // Blind replace might break Nether/End if we are not careful.
        
        // Let's assume we want to replace EVERYTHING with our Native Generator for now (total conversion),
        // or check the dimension type if possible.
        
        // Actually, let's just check if the dimension type key path contains "overworld".
        // This is a bit hacky but works if the holder has a key.
        
        if (dimensionType.unwrapKey().isPresent() && dimensionType.unwrapKey().get().location().getPath().equals("overworld")) {
             System.out.println("[Canalize] Hijacking Overworld ChunkGenerator!");
             return new LevelStem(dimensionType, new NativeChunkGenerator());
        }
        
        return new LevelStem(dimensionType, originalGenerator);
    }
}
