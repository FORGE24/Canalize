package cn.sanrol.canalize;

import com.mojang.brigadier.CommandDispatcher;
import com.mojang.logging.LogUtils;
import net.minecraft.commands.CommandSourceStack;
import net.minecraft.commands.Commands;
import net.minecraft.network.chat.Component;
import net.neoforged.bus.api.SubscribeEvent;
import net.neoforged.fml.common.EventBusSubscriber;
import net.neoforged.neoforge.event.RegisterCommandsEvent;
import org.slf4j.Logger;

@EventBusSubscriber(modid = Canalize.MODID)
public class CommandHandler {
    private static final Logger LOGGER = LogUtils.getLogger();

    @SubscribeEvent
    public static void onRegisterCommands(RegisterCommandsEvent event) {
        CommandDispatcher<CommandSourceStack> dispatcher = event.getDispatcher();

        dispatcher.register(Commands.literal("canalize")
            .then(Commands.literal("on")
                .then(Commands.literal("chunkloadlog")
                    .executes(context -> {
                        Canalize.CHUNK_LOAD_LOG = true;
                        Component msg = Component.literal("[Canalize] Chunk Load Log: ON (Level 12 Detailed)");
                        context.getSource().sendSuccess(() -> msg, true);
                        LOGGER.info("Chunk Load Log enabled by {}", context.getSource().getTextName());
                        return 1;
                    })
                )
            )
            .then(Commands.literal("off")
                .then(Commands.literal("chunkloadlog")
                    .executes(context -> {
                        Canalize.CHUNK_LOAD_LOG = false;
                        Component msg = Component.literal("[Canalize] Chunk Load Log: OFF");
                        context.getSource().sendSuccess(() -> msg, true);
                        LOGGER.info("Chunk Load Log disabled by {}", context.getSource().getTextName());
                        return 1;
                    })
                )
            )
        );
    }
}
