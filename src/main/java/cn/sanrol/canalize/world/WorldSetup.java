package cn.sanrol.canalize.world;

import cn.sanrol.canalize.Canalize;
import net.neoforged.bus.api.SubscribeEvent;
import net.neoforged.fml.common.EventBusSubscriber;
import net.neoforged.neoforge.event.server.ServerStartedEvent;

/**
 * Alternative world setup handler.
 * 
 * Since MixinWorldPresets causes JourneyMap compatibility issues,
 * we use FML events instead to safely register custom terrain generation.
 */
@EventBusSubscriber(modid = Canalize.MODID)
public class WorldSetup {

    /**
     * Called when a new world is created or loaded.
     * This is where we could inject custom chunk generators.
     * 
     * TODO: Implement world generation override via world event system.
     * For now, users can manually select "Canalize" world preset in world creation screen.
     */
    @SubscribeEvent
    public static void onServerStarted(ServerStartedEvent event) {
        Canalize.LOGGER.info("[Canalize] Server started. Custom terrain generation available.");
        // TODO: Auto-apply Canalize terrain to default world if configured
    }
}
