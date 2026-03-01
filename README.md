# Canalize

> 一个通过 JNI 调用高性能 C++ 原生库，完全重写 Minecraft 主世界地形生成的 NeoForge 模组。

[![Minecraft](https://img.shields.io/badge/Minecraft-1.21.1-green)](https://www.minecraft.net/)
[![NeoForge](https://img.shields.io/badge/NeoForge-21.1.219-orange)](https://neoforged.net/)
[![Java](https://img.shields.io/badge/Java-21-blue)](https://www.oracle.com/java/)
[![License](https://img.shields.io/badge/License-AGPL--3.0-blue)](LICENSE)

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
|  极端高度 | 主世界高度扩展至 1808 格，支持超过 1700 格高的巨型山脉 |
|  原生加速 | C++17 实现噪声内核，AVX2 SIMD 并行计算 |
|  水力侵蚀 | 基于 Domain Warping 模拟流水侵蚀，雕刻山谷与河道 |
|  岩石细节 | 山顶混合 Worley 细胞噪声，呈现真实岩石质感 |
|  平滑过渡 | `smoothstep` 生物群系插值，消除海洋/平原/山脉之间的硬边界 |
|  线程安全 | 无状态哈希函数替代 `srand()`/`rand()`，杜绝竞态条件 |

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
│   ├── jni.cpp          # 原生噪声算法实现（Simplex / FBM / Worley / Domain Warping）
│   ├── CMakeLists.txt   # CMake 构建配置
│   └── build_native.bat # Windows 一键构建脚本
└── main/
    └── java/
        └── cn/sanrol/canalize/
            ├── worldgen/  # 地形生成器（NativeChunkGenerator / NativeBiomeSource）
            └── mixin/     # Mixin 注入（MixinWorldPresets 等）
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

本项目基于 [GNU Affero General Public License v3.0 (AGPL-3.0)](LICENSE) 开源。

简而言之：你可以自由使用、修改和分发本项目，但任何基于本项目的衍生作品（包括以网络服务形式提供）**必须**以同样的 AGPL-3.0 许可证开源。
