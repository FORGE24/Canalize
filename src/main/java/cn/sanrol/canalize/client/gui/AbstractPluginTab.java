package cn.sanrol.canalize.client.gui;

import net.minecraft.client.gui.GuiGraphics;
import net.minecraft.client.gui.screens.Screen;
import net.minecraft.network.chat.Component;
import org.lwjgl.glfw.GLFW;

/**
 * 抽象基类，所有插件管理选项卡都继承此类
 */
public abstract class AbstractPluginTab {
    protected Screen parent;
    protected int x;
    protected int y;
    protected int width;
    protected int height;
    protected int tabIndex;
    protected String tabName;

    public AbstractPluginTab(Screen parent, int tabIndex, String tabName) {
        this.parent = parent;
        this.tabIndex = tabIndex;
        this.tabName = tabName;
    }

    public void setPosition(int x, int y, int width, int height) {
        this.x = x;
        this.y = y;
        this.width = width;
        this.height = height;
    }

    /** 渲染标签页内容 */
    public abstract void render(GuiGraphics guiGraphics, int mouseX, int mouseY, float partialTick);

    /** 处理标签页内鼠标点击 */
    public abstract boolean mouseClicked(double mouseX, double mouseY, int button);

    /** 处理标签页内鼠标释放 */
    public abstract boolean mouseReleased(double mouseX, double mouseY, int button);

    /** 处理鼠标滚动 */
    public abstract boolean mouseScrolled(double mouseX, double mouseY, double scrollDelta);

    /** 处理键盘输入 */
    public abstract boolean keyPressed(int keyCode, int scanCode, int modifiers);

    /** 字符输入处理 */
    public abstract boolean charTyped(char codePoint, int modifiers);

    /** 标签初始化 */
    public abstract void init();

    /** 标签关闭/清理 */
    public abstract void onClose();

    public Component getTabTitle() {
        return Component.literal(tabName);
    }

    protected boolean isMouseInBounds(double mouseX, double mouseY) {
        return mouseX >= x && mouseX < x + width && mouseY >= y && mouseY < y + height;
    }

    // 工具方法
    protected int getTextColor() {
        return 0xFFFFFF;
    }

    protected int getDisabledTextColor() {
        return 0x888888;
    }

    protected void drawCenteredString(GuiGraphics guiGraphics, String text, int x, int y, int color) {
        int width = parent.getMinecraft().font.width(text);
        guiGraphics.drawString(parent.getMinecraft().font, text, x - width / 2, y, color, false);
    }

    protected void drawString(GuiGraphics guiGraphics, String text, int x, int y, int color) {
        guiGraphics.drawString(parent.getMinecraft().font, text, x, y, color, false);
    }
}
