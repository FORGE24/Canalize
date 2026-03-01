package cn.sanrol.canalize.mixin;

import net.minecraft.server.MinecraftServer;
import org.spongepowered.asm.mixin.Mixin;
import org.spongepowered.asm.mixin.injection.At;
import org.spongepowered.asm.mixin.injection.Inject;
import org.spongepowered.asm.mixin.injection.callback.CallbackInfo;

@Mixin(MinecraftServer.class)
public class MixinMinecraftServer {
    @Inject(method = "runServer", at = @At("HEAD"))
    private void onRunServer(CallbackInfo ci) {
        System.out.println("[Canalize] Hijacking Server Loop for Native Processing...");
        // This is where we could inject further native hooks or manage the native loop
    }
}
