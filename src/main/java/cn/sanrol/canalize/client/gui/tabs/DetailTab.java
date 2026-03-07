package cn.sanrol.canalize.client.gui.tabs;

import cn.sanrol.canalize.client.gui.AbstractPluginTab;
import net.minecraft.client.gui.GuiGraphics;
import net.minecraft.client.gui.screens.Screen;

/**
 * 插件详情标签页
 * 显示选中插件的详细信息
 */
public class DetailTab extends AbstractPluginTab {
    public DetailTab(Screen parent, int tabIndex, String tabName) {
        super(parent, tabIndex, tabName);
    }

    @Override
    public void init() {
        // Initialize detail view
    }

    @Override
    public void render(GuiGraphics guiGraphics, int mouseX, int mouseY, float partialTick) {
        // Draw background
        guiGraphics.fill(x, y, x + width, y + height, 0xFF1E1E1E);
        guiGraphics.fill(x, y, x + width, y + 1, 0xFFCCCCCC);

        // Draw title
        drawString(guiGraphics, "插件详情", x + 10, y + 10, 0xFFFFFF);

        // Draw sample content
        int yOffset = y + 40;
        drawString(guiGraphics, "插件名称: TerrainOverhaul", x + 20, yOffset, 0xFFFFFFFF);
        yOffset += 20;
        drawString(guiGraphics, "版本: 1.0.0-alpha.1", x + 20, yOffset, 0xFFFFFFFF);
        yOffset += 20;
        drawString(guiGraphics, "作者: Sanrol", x + 20, yOffset, 0xFFFFFFFF);
        yOffset += 20;
        drawString(guiGraphics, "API版本: 1.0", x + 20, yOffset, 0xFFFFFFFF);
        yOffset += 20;
        drawString(guiGraphics, "状态: [激活]", x + 20, yOffset, 0xFF00FF00);
    }

    @Override
    public boolean mouseClicked(double mouseX, double mouseY, int button) {
        return isMouseInBounds(mouseX, mouseY);
    }

    @Override
    public boolean mouseReleased(double mouseX, double mouseY, int button) {
        return false;
    }

    @Override
    public boolean mouseScrolled(double mouseX, double mouseY, double scrollDelta) {
        return false;
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
    }
}
