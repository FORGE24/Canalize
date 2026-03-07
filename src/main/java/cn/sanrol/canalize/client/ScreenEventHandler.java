package cn.sanrol.canalize.client;

import cn.sanrol.canalize.Canalize;
import cn.sanrol.canalize.client.gui.PluginManagerScreen;
import net.minecraft.client.gui.screens.Screen;
import net.neoforged.api.distmarker.Dist;
import net.neoforged.bus.api.SubscribeEvent;
import net.neoforged.fml.common.EventBusSubscriber;
import net.neoforged.neoforge.client.event.ScreenEvent;
import net.minecraft.client.gui.components.Button;
import net.minecraft.network.chat.Component;
import net.minecraft.client.Minecraft;

/**
 * 屏幕事件处理器
 * 在选项屏幕中添加"Canalize插件管理"按钮
 */
@EventBusSubscriber(modid = Canalize.MODID, value = Dist.CLIENT)
public class ScreenEventHandler {

    @SubscribeEvent
    public static void onScreenOpen(ScreenEvent.Init.Post event) {
        Screen screen = event.getScreen();
        
        // 在任何屏幕的底部左方添加Canalize按钮
        // 检查屏幕标题是否包含"选项"或其他标志
        String screenTitle = screen.getTitle().getString();
        if (screenTitle.contains("选项") || screenTitle.contains("Options") || 
            screenTitle.contains("Settings")) {
            
            Screen currentScreen = event.getScreen();
            Button canalizeButton = Button.builder(
                Component.literal("Canalize 插件管理器"),
                button -> {
                    Canalize.LOGGER.info("[Canalize] 打开插件管理器屏幕");
                    // 获取minecraft实例并打开插件管理器屏幕
                    Minecraft minecraft = Minecraft.getInstance();
                    if (minecraft != null) {
                        minecraft.setScreen(
                            new PluginManagerScreen(currentScreen)
                        );
                    }
                }
            )
            .pos(10, event.getScreen().height - 25)
            .size(200, 20)
            .build();
            
            event.addListener(canalizeButton);
        }
    }
}
