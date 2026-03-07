package cn.sanrol.canalize.client.gui;

import cn.sanrol.canalize.client.gui.tabs.*;
import net.minecraft.client.Minecraft;
import net.minecraft.client.gui.GuiGraphics;
import net.minecraft.client.gui.components.Button;
import net.minecraft.client.gui.screens.Screen;
import net.minecraft.network.chat.Component;

import java.util.ArrayList;
import java.util.List;

/**
 * Canalize 插件管理器主屏幕
 * 包含多个标签页用于管理插件的各个方面
 */
public class PluginManagerScreen extends Screen {
    private static final int TAB_HEIGHT = 25;
    private static final int TAB_BUTTON_WIDTH = 80;
    private static final int PADDING = 10;

    private final Screen lastScreen;
    private final List<AbstractPluginTab> tabs = new ArrayList<>();
    private int currentTabIndex = 0;

    // Tab按钮组件
    private List<Button> tabButtons = new ArrayList<>();

    public PluginManagerScreen(Screen lastScreen) {
        super(Component.literal("Canalize - 插件管理器"));
        this.lastScreen = lastScreen;
    }

    @Override
    protected void init() {
        super.init();
        this.tabButtons.clear();
        this.tabs.clear();

        // 初始化所有标签页
        tabs.add(new PluginsTab(this, 0, "插件列表"));
        tabs.add(new DetailTab(this, 1, "插件详情"));
        tabs.add(new ConfigTab(this, 2, "配置"));
        tabs.add(new StatsTab(this, 3, "统计"));
        tabs.add(new LogsTab(this, 4, "日志"));
        tabs.add(new AdvancedTab(this, 5, "高级"));

        // 创建标签按钮
        int tabX = PADDING;
        for (int i = 0; i < tabs.size(); i++) {
            AbstractPluginTab tab = tabs.get(i);
            final int tabIndex = i;
            Button btn = this.addRenderableWidget(Button.builder(
                Component.literal(tab.getTabTitle().getString()),
                button -> selectTab(tabIndex)
            )
            .pos(tabX, PADDING)
            .size(TAB_BUTTON_WIDTH, TAB_HEIGHT)
            .build());
            tabButtons.add(btn);
            tabX += TAB_BUTTON_WIDTH + 5;
        }

        // 返回按钮
        this.addRenderableWidget(Button.builder(
            Component.literal("返回"),
            button -> this.onClose()
        )
        .pos(this.width - 75, PADDING)
        .size(65, TAB_HEIGHT)
        .build());

        // 设置标签页内容区域
        int contentY = TAB_HEIGHT + PADDING * 2;
        int contentHeight = this.height - contentY - PADDING - 30;
        for (AbstractPluginTab tab : tabs) {
            tab.setPosition(PADDING, contentY, this.width - PADDING * 2, contentHeight);
            tab.init();
        }

        // 选中第一个标签页
        selectTab(0);
    }

    private void selectTab(int tabIndex) {
        if (tabIndex >= 0 && tabIndex < tabs.size()) {
            currentTabIndex = tabIndex;
            
            // 更新标签按钮的激活状态
            for (int i = 0; i < tabButtons.size(); i++) {
                Button btn = tabButtons.get(i);
                btn.active = (i != tabIndex);
            }

            // 初始化新选中的标签页
            tabs.get(tabIndex).init();
        }
    }

    @Override
    public void render(GuiGraphics guiGraphics, int mouseX, int mouseY, float partialTick) {
        this.renderTransparentBackground(guiGraphics);
        super.render(guiGraphics, mouseX, mouseY, partialTick);

        // 绘制标题
        guiGraphics.drawCenteredString(this.font, this.title.getString(), 
            this.width / 2, PADDING - 15, 0xFFFFFF);

        // 绘制当前标签页
        AbstractPluginTab currentTab = tabs.get(currentTabIndex);
        currentTab.render(guiGraphics, mouseX, mouseY, partialTick);
    }

    @Override
    public boolean mouseClicked(double mouseX, double mouseY, int button) {
        // 先处理标签按钮点击
        if (super.mouseClicked(mouseX, mouseY, button)) {
            return true;
        }

        // 处理当前标签页的鼠标点击
        AbstractPluginTab currentTab = tabs.get(currentTabIndex);
        return currentTab.mouseClicked(mouseX, mouseY, button);
    }

    @Override
    public boolean mouseReleased(double mouseX, double mouseY, int button) {
        super.mouseReleased(mouseX, mouseY, button);
        AbstractPluginTab currentTab = tabs.get(currentTabIndex);
        return currentTab.mouseReleased(mouseX, mouseY, button);
    }

    @Override
    public boolean mouseScrolled(double mouseX, double mouseY, double scrollX, double scrollY) {
        AbstractPluginTab currentTab = tabs.get(currentTabIndex);
        return currentTab.mouseScrolled(mouseX, mouseY, scrollY);
    }

    @Override
    public boolean keyPressed(int keyCode, int scanCode, int modifiers) {
        if (super.keyPressed(keyCode, scanCode, modifiers)) {
            return true;
        }
        AbstractPluginTab currentTab = tabs.get(currentTabIndex);
        return currentTab.keyPressed(keyCode, scanCode, modifiers);
    }

    @Override
    public boolean charTyped(char codePoint, int modifiers) {
        if (super.charTyped(codePoint, modifiers)) {
            return true;
        }
        AbstractPluginTab currentTab = tabs.get(currentTabIndex);
        return currentTab.charTyped(codePoint, modifiers);
    }

    @Override
    public void onClose() {
        AbstractPluginTab currentTab = tabs.get(currentTabIndex);
        currentTab.onClose();
        Minecraft.getInstance().setScreen(lastScreen);
    }

    // 辅助方法
    public int getCurrentTabIndex() {
        return currentTabIndex;
    }

    public AbstractPluginTab getCurrentTab() {
        return tabs.get(currentTabIndex);
    }

    public List<AbstractPluginTab> getAllTabs() {
        return new ArrayList<>(tabs);
    }
}
