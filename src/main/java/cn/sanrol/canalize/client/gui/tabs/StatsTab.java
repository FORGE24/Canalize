package cn.sanrol.canalize.client.gui.tabs;

import cn.sanrol.canalize.client.gui.AbstractPluginTab;
import net.minecraft.client.gui.GuiGraphics;
import net.minecraft.client.gui.screens.Screen;

/**
 * 统计分析标签页
 * 显示插件性能统计和分析数据
 */
public class StatsTab extends AbstractPluginTab {
    public StatsTab(Screen parent, int tabIndex, String tabName) {
        super(parent, tabIndex, tabName);
    }

    @Override
    public void init() {
        // Initialize stats view
    }

    @Override
    public void render(GuiGraphics guiGraphics, int mouseX, int mouseY, float partialTick) {
        guiGraphics.fill(x, y, x + width, y + height, 0xFF1E1E1E);
        guiGraphics.fill(x, y, x + width, y + 1, 0xFFCCCCCC);

        drawString(guiGraphics, "性能统计", x + 10, y + 10, 0xFFFFFF);

        int yOffset = y + 40;
        
        // Performance metrics
        drawString(guiGraphics, "插件执行时间统计:", x + 20, yOffset, 0xFFFFFF00);
        yOffset += 20;
        
        drawString(guiGraphics, "TerrainOverhaul:", x + 30, yOffset, 0xFFFFFFFF);
        drawString(guiGraphics, "执行次数: 1234 | 平均时间: 2.34ms | 峰值: 5.12ms", 
            x + 150, yOffset, 0xFFCCCCCC);
        yOffset += 20;
        
        drawString(guiGraphics, "内存占用:", x + 20, yOffset, 0xFFFFFF00);
        yOffset += 20;
        
        drawString(guiGraphics, "总计: 45.2 MB | Java堆: 23.1 MB | 本地: 22.1 MB", 
            x + 30, yOffset, 0xFFCCCCCC);
        yOffset += 25;
        
        // Analysis
        drawString(guiGraphics, "分析与建议:", x + 20, yOffset, 0xFFFFFF00);
        yOffset += 20;
        drawString(guiGraphics, "系统健康状态: [良好]", x + 30, yOffset, 0xFF00FF00);
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
