# 发布说明

当前发布分支：[ueVite26-JulyMajor-release](https://github.com/GapingPixel/UnrealEngineVite-PhysX)。测试工作会在正式发布前合并到 ueVite26-JulyStaging 分支。诸如 ueVite26-HairWorks 和 ueVite26-JulyRtSettings 等功能分支会在稳定后合并。

此页面记录了当前主要版本包含的内容、正在开发中的功能以及计划中的功能。功能列表是累积的——“已发布（Shipped）”下的所有内容都已包含在当前发布分支中。


## 分支布局

| 分支 | 用途 |
|---|---|
| [ueVite26-JulyMajor-release](https://github.com/GapingPixel/UnrealEngineVite-PhysX) | 当前主要版本。您应该基于此版本进行构建。 |
| `ueVite26-JulyStaging` | 集成分支。向后移植和新增功能会首先发布到这里。 |
| `ueVite26-JuneMajorRelease` | 之前的主要版本，保留用于项目的中期里程碑。 |
| 特性分支 | 目前仅对单个系统进行开发，稳定后将合并。 |

正在进行中的更改应该放在某个分支上，而不是发布版本中。请参阅[提交和 PR 规范](../Contributing/Commit-Conventions.md)。


## 已发布

### 物理引擎

- **Vite PhysX 3.4** 作为发布版本的物理引擎后端。
- **库已升级**，以兼容更新的 Clang 版本和最新的 Android NDK Clang，从而显著提升性能——压力测试中最高可达 2 倍，在 Box Container Pile 10 测试中通常可提升约 1.4 倍。
- **PhysX Blast** 与 Apex Destruction 协同工作。
- **PhysX Fixed Timestep** 作为可选功能，完全保护，确保常规路径零开销。

### 渲染

- **Callisto BRDF** 着色模型：单瓣和双瓣 GGX 高光，带有高光菲涅尔衰减。
- **Toon** 着色模型，灵感来自《罪恶装备》。
- **HBAO+** 环境光遮蔽。
- **基于计算的 SMAA**.
- 改进的 **FXAA**，提供更高图像质量选项。
- 更新的 **TAA** ，以获得更好的时间分辨率。
- 移植并集成了 **DLSS 4.5**, **FSR 2**, **FSR 4**, **XeSS**, **NIS** 和 **AMD Anti-Lag 2**。
- 引擎端 DLSS 渲染支持改进。
- 改进的 **DDGI**，包括对 RT 反射的性能优化。
- **SSAO 快速路径**和更经济的 SSAO。
- 集成**TressFX** 毛发。
- 局部化 IBL.
- 改进的 **ACES** 色彩还原。
- 对 RHI、RT 直接光照、RT 阴影、几何体集合、绘制、人眼自适应和阴影/光照绘制距离进行了渲染优化，并针对主机进行了 AMD 优化。
- **软件遮挡**功能已在 4.27 上游版本后移除。
- Nintendo Switch 专用渲染器基础已在 UE 5.0 及更高版本中移除。

### CPU 和核心

- 针对容器、好友使用、哈希映射、导航网格、体积云、游戏线程、任务图、SIMD、动画系统、纹理处理、流媒体和音频系统进行了 CPU 优化。
- 针对骨骼网格和 Actor 进行了运行时优化。
- 更新了类，以便更轻松地向后移植 UE5 代码库（游戏框架和容器）。
- 游戏玩法（Gameplay）能力系统更新。
- Oodle 更新。
- 现代化控制台系统。

### 构建、工具和内容

- 完全合并 [UE 4.27 Plus](https://github.com/EpicGames/UnrealEngine/tree/4.27-plus)、[NvRTX 4.27](https://github.com/NvRTX/UnrealEngine/tree/NvRTX_Caustics-4.27)、[NvRTX 5.0](https://github.com/NvRTX/UnrealEngine/tree/nvrtx-5.0)、[DLSS](https://github.com/NvRTX/UnrealEngine/tree/dlss3/sl2-4.27-dlss-plugin)、[TressFX](https://github.com/GPUOpen-Effects/TressFX)、[FSR](https://github.com/GPUOpen-Effects/FidelityFX-FSR) 和 [AMD](https://github.com/GPUOpenSoftware/UnrealEngine/tree/EngineOpt-4.24) 补丁。
- 从 UE 5.0 到 5.8 时代**向后移植了 300 多个**功能。
-兼容 MSVC 工具链至 **14.50 和 SDK 10.0.26100（Visual Studio 2026）**。
- 改进了着色器编译。
- 精简了发布配置中的**运行时 PSO**。
- 改进了编辑器加载并移植了编辑器使用体验优化功能。
- 精简了项目默认插件。
- 提供批量工具来生成已安装的引擎构建并精简代码树。
- 捆绑插件：FSR, Motion Symphony, Houdini, ACL, Kawaii Physics, PhysX Instanced Subsystem,
  Splash Damage Ability System, ImGui 和 ImGui Tools 等。请参阅[已捆绑的插件](../Plugins/Bundled-Plugins.md)部分。
- [ImGui](https://github.com/segross/UnrealImGui) 与基准测试工具集成。
- 大量工具链更新、C++ 现代化、更快的渲染速度、引擎修复、已解决的内存泄漏和卡顿问题。
- 移动端改进和修复。

!!! 笔记
    
    * [DLSS](https://github.com/NvRTX/UnrealEngine/tree/dlss3/sl2-4.27-dlss-plugin)（深度学习超级采样，Deep Learning Super Sampling）是NVIDIA开发的一套AI驱动的图形渲染技术。它的核心作用简单来说就是：在游戏里，用更低的硬件性能消耗，换取接近甚至超越原生分辨率的高画质和高帧率体验。

    * “运行时 PSO”指的是在游戏运行过程中，由引擎动态地生成和编译管线状态对象（Pipeline State Object，即PSO）的过程或现象

## 进行中

- 扁平化 UI 重新设计，更贴近 UE5 的视觉语言。
- 集成多线程 FLECS ECS 库与虚幻 Actors 交互。
- 完全支持 C++20。
- 进一步整合 UE5 向后移植功能——约有 1000 个向后移植功能等待发布。
- 渲染特性：改进了网格处理、全局光照、着色模型、环境光遮蔽和镜面反射锯齿处理。
- 进一步升级 ACES，包括色彩空间、HDR 处理和色调映射器。
- 大规模关卡优化。
- 进一步改进了着色器编译。
- CACAO 环境光遮蔽。
- PhysX Flex 和 Flow：跨厂商 GPU 加速粒子，支持 AMD 计算和 NVIDIA CUDA 路径。
- 用于海洋渲染的细分曲面水体，集成到 RT 场景中以实现反射效果。
- 提升了“光线追踪环境光遮蔽（Ray-Traced Ambient Occlusion, RTAO）” 和 RT 阴影的性能。
- 引擎级多线程时钟聚合，提升指令一致性。
- 核心 C++ 库升级和核心引擎数学运算升级。
- 光线追踪渲染方面进行了重大改进。

## 计划中

- [AMD 单次降采样器](https://github.com/GPUOpenSoftware/UnrealEngine/tree/FidelityFXSPD-4.26/UnrealEngine).
- 改进的 SSGI
- 自定义关卡编辑器

## 跟踪

| 资源 | 链接 |
|---|---|
| 工作计划 | [Trello 看板](https://trello.com/b/JKyBFS5X/ue-vite-physx-vite-studio-fork) |
| 反向移植跟踪器 | [GitHub 项目](https://github.com/users/GapingPixel/projects/1/views/1) |
| 示例项目 | [ViteStudio-Tech](https://github.com/ViteStudio-Tech) |

## 另请参阅

- [引擎概述](./Engine-Overview.md)
- [反向移植工作流](../Contributing/Backporting.md)
- [已捆绑的插件](../Plugins/Bundled-Plugins.md)
- [推荐的插件](../Plugins/Proposed-Plugins.md)
