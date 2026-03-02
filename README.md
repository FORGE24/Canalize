# Canalize

**[中文](#canalize-zh) | [English](#canalize-en)**

---

<a name="canalize-zh"></a>

> 一个通过 JNI 调用高性能 C++ 原生库，完全重写 Minecraft 主世界地形生成的 NeoForge 模组。

[![Minecraft](https://img.shields.io/badge/Minecraft-1.21.1-green)](https://www.minecraft.net/)
[![NeoForge](https://img.shields.io/badge/NeoForge-21.1.219-orange)](https://neoforged.net/)
[![Java](https://img.shields.io/badge/Java-21-blue)](https://www.oracle.com/java/)
[![License](https://img.shields.io/badge/License-MulanPubL--2.0-blue)](LICENSE)

---

> [!CAUTION]
> **⚠ WARNING 警告 — 早期开发阶段，请勿在重要存档中使用**
>
> 本模组目前处于**早期开发阶段**，存在以下严重风险：
>
> - **不可逆绑定**：一旦在某个世界存档中启用本模组，该存档将永久依赖 Canalize 的地形生成器。**移除本模组后，受影响的区块将无法正确加载，存档可能损坏。**
> - 地形生成逻辑、世界高度参数及数据格式可能在后续版本中发生破坏性变更，届时旧存档将不再兼容。
> - 原生库（`.dll` / `.so`）目前仅支持具备 AVX2 指令集的 CPU，不满足条件的设备将直接崩溃。
>
> **请始终在独立测试存档中使用，切勿用于生产环境或多人服务器。**

---

> [!CAUTION]
> **⚠ EARLY DEVELOPMENT — Do NOT use on important worlds**
>
> This mod is currently in an **early development stage** and carries the following serious risks:
>
> - **Irreversible world binding**: Once this mod is enabled in a world save, that save will permanently depend on Canalize's terrain generator. **Removing the mod will cause affected chunks to fail to load, potentially corrupting the save.**
> - Terrain generation logic, world height parameters, and data formats may undergo breaking changes in future versions, making old saves incompatible.
> - The native library (`.dll` / `.so`) currently requires a CPU with AVX2 support. Devices that do not meet this requirement will crash immediately.
>
> **Always use in a dedicated test world. Do not use in production environments or multiplayer servers.**

---

## 简介

**Canalize** 打破了原版 Minecraft 的地形生成框架，将核心高度图计算下沉至 C++ 层，通过 JNI 桥接回 Java 游戏逻辑。其主要目标是：

- 将主世界高度上限从原版的 384 格**扩展至 1808 格**（Min Y: −64，Max Y: 1744）
- 引入现代噪声算法（Ridged FBM、Worley 噪声、Domain Warping）生成更真实、更宏大的地形
- 利用 **AVX2 SIMD** 指令集加速噪声计算，保证高度扩展后仍具备可接受的生成性能

---

## 特性

| 特性 | 说明 |
|---|---|
| 极端高度 | 主世界高度扩展至 1808 格，支持超过 1700 格高的巨型山脉 |
| 原生加速 | C++17 实现噪声内核，AVX2 SIMD 并行计算 |
| 水力侵蚀 | 基于散度梯度侵蚀算法，雕刻山谷与河道 |
| 岩石细节 | 山顶混合 Worley 细胞噪声，呈现真实岩石质感 |
| 平滑过渡 | `smoothstep` 生物群系插值，消除海洋/平原/山脉之间的硬边界 |
| 线程安全 | 无状态哈希函数替代 `srand()`/`rand()`，杜绝竞态条件 |

---

## 噪声算法栈

```
Simplex 噪声 (OS2S)
    └── FBM（分形布朗运动）
         ├── Ridged FBM（脊状分形）—— 山脉骨架
         ├── Worley 噪声（细胞噪声）—— 岩石质感
         └── Domain Warping（领域扭曲）—— 水力侵蚀模拟
```

详细算法说明请参见 [TERRAIN_ALGORITHM.md](TERRAIN_ALGORITHM.md)。

---

## 技术栈

- **游戏版本**: Minecraft 1.21.1
- **模组加载器**: NeoForge 21.1.219
- **Java 版本**: Java 21
- **C++ 标准**: C++17
- **构建工具**: Gradle + CMake
- **原生接口**: JNI (Java Native Interface)
- **SIMD 优化**: AVX2

---

## 构建

### 前置要求

- JDK 21
- CMake 3.10+
- 支持 AVX2 的 C++ 编译器（MSVC / GCC / Clang）

### 构建原生库

```bat
cd src/cpp
build_native.bat
```

编译产物（`canalize_native.dll` / `libcanalize_native.so`）将输出至 `build/libs/`。

### 构建模组

```bash
./gradlew build
```

JAR 产物位于 `build/libs/canalize-<version>.jar`。

### 运行开发客户端

```bash
./gradlew runClient
```

---

## 项目结构

```
src/
├── cpp/
│   ├── jni.cpp              # JNI 入口（Java ↔ C++ 桥接）
│   ├── CMakeLists.txt       # CMake 构建配置
│   ├── build_native.bat     # Windows 一键构建脚本
│   ├── include/
│   │   ├── MathUtil.h       # 噪声原语（Simplex / FBM / Worley）
│   │   └── TerrainGen.h     # 地形生成器接口
│   └── src/
│       ├── TerrainGen.cpp   # 地形生成核心（JJTH 系统 C++ 移植）
│       ├── Carver.cpp       # 洞穴/峡谷雕刻
│       ├── Decorator.cpp    # 地物装饰
│       └── WorldLoader.cpp  # 世界加载辅助
└── main/
    └── java/
        └── cn/sanrol/canalize/
            ├── world/       # 地形生成器（NativeChunkGenerator / NativeBiomeSource）
            └── mixin/       # Mixin 注入（MixinWorldPresets 等）

EXAMPLE/
└── JJTH/                    # 独立项目 JJTH 的 worldgen datapack（仅作参考）
    └── data/minecraft/worldgen/   # density_function / noise_settings / biome 配置
```

---

## 参考文献

1. **Musgrave, F. K., Kolb, C. E., & Mace, R. S. (1989).** *The synthesis and rendering of eroded fractal terrains*. ACM SIGGRAPH, 23(3), 41–50. — Ridged FBM 与水力侵蚀思想来源。
2. **Inigo Quilez.** *Domain Warping*. [iquilezles.org](https://iquilezles.org/articles/warp/) — Domain Warping 实现参考。
3. **Ken Perlin.** *Simplex Noise*. — Simplex 噪声基础实现参考。
4. **Chang, P., Tang, J., Gross, M., & Azevedo, V. C. (2025).** *How I Warped Your Noise: a Temporally-Correlated Noise Prior for Diffusion Models*. ICLR 2024 (Oral). [arXiv:2504.03072](https://arxiv.org/abs/2504.03072) — 提出 ∫-noise（积分噪声）表示，将噪声定义为连续噪声场的像素级积分并通过光流搬运，实现帧间时序相关性；与本项目 Domain Warping 在"搬运连续噪声场坐标"上具有数学同源性。
5. **Deng, Y., Lin, W., Li, L., Smirnov, D., Burgert, R., Yu, N., Dedun, V., & Taghavi, M. H. (2024).** *Infinite-Resolution Integral Noise Warping for Diffusion Models*. [arXiv:2411.01212](https://arxiv.org/abs/2411.01212) — 对上文 ∫-noise 算法的改进，利用布朗桥增量在无限分辨率精度下将计算量降低数个量级，并扩展至三维空间。
6. **Daras, G., Nie, W., Kreis, K., Dimakis, A., Mardani, M., Kovachki, N. B., & Vahdat, A. (2024).** *Warped Diffusion: Solving Video Inverse Problems with Image Diffusion Models*. NeurIPS 2024. [arXiv:2410.16152](https://arxiv.org/abs/2410.16152) — 将帧建模为连续 2D 函数、视频为帧间连续空间变换，在函数空间训练图像扩散模型并施加等变性约束，用于视频修复与超分辨率。
7. **Na, B., Kim, Y., Park, M., Shin, D., Kang, W., & Moon, I. (2024).** *Diffusion Rejection Sampling*. ICML 2024. [arXiv:2405.17880](https://arxiv.org/abs/2405.17880) — 在扩散模型每个去噪时间步引入拒绝采样机制，对中间样本进行质量筛选与精炼，理论上能更紧地逼近真实分布。
8. **Liu, C., & Vahdat, A. (2025).** *On Equivariance and Fast Sampling in Video Diffusion Models Trained with Warped Noise*. [arXiv:2504.09789](https://arxiv.org/abs/2504.09789) — 从理论上证明扭曲噪声训练隐式赋予模型对输入噪声空间变换的等变性（EquiVDM），使噪声运动自然对齐视频运动，同时大幅减少所需采样步数。

---

## 许可证

本项目基于 [木兰公共许可证 第2版（MulanPubL-2.0）](LICENSE) 开源。

简而言之：你可以自由使用、修改和分发本项目，但任何衍生作品**必须**以同样的 MulanPubL-2.0 许可证开源并提供对应源代码。许可证以中英文双语表述，具有同等法律效力，中文版优先。

> Copyright (c) 2026 NullSenpai  
> Canalize is licensed under Mulan PubL v2.  
> You can use this software according to the terms and conditions of the Mulan PubL v2.  
> You may obtain a copy of Mulan PubL v2 at: http://license.coscl.org.cn/MulanPubL-2.0

---
---

<a name="canalize-en"></a>

# Canalize (English)

> A NeoForge mod that completely rewrites Minecraft's Overworld terrain generation by calling a high-performance C++ native library via JNI.

[![Minecraft](https://img.shields.io/badge/Minecraft-1.21.1-green)](https://www.minecraft.net/)
[![NeoForge](https://img.shields.io/badge/NeoForge-21.1.219-orange)](https://neoforged.net/)
[![Java](https://img.shields.io/badge/Java-21-blue)](https://www.oracle.com/java/)
[![License](https://img.shields.io/badge/License-MulanPubL--2.0-blue)](LICENSE)

---

> [!CAUTION]
> **⚠ EARLY DEVELOPMENT — Do NOT use on important worlds**
>
> This mod is currently in an **early development stage** and carries the following serious risks:
>
> - **Irreversible world binding**: Once this mod is enabled in a world save, that save will permanently depend on Canalize's terrain generator. **Removing the mod will cause affected chunks to fail to load, potentially corrupting the save.**
> - Terrain generation logic, world height parameters, and data formats may undergo breaking changes in future versions, making old saves incompatible.
> - The native library (`.dll` / `.so`) currently requires a CPU with AVX2 support. Devices that do not meet this requirement will crash immediately.
>
> **Always use in a dedicated test world. Do not use in production environments or multiplayer servers.**

---

## Introduction

**Canalize** breaks out of Minecraft's vanilla terrain generation framework, moving the core heightmap computation down to a C++ native layer and bridging it back to Java game logic via JNI. Its primary goals are:

- Extend the Overworld height limit from the vanilla 384 blocks **to 1808 blocks** (Min Y: −64, Max Y: 1744)
- Introduce modern noise algorithms (Ridged FBM, Worley noise, Domain Warping) for more realistic and dramatic terrain
- Leverage **AVX2 SIMD** instruction sets to accelerate noise computation, maintaining acceptable generation performance despite the expanded height

---

## Features

| Feature | Description |
|---|---|
| Extreme Height | Overworld height extended to 1808 blocks, supporting mountain ranges over 1700 blocks tall |
| Native Acceleration | C++17 noise kernel with AVX2 SIMD parallel computation |
| Hydraulic Erosion | Divergence-gradient erosion algorithm carves valleys and riverbeds |
| Rocky Detail | Worley cellular noise blended at mountain peaks for realistic rock texture |
| Smooth Transitions | `smoothstep` biome interpolation eliminates hard boundaries between ocean, plains, and mountains |
| Thread Safety | Stateless hash functions replace `srand()`/`rand()`, eliminating race conditions |

---

## Noise Algorithm Stack

```
Simplex Noise (OS2S)
    └── FBM (Fractional Brownian Motion)
         ├── Ridged FBM      —— mountain skeleton
         ├── Worley Noise    —— rock texture
         └── Domain Warping  —— hydraulic erosion simulation
```

See [TERRAIN_ALGORITHM.md](TERRAIN_ALGORITHM.md) for detailed algorithm documentation.

---

## Tech Stack

- **Game Version**: Minecraft 1.21.1
- **Mod Loader**: NeoForge 21.1.219
- **Java Version**: Java 21
- **C++ Standard**: C++17
- **Build Tools**: Gradle + CMake
- **Native Interface**: JNI (Java Native Interface)
- **SIMD Optimization**: AVX2

---

## Building

### Prerequisites

- JDK 21
- CMake 3.10+
- C++ compiler with AVX2 support (MSVC / GCC / Clang)

### Build the Native Library

```bat
cd src/cpp
build_native.bat
```

Output (`canalize_native.dll` / `libcanalize_native.so`) will be placed in `build/libs/`.

### Build the Mod

```bash
./gradlew build
```

JAR artifact: `build/libs/canalize-<version>.jar`

### Run the Development Client

```bash
./gradlew runClient
```

---

## Project Structure

```
src/
├── cpp/
│   ├── jni.cpp              # JNI entry point (Java ↔ C++ bridge)
│   ├── CMakeLists.txt       # CMake build configuration
│   ├── build_native.bat     # Windows one-click build script
│   ├── include/
│   │   ├── MathUtil.h       # Noise primitives (Simplex / FBM / Worley)
│   │   └── TerrainGen.h     # Terrain generator interface
│   └── src/
│       ├── TerrainGen.cpp   # Core terrain generation (C++ port of JJTH system)
│       ├── Carver.cpp       # Cave / canyon carving
│       ├── Decorator.cpp    # Feature decoration
│       └── WorldLoader.cpp  # World loading helper
└── main/
    └── java/
        └── cn/sanrol/canalize/
            ├── world/       # Terrain generators (NativeChunkGenerator / NativeBiomeSource)
            └── mixin/       # Mixin injections (MixinWorldPresets, etc.)

EXAMPLE/
└── JJTH/                    # Worldgen datapack from the independent JJTH project (reference only)
    └── data/minecraft/worldgen/   # density_function / noise_settings / biome configs
```

---

## References

1. **Musgrave, F. K., Kolb, C. E., & Mace, R. S. (1989).** *The synthesis and rendering of eroded fractal terrains*. ACM SIGGRAPH, 23(3), 41–50. — Source for Ridged FBM and hydraulic erosion concepts.
2. **Inigo Quilez.** *Domain Warping*. [iquilezles.org](https://iquilezles.org/articles/warp/) — Reference implementation for Domain Warping.
3. **Ken Perlin.** *Simplex Noise*. — Reference for the Simplex noise base implementation.
4. **Chang, P., Tang, J., Gross, M., & Azevedo, V. C. (2025).** *How I Warped Your Noise: a Temporally-Correlated Noise Prior for Diffusion Models*. ICLR 2024 (Oral). [arXiv:2504.03072](https://arxiv.org/abs/2504.03072) — Proposes ∫-noise (integral noise), defining noise as per-pixel integrals of a continuous noise field transported via optical flow for temporal coherence; mathematically homologous to this project's Domain Warping in "transporting continuous noise field coordinates."
5. **Deng, Y., Lin, W., Li, L., Smirnov, D., Burgert, R., Yu, N., Dedun, V., & Taghavi, M. H. (2024).** *Infinite-Resolution Integral Noise Warping for Diffusion Models*. [arXiv:2411.01212](https://arxiv.org/abs/2411.01212) — Improves on ∫-noise using Brownian bridge increments to reduce computation by orders of magnitude at infinite resolution, extended to 3D.
6. **Daras, G., Nie, W., Kreis, K., Dimakis, A., Mardani, M., Kovachki, N. B., & Vahdat, A. (2024).** *Warped Diffusion: Solving Video Inverse Problems with Image Diffusion Models*. NeurIPS 2024. [arXiv:2410.16152](https://arxiv.org/abs/2410.16152) — Models frames as continuous 2D functions and video as continuous spatial transformations between frames; trains image diffusion models in function space with equivariance constraints for video restoration and super-resolution.
7. **Na, B., Kim, Y., Park, M., Shin, D., Kang, W., & Moon, I. (2024).** *Diffusion Rejection Sampling*. ICML 2024. [arXiv:2405.17880](https://arxiv.org/abs/2405.17880) — Introduces rejection sampling at each diffusion denoising step to screen and refine intermediate samples, theoretically achieving tighter approximation of the true distribution.
8. **Liu, C., & Vahdat, A. (2025).** *On Equivariance and Fast Sampling in Video Diffusion Models Trained with Warped Noise*. [arXiv:2504.09789](https://arxiv.org/abs/2504.09789) — Theoretically proves that warped-noise training implicitly endows models with equivariance to spatial transformations of input noise (EquiVDM), naturally aligning noise motion with video motion while greatly reducing required sampling steps.

---

## License

This project is open-sourced under the [Mulan Public License, Version 2 (MulanPubL-2.0)](LICENSE).

In short: you are free to use, modify, and distribute this project, but any derivative works **must** be open-sourced under the same MulanPubL-2.0 license and provide the corresponding source code. This license is written in both Chinese and English with equal legal effect; the Chinese version shall prevail in case of divergence.

> Copyright (c) 2026 NullSenpai  
> Canalize is licensed under Mulan PubL v2.  
> You can use this software according to the terms and conditions of the Mulan PubL v2.  
> You may obtain a copy of Mulan PubL v2 at: http://license.coscl.org.cn/MulanPubL-2.0
