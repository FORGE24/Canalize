package cn.sanrol.canalize;

import cn.sanrol.canalize.world.NativeQueryBridge;

import net.minecraft.ChatFormatting;
import net.minecraft.client.Minecraft;
import net.minecraft.network.chat.Component;
import net.minecraft.network.chat.MutableComponent;

import net.neoforged.api.distmarker.Dist;
import net.neoforged.bus.api.SubscribeEvent;
import net.neoforged.fml.ModContainer;
import net.neoforged.fml.common.EventBusSubscriber;
import net.neoforged.fml.event.lifecycle.FMLClientSetupEvent;
import net.neoforged.neoforge.client.event.ClientTickEvent;
import net.neoforged.neoforge.client.gui.ConfigurationScreen;
import net.neoforged.neoforge.client.gui.IConfigScreenFactory;

/**
 * Client-only mod entrypoint.
 *
 * Subscribes to the client tick to drain the native log queue and push
 * messages to the in-game chat when /canalize debug is enabled.
 */
@EventBusSubscriber(modid = Canalize.MODID, value = Dist.CLIENT)
public class CanalizeClient {

    // Drain interval: every DRAIN_INTERVAL_TICKS ticks (1 tick = 50 ms)
    // = 4 ticks → ~200 ms latency max
    private static final int DRAIN_INTERVAL_TICKS = 4;

    // Hard cap on messages shown per drain cycle to avoid chat flooding
    private static final int MAX_MSGS_PER_DRAIN = 20;

    // Tick counter (reset each drain)
    private static int s_tickCounter = 0;

    // Mod prefix shown before every native log message
    private static final MutableComponent PREFIX =
        Component.literal("[Canalize] ").withStyle(ChatFormatting.AQUA);

    public CanalizeClient(ModContainer container) {
        container.registerExtensionPoint(IConfigScreenFactory.class, ConfigurationScreen::new);
    }

    @SubscribeEvent
    static void onClientSetup(FMLClientSetupEvent event) {
        // Client setup (future: register renderers etc.)
    }

    // ------------------------------------------------------------------
    // Client tick — drain native log queue → chat
    // ------------------------------------------------------------------
    @SubscribeEvent
    public static void onClientTick(ClientTickEvent.Post event) {
        if (!Canalize.CHAT_DEBUG) return;

        s_tickCounter++;
        if (s_tickCounter < DRAIN_INTERVAL_TICKS) return;
        s_tickCounter = 0;

        String[] entries = NativeQueryBridge.drainLog();
        if (entries.length == 0) return;

        Minecraft mc = Minecraft.getInstance();
        if (mc.player == null) return;  // not in a world yet

        int shown = 0;
        int total = entries.length;

        for (String entry : entries) {
            if (shown >= MAX_MSGS_PER_DRAIN) break;

            int pipe = entry.indexOf('|');
            String level = (pipe > 0) ? entry.substring(0, pipe)  : "INFO";
            String text  = (pipe > 0) ? entry.substring(pipe + 1) : entry;

            ChatFormatting color = levelColor(level);
            ChatFormatting levelColor = levelBadgeColor(level);

            MutableComponent badge = Component.literal("[" + level + "] ")
                .withStyle(levelColor, ChatFormatting.BOLD);
            MutableComponent body  = Component.literal(text)
                .withStyle(color);

            mc.player.sendSystemMessage(PREFIX.copy().append(badge).append(body));
            shown++;
        }

        // If messages were truncated, show a summary line
        if (total > MAX_MSGS_PER_DRAIN) {
            int skipped = total - MAX_MSGS_PER_DRAIN;
            mc.player.sendSystemMessage(
                PREFIX.copy().append(
                    Component.literal("... +" + skipped + " more messages suppressed this cycle.")
                        .withStyle(ChatFormatting.DARK_GRAY)));
        }
    }

    // ------------------------------------------------------------------
    // Helpers
    // ------------------------------------------------------------------

    /** Text color for the message body. */
    private static ChatFormatting levelColor(String level) {
        return switch (level) {
            case "DEBUG" -> ChatFormatting.DARK_GRAY;
            case "INFO"  -> ChatFormatting.WHITE;
            case "WARN"  -> ChatFormatting.GOLD;
            case "ERROR" -> ChatFormatting.RED;
            default      -> ChatFormatting.GRAY;
        };
    }

    /** Badge background color (bold prefix "[LEVEL]"). */
    private static ChatFormatting levelBadgeColor(String level) {
        return switch (level) {
            case "DEBUG" -> ChatFormatting.GRAY;
            case "INFO"  -> ChatFormatting.GREEN;
            case "WARN"  -> ChatFormatting.YELLOW;
            case "ERROR" -> ChatFormatting.DARK_RED;
            default      -> ChatFormatting.WHITE;
        };
    }
}

