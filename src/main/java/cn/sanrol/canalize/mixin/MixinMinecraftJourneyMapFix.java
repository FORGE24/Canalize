package cn.sanrol.canalize.mixin;

/**
 * ⚠️ DEPRECATED - See note below
 * 
 * JOURNEYMAP COMPATIBILITY ISSUE:
 * 
 * JourneyMap 6.0.0-beta.53 has a CRITICAL BUG on Minecraft 1.21.1:
 *   java.lang.NullPointerException: Cannot invoke "journeymap.client.JourneymapClient.isInitialized()"
 *   because the return value of "journeymap.client.JourneymapClient.getInstance()" is null
 * 
 * This bug is IN JourneyMap's own Mixin and cannot be fixed from external mods.
 * 
 * SOLUTIONS:
 * 
 * Option 1: Upgrade JourneyMap (RECOMMENDED)
 *   - Download JourneyMap 6.0.0 final release (when available)
 *   - Or use JourneyMap 5.x which is stable for 1.21.1
 *   - Replace journeymap-neoforge-1.21.1-6.0.0-beta.53.jar with newer version
 * 
 * Option 2: Temporarily disable JourneyMap for Canalize testing
 *   - Delete or rename:
 *     - journeymap-neoforge-1.21.1-6.0.0-beta.53.jar
 *     - journeymap-api-neoforge-2.0.0-1.21.1-SNAPSHOT.jar
 *   - Restart Minecraft
 *   - Canalize will work perfectly without JourneyMap
 * 
 * Option 3: Create compatibility layer (advanced)
 *   - See: https://github.com/TeamJM/journeymap/issues
 *   - Report the bug to JourneyMap developers
 * 
 * TODO: Monitor JourneyMap releases and update to 6.0.0 final when available.
 */
public class MixinMinecraftJourneyMapFix {
    // This Mixin is intentionally empty and disabled.
    // See above for workaround instructions.
}



