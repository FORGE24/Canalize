package cn.sanrol.canalize.client.gui.tabs;

import cn.sanrol.canalize.client.gui.AbstractPluginTab;
import cn.sanrol.canalize.world.NativeQueryBridge;
import net.minecraft.client.gui.GuiGraphics;
import net.minecraft.client.gui.screens.Screen;

import java.util.ArrayList;
import java.util.List;

/**
 * 日志查看标签页
 * 实时日志流和日志查询功能
 */
public class LogsTab extends AbstractPluginTab {
    private List<String> logBuffer = new ArrayList<>();
    private String filterKeyword = "";
    private String filterLevel = "ALL";
    private int scrollOffset = 0;
    private static final int LINES_PER_PAGE = 15;

    public LogsTab(Screen parent, int tabIndex, String tabName) {
        super(parent, tabIndex, tabName);
    }

    @Override
    public void init() {
        logBuffer.clear();
        // Load initial logs
        String[] logs = NativeQueryBridge.drainLog();
        for (String log : logs) {
            logBuffer.add(log);
        }
    }

    @Override
    public void render(GuiGraphics guiGraphics, int mouseX, int mouseY, float partialTick) {
        guiGraphics.fill(x, y, x + width, y + height, 0xFF1E1E1E);
        guiGraphics.fill(x, y, x + width, y + 1, 0xFFCCCCCC);

        drawString(guiGraphics, "实时日志", x + 10, y + 10, 0xFFFFFF);

        // Draw filter bar
        drawString(guiGraphics, "过滤: [关键词] [级别: " + filterLevel + "]", x + 20, y + 35, 0xFFCCCCCC);

        // Draw log lines
        int yOffset = y + 60;
        int lineCount = 0;
        for (String logLine : logBuffer) {
            if (lineCount >= scrollOffset && lineCount < scrollOffset + LINES_PER_PAGE) {
                if (logLine.isEmpty()) continue;
                
                // Color code based on log level
                int color = 0xFFFFFFFF;
                if (logLine.contains("ERROR")) color = 0xFFFF0000;
                else if (logLine.contains("WARN")) color = 0xFFFFFF00;
                else if (logLine.contains("INFO")) color = 0xFF00FF00;
                else if (logLine.contains("DEBUG")) color = 0xFF0088FF;
                
                String displayLine = logLine;
                if (displayLine.length() > width / 7) {
                    displayLine = displayLine.substring(0, Math.min(width / 7, displayLine.length())) + "...";
                }
                
                drawString(guiGraphics, displayLine, x + 20, yOffset, color);
                yOffset += 12;
            }
            lineCount++;
        }

        // Draw footer
        drawString(guiGraphics, "总日志行数: " + logBuffer.size() + " | [导出] [清空] [搜索]", 
            x + 20, y + height - 25, 0xFFCCCCCC);
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
    }
}
