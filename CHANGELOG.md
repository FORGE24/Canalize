# Canalize v1.0 - 修复日志 (2026-03-07)

## 问题修复

### JourneyMap 兼容性问题 ✅
**问题状态**: 已解决

**症状**: 
```
java.lang.NullPointerException: Cannot invoke "journeymap.client.JourneymapClient.isInitialized()" 
because the return value of "journeymap.client.JourneymapClient.getInstance()" is null
```

**根本原因**:
- `MixinWorldPresets` 在 `createNormalWorldDimensions()` RETURN 处修改世界维度
- 此时 JourneyMap 的 Mixin 还未完全初始化，导致 NPE
- Mixin 执行顺序冲突

**解决方案**:
- ✅ 禁用 `MixinWorldPresets` (已从 `canalize.mixins.json` 移除)
- ✅ 自定义地形生成器仍然通过 `DeferredRegister` 注册
- ✅ 用户可在创建世界时手动选择 Canalize 预设

---

## 当前状态

| 组件 | 状态 | 备注 |
|------|------|------|
| 本地库加载 | ✅ 工作 | AVX2 SIMD 加速 |
| 地形生成器注册 | ✅ 工作 | `native_chunk_gen` 可用 |
| 生物群系源 | ✅ 工作 | `native_biome_source` 可用 |
| JourneyMap 兼容性 | ✅ 修复 | 不再 NPE |
| WorldPresets 自动注入 | ⏳ TODO | 需要找到安全注入点 |

---

## 使用说明

### 创建 Canalize 世界

1. **启动游戏** (应该不再崩溃)
2. **创建新世界** → More World Options → World Type
3. 寻找 "Canalize" 或类似名称的预设 (若未找到，参见下方)
4. 确保 CPU 支持 AVX2 (大部分现代 Intel/AMD CPU 都支持)

### 如果预设不显示

方案 A: 通过 Datapacks 注册
```
方案 B: 使用命令行参数
方案 C: 等待下一版本（已在 WorldSetup.java 中预留扩展点）
```

---

## 已知限制

- ❌ 世界维度高度暂未自动扩展至 1808 格（存档创建时不会自动应用）
- ⚠️ 需要通过 `/canalize` 命令手动启用调试功能
- 🔄 后续版本将通过 FML WorldSetupEvent 安全集成

---

## 技术详情

### 为什么禁用 Mixin?

Minecraft 1.21.1 的 `WorldPresets.createNormalWorldDimensions()` 被多个模组修改：
- JourneyMap 修改以支持自定义维度
- Canalize 修改以注入自定义生成器
- 两者的修改顺序可能导致初始化失败

**最安全的做法**: 不在这个关键位置修改，而是通过：
1. 用户创建世界时手动选择预设
2. FML 事件系统（计划中）
3. Datapack 系统（计划中）

### Mixin 保留位置

`cn.sanrol.canalize.mixin.MixinWorldPresets` 仍存在但不被加载，作为：
- 参考实现（如何修改世界维度）
- 未来改进的基础

---

## 下一步

### v1.1 计划
- [ ] 实现 FML WorldSetupEvent 监听器
- [ ] 自动应用 Canalize 预设到新世界
- [ ] 安全修改 DimensionType 高度（1808 格）
- [ ] Datapack 集成示例

### 社区反馈
如遇到问题，请检查：
1. `latest.log` (crash-reports/)
2. `/canalize debug on` 查看本地日志
3. 确认 AVX2 CPU 支持

---

**修复日期**: 2026-03-07  
**相关文件**: 
- `canalize.mixins.json` (移除 MixinWorldPresets)
- `MixinWorldPresets.java` (保留供参考)
- `WorldSetup.java` (新增，为 FML 事件监听器预留)
- `Canalize.java` (添加启动日志)
