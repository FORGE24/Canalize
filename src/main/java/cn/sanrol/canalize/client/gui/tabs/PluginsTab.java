package cn.sanrol.canalize.client.gui.tabs;

import cn.sanrol.canalize.client.gui.AbstractPluginTab;
import net.minecraft.client.gui.GuiGraphics;
import net.minecraft.client.gui.components.Button;
import net.minecraft.client.gui.screens.Screen;
import net.minecraft.network.chat.Component;

import java.util.ArrayList;
import java.util.List;

/**
 * 插件列表标签页
 * 显示已加载、可用、禁用的插件列表
 */
public class PluginsTab extends AbstractPluginTab {
    private List<Button> pluginListButtons = new ArrayList<>();
    private int scrollOffset = 0;
    private static final int PLUGIN_ITEM_HEIGHT = 30;
    private static final int ITEMS_PER_PAGE = 10;

    public PluginsTab(Screen parent, int tabIndex, String tabName) {
        super(parent, tabIndex, tabName);
    }

    @Override
    public void init() {
        pluginListButtons.clear();
        // 将在render中动态创建按钮
    }

    @Override
    public void render(GuiGraphics guiGraphics, int mouseX, int mouseY, float partialTick) {
        // 绘制背景框
        guiGraphics.fill(x, y, x + width, y + height, 0xFF1E1E1E);
        guiGraphics.fill(x, y, x + width, y + 1, 0xFFCCCCCC);

        // 绘制标题
        drawString(guiGraphics, "已加载插件", x + 10, y + 10, 0xFFFFFF);

        // 绘制插件列表（示例）
        int yOffset = y + 35;
        drawString(guiGraphics, "插件列表（演示模式）", x + 10, yOffset, 0xFF00FF00);
        yOffset += 25;
        
        // 显示样本插件
        drawString(guiGraphics, "► TerrainOverhaul v1.0.0", x + 20, yOffset, 0xFFFFFFFF);
        drawString(guiGraphics, "   [状态: 已加载]  [优先级: 100]", x + 30, yOffset + 12, 0xFFCCCCCC);
        
        yOffset += 30;
        drawString(guiGraphics, "► 可用插件列表", x + 10, yOffset, 0xFFFFFF00);

        // 绘制相关按钮
        drawString(guiGraphics, "[加载插件] [删除] [重载全部]", x + 10, y + height - 30, 0xFFCCCCCC);
    }

    @Override
    public boolean mouseClicked(double mouseX, double mouseY, int button) {
        if (!isMouseInBounds(mouseX, mouseY)) {
            return false;
        }
        // Handle plugin list interactions
        return false;
    }

    @Override
    public boolean mouseReleased(double mouseX, double mouseY, int button) {
        return false;
    }

    @Override
    public boolean mouseScrolled(double mouseX, double mouseY, double scrollDelta) {
        if (!isMouseInBounds(mouseX, mouseY)) {
            return false;
        }
        scrollOffset = Math.max(0, scrollOffset - (int)scrollDelta * 3);
        return true;
    }

    @Override
    public boolean keyPressed(int keyCode, int scanCode, int modifiers) {
        return false;
    }

    @Override
    public boolean charTyped(char codePoint, int modifiers) {
        return false;
    }

    @Override
    public void onClose() {
        // Clean up
    }
}
