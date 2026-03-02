package cn.sanrol.canalize;

import cn.sanrol.canalize.world.NativeQueryBridge;

import com.mojang.brigadier.CommandDispatcher;
import com.mojang.brigadier.arguments.IntegerArgumentType;
import com.mojang.logging.LogUtils;

import net.minecraft.ChatFormatting;
import net.minecraft.commands.CommandSourceStack;
import net.minecraft.commands.Commands;
import net.minecraft.network.chat.Component;
import net.minecraft.network.chat.MutableComponent;
import net.minecraft.world.phys.Vec3;

import net.neoforged.bus.api.SubscribeEvent;
import net.neoforged.fml.common.EventBusSubscriber;
import net.neoforged.neoforge.event.RegisterCommandsEvent;

import org.slf4j.Logger;

import java.util.Map;

@EventBusSubscriber(modid = Canalize.MODID)
public class CommandHandler {
    private static final Logger LOGGER = LogUtils.getLogger();

    // -----------------------------------------------------------------------
    // Helpers
    // -----------------------------------------------------------------------

    private static MutableComponent header(String text) {
        return Component.literal(text).withStyle(ChatFormatting.AQUA, ChatFormatting.BOLD);
    }

    private static MutableComponent label(String text) {
        return Component.literal(text).withStyle(ChatFormatting.YELLOW);
    }

    private static MutableComponent value(String text) {
        return Component.literal(text).withStyle(ChatFormatting.GREEN);
    }

    private static MutableComponent error(String text) {
        return Component.literal(text).withStyle(ChatFormatting.RED);
    }

    private static MutableComponent line(String lbl, String val) {
        return label("  " + lbl + ": ").append(value(val));
    }

    // -----------------------------------------------------------------------
    // Registration
    // -----------------------------------------------------------------------

    @SubscribeEvent
    public static void onRegisterCommands(RegisterCommandsEvent event) {
        CommandDispatcher<CommandSourceStack> dispatcher = event.getDispatcher();

        dispatcher.register(Commands.literal("canalize")

            // ------------------------------------------------------------------
            // /canalize on chunkloadlog
            // ------------------------------------------------------------------
            .then(Commands.literal("on")
                .then(Commands.literal("chunkloadlog")
                    .executes(ctx -> {
                        Canalize.CHUNK_LOAD_LOG = true;
                        ctx.getSource().sendSuccess(() ->
                            Component.literal("[Canalize] Chunk Load Log: ON"), true);
                        return 1;
                    })
                )
            )

            // ------------------------------------------------------------------
            // /canalize off chunkloadlog
            // ------------------------------------------------------------------
            .then(Commands.literal("off")
                .then(Commands.literal("chunkloadlog")
                    .executes(ctx -> {
                        Canalize.CHUNK_LOAD_LOG = false;
                        ctx.getSource().sendSuccess(() ->
                            Component.literal("[Canalize] Chunk Load Log: OFF"), true);
                        return 1;
                    })
                )
            )

            // ------------------------------------------------------------------
            // /canalize debug on|off
            //   Enables / disables real-time native C++ log output to chat.
            //   Messages are color-coded by level and drained every ~200 ms.
            // ------------------------------------------------------------------
            .then(Commands.literal("debug")
                .then(Commands.literal("on")
                    .executes(ctx -> {
                        Canalize.CHAT_DEBUG = true;
                        NativeQueryBridge.setLogEnabled(true);
                        ctx.getSource().sendSuccess(() ->
                            header("=== Canalize Debug Log: ON ==="), false);
                        ctx.getSource().sendSuccess(() ->
                            Component.literal("  Native C++ logs will now appear in this chat.")
                                .withStyle(ChatFormatting.GRAY), false);
                        ctx.getSource().sendSuccess(() ->
                            Component.literal("  Use /canalize debug off to stop.")
                                .withStyle(ChatFormatting.DARK_GRAY), false);
                        LOGGER.info("Canalize chat debug enabled by {}", ctx.getSource().getTextName());
                        return 1;
                    })
                )
                .then(Commands.literal("off")
                    .executes(ctx -> {
                        Canalize.CHAT_DEBUG = false;
                        NativeQueryBridge.setLogEnabled(false);
                        ctx.getSource().sendSuccess(() ->
                            Component.literal("[Canalize] Debug Log: OFF")
                                .withStyle(ChatFormatting.GRAY), false);
                        LOGGER.info("Canalize chat debug disabled by {}", ctx.getSource().getTextName());
                        return 1;
                    })
                )
                // /canalize debug status — shows current debug state without toggling
                .then(Commands.literal("status")
                    .executes(ctx -> {
                        boolean on = Canalize.CHAT_DEBUG;
                        ctx.getSource().sendSuccess(() ->
                            line("Chat Debug",
                                on ? "\u25CF ON (native logs \u2192 chat)" : "\u25CB OFF"), false);
                        ctx.getSource().sendSuccess(() ->
                            line("Native lib",
                                Canalize.NATIVE_LOADED ? "loaded" : "NOT LOADED"), false);
                        return 1;
                    })
                )
            )

            // ------------------------------------------------------------------
            // /canalize status
            //   Shows native library status, version, chunk gen stats.
            // ------------------------------------------------------------------
            .then(Commands.literal("status")
                .executes(ctx -> {
                    ctx.getSource().sendSuccess(() -> header("=== Canalize Native Status ==="), false);

                    // Native loaded?
                    ctx.getSource().sendSuccess(() ->
                        line("Native Loaded", Canalize.NATIVE_LOADED ? "YES" : "NO"), false);

                    if (!Canalize.NATIVE_LOADED) {
                        ctx.getSource().sendSuccess(() ->
                            error("  Native library failed to load. World generation is unavailable."), false);
                        return 1;
                    }

                    Map<String, String> s = NativeQueryBridge.getStatusMap();
                    if (s.containsKey("error")) {
                        ctx.getSource().sendSuccess(() ->
                            error("  Error: " + s.get("error")), false);
                        return 1;
                    }

                    String version  = s.getOrDefault("version",   "?");
                    String chunks   = s.getOrDefault("chunks",    "0");
                    long   avgNs    = parseLong(s.get("avg_ns"),   0L);
                    long   minNs    = parseLong(s.get("min_ns"),   0L);
                    long   maxNs    = parseLong(s.get("max_ns"),   0L);
                    long   totalMs  = parseLong(s.get("total_ms"), 0L);
                    String lastCx   = s.getOrDefault("last_cx",   "?");
                    String lastCz   = s.getOrDefault("last_cz",   "?");

                    ctx.getSource().sendSuccess(() -> line("Library Version",  version),  false);
                    ctx.getSource().sendSuccess(() -> line("Chunks Generated", chunks),   false);
                    ctx.getSource().sendSuccess(() -> line("Avg Gen Time",
                        NativeQueryBridge.formatNs(avgNs)), false);
                    ctx.getSource().sendSuccess(() -> line("Min Gen Time",
                        NativeQueryBridge.formatNs(minNs)), false);
                    ctx.getSource().sendSuccess(() -> line("Max Gen Time",
                        NativeQueryBridge.formatNs(maxNs)), false);
                    ctx.getSource().sendSuccess(() -> line("Total Gen Time",
                        totalMs + " ms"), false);
                    ctx.getSource().sendSuccess(() -> line("Last Chunk",
                        "[" + lastCx + ", " + lastCz + "]"), false);
                    ctx.getSource().sendSuccess(() -> line("Chunk Load Log",
                        Canalize.CHUNK_LOAD_LOG ? "ON" : "OFF"), false);
                    return 1;
                })
            )

            // ------------------------------------------------------------------
            // /canalize terrain          (uses player position)
            // /canalize terrain <x> <z>  (uses given block coordinates)
            //   Queries native terrain info: height, biome, Y-range.
            // ------------------------------------------------------------------
            .then(Commands.literal("terrain")
                // No args — use player/entity position
                .executes(ctx -> {
                    Vec3 pos = ctx.getSource().getPosition();
                    int bx = (int) Math.floor(pos.x);
                    int bz = (int) Math.floor(pos.z);
                    return sendTerrainInfo(ctx.getSource(), bx, bz);
                })
                // With explicit x z
                .then(Commands.argument("x", IntegerArgumentType.integer())
                    .then(Commands.argument("z", IntegerArgumentType.integer())
                        .executes(ctx -> {
                            int bx = IntegerArgumentType.getInteger(ctx, "x");
                            int bz = IntegerArgumentType.getInteger(ctx, "z");
                            return sendTerrainInfo(ctx.getSource(), bx, bz);
                        })
                    )
                )
            )

            // ------------------------------------------------------------------
            // /canalize reset
            //   Resets native chunk generation statistics counters.
            // ------------------------------------------------------------------
            .then(Commands.literal("reset")
                .executes(ctx -> {
                    NativeQueryBridge.resetStats();
                    ctx.getSource().sendSuccess(() ->
                        Component.literal("[Canalize] Native stats reset.")
                            .withStyle(ChatFormatting.GREEN), true);
                    LOGGER.info("Canalize native stats reset by {}", ctx.getSource().getTextName());
                    return 1;
                })
            )
        );
    }

    // -----------------------------------------------------------------------
    // Terrain info sender (shared by both /canalize terrain variants)
    // -----------------------------------------------------------------------
    private static int sendTerrainInfo(CommandSourceStack src, int bx, int bz) {
        src.sendSuccess(() -> header(
            String.format("=== Terrain at (%d, %d) ===", bx, bz)), false);

        if (!Canalize.NATIVE_LOADED) {
            src.sendSuccess(() -> error("  Native library not loaded."), false);
            return 1;
        }

        Map<String, String> t = NativeQueryBridge.getTerrainInfoMap(bx, bz);
        if (t.containsKey("error")) {
            src.sendSuccess(() -> error("  Error: " + t.get("error")), false);
            return 1;
        }

        String height   = t.getOrDefault("height",    "?");
        String biome    = t.getOrDefault("biome",     "?");
        String biomeId  = t.getOrDefault("biomeId",   "?");
        String seaLvl   = t.getOrDefault("seaLevel",  "63");
        String minY     = t.getOrDefault("minY",      "-64");
        String maxY     = t.getOrDefault("maxY",      "?");

        src.sendSuccess(() -> line("Surface Height", height),               false);
        src.sendSuccess(() -> line("Biome",          biome + " (id=" + biomeId + ")"), false);
        src.sendSuccess(() -> line("Sea Level",      seaLvl),               false);
        src.sendSuccess(() -> line("Y Range",        minY + " ~ " + maxY),  false);
        return 1;
    }

    // -----------------------------------------------------------------------
    // Utility
    // -----------------------------------------------------------------------
    private static long parseLong(String s, long def) {
        if (s == null) return def;
        try { return Long.parseLong(s); }
        catch (NumberFormatException e) { return def; }
    }
}

