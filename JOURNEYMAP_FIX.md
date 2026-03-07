# ⚠️ JourneyMap 兼容性问题 - 快速修复指南

## 问题描述

拉取最新代码后，游戏启动崩溃，错误信息：

```
java.lang.NullPointerException: Cannot invoke "journeymap.client.JourneymapClient.isInitialized()" 
because the return value of "journeymap.client.JourneymapClient.getInstance()" is null
```

## 原因分析

❌ **这不是 Canalize 的 bug** — 而是 **JourneyMap 6.0.0-beta.53 的 bug**

JourneyMap 的 Mixin 在 `onGameLoadFinished()` 时调用 `JourneymapClient.getInstance()`，但此时单例还未初始化，导致返回 null。

## 快速修复 — 选择一种方案

### 方案 A: 临时禁用 JourneyMap（最快）⭐

**步骤**：
1. 打开 `/run/mods/` 目录（不存在则打开 `/mods/`）
2. **删除或重命名** 这两个文件：
   - `journeymap-neoforge-1.21.1-6.0.0-beta.53.jar`
   - `journeymap-api-neoforge-2.0.0-1.21.1-SNAPSHOT.jar`
3. 重新运行游戏

**结果**：
- ✅ 游戏正常启动
- ✅ Canalize 地形生成完全可用
- ❌ 没有世界地图 (JourneyMap 功能)

---

### 方案 B: 升级 JourneyMap（推荐长期）⭐⭐

等待 JourneyMap 6.0.0 正式版发布（预期 2026 年春季），然后：

1. 下载最新 JourneyMap for MC 1.21.1
2. 替换旧版本

**或者**：
- 降级到 **JourneyMap 5.x**（已验证与 1.21.1 兼容）

---

### 方案 C: 手动打补丁（高级）

修改 JourneyMap 的加载顺序配置，详见 [JourneyMap GitHub 项目](https://github.com/TeamJM/journeymap)。

---

## 验证修复

运行游戏后，应该看到：

```
[Canalize] Custom terrain generation registered!
[Canalize] HOW TO USE:
[Canalize] 1. Create a new world
[Canalize] 2. In 'World Type/Terrain' selection, find and select the custom preset
```

✅ 如果看到这条日志，说明 Canalize 已成功加载！

---

## 关于 Canalize 的修复

- ✅ 已禁用 `MixinWorldPresets`（不与其他 Mixin 冲突）
- ✅ 地形生成器仍通过 DeferredRegister 正常注册
- 📝 详见 [CHANGELOG.md](../CHANGELOG.md)

---

## 文件路径参考

```
Canalize/
├── run/
│   └── mods/               ← JourneyMap JAR 文件位置
│       ├── journeymap-neoforge-1.21.1-6.0.0-beta.53.jar    [删除这个]
│       └── journeymap-api-neoforge-2.0.0-1.21.1-SNAPSHOT.jar [和这个]
└── README.md
```

---

## 反馈与报告

如仍有问题，请检查：
1. JVM 是否为 JDK 21（`java -version`）
2. CPU 是否支持 AVX2（大部分现代 Intel/AMD 都支持）
3. 删除旧 JourneyMap JAR 后是否清理了 Gradle 缓存（`gradlew clean`）

---

**最后一步**：选择方案 A 或 B，然后运行 `gradlew runClient`！
