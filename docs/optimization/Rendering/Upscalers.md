# 超分辨率和帧生成

!!! note "概要"
    Vite 捆绑了带光线重建与多帧生成的 NVIDIA DLSS 4.5、带 FSR 3.1 回退与不挑显卡的帧生成的 AMD FSR 4、带 XeFG 的 Intel XeSS 3.1、NVIDIA NIS，以及 Reflex 与 Anti-Lag 2 两套延迟 SDK。它们只是**可选的余量**，不是渲染方案：不启用其中任何一个，Vite 也能在原生分辨率下达到[性能目标](../EngineOverview/Performance-Targets.md)。

Vite 捆绑了 DLSS 4.5、DLSS 帧生成、FSR 4、XeSS 3.1、NIS 等插件，以及 Reflex 与 Anti-Lag 2 两套延迟 SDK。这些功能是可选的额外余量，并非渲染方案的核心；即使全部不启用，Vite 也能在原生分辨率下达到其[性能目标](../EngineOverview/Performance-Targets.md)。
请查看 [UpscalerTest 示例项目](https://github.com/ViteStudio-Tech/ueVite-UpscalerTest)。

虚幻 5 将超分辨率视为强制性功能：其渲染器的设计理念是先以部分输出分辨率进行渲染，然后重建剩余部分。Vite 则采取相反的做法。其目标是原生分辨率，而超分辨率功能仅供希望将性能提升空间用于其他用途的用户使用。

这种区别会影响您使用此页面上插件的方式。如果您的项目只有在启用 DLSS Performance 后才能达到帧目标，则说明项目超出了预算——请先解决这个问题，然后再提供超分辨率选项。


## 已捆绑的插件

所有这些插件都位于引擎仓库的 `Engine/Plugins/Runtime` 目录下。默认情况下均未启用；请在 **编辑（Edit）> 插件（Plugins）** 中启用您的项目提供的插件。版本以 9 月主要版本为准。


| 插件 | 版本 | 功能 | 位置 |
|---|---|---|---|
| NVIDIA DLSS | 8.7.0-NGX310.7.0（DLSS 4.5 预设） | 超分辨率、光线重建、DLAA | `Nvidia/DLSS` |
| NVIDIA DLSS 帧生成 | 基于 Streamline SDK 2.12.0 的 1.3.0 插件 | 帧生成，含多帧生成 | `Nvidia/Streamline` |
| NVIDIA Reflex | 1.6 | 降低延迟 | `Nvidia/Reflex` |
| NVIDIA RTX Dynamic Vibrance | 1.3.0（Streamline） | 深度学习动态鲜艳度 | `Nvidia/StreamlineDeepDVC` |
| NVIDIA NIS | 1.2.1 | 图像缩放：空间放大与锐化 | `Nvidia/NIS` |
| Movie Render Queue DLSS/DLAA 支持 | 2.3.3 | 影片渲染队列的 DLSS 集成 | `Nvidia/DLSSMoviePipelineSupport` |
| AMD FSR 4 | 4.1.1 | FSR 4 机器学习超分，带 FSR 3.1 回退、原生 AA，以及与任意超分器配合的帧生成；DirectX 12 | `VitePlugins/FSR4-427/FSR4` |
| AMD Anti-Lag 2 | 2.0.4 | AMD 显卡上降低延迟 | `VitePlugins/AntiLag2.0.4` |
| Intel XeSS | 3.1.0 | Xe 超采样与 XeFG 帧生成 | `VitePlugins/XeSS_UE4.27_Plugin_v3.1.0` |

FSR 4 和 XeSS 3 值得注意：两者都是 4.27 原生版本里没有的向后移植。FSR 4 使用原生 `ffx-api`，并要求 DirectX 12。这套插件里的大部分由 6 月主要版本引入；8 月主要版本把 XeSS 换到 3.1.0，并把 DLSS 运行时换到 NGX 310.7.129。

早期版本里的 FSR 2 插件仍留在仓库中（`VitePlugins/FSR2-427`），但它的描述文件已被改名为 `.uplugin.disabled`，引擎不会加载它。FSR 4 内置的 FSR 3.1 路径覆盖了它原本服务的硬件。

!!! warning
    [ViteSetup](../Tools/ViteSetup.md) 的 Express 路径会把 `Engine\Plugins\Runtime\VitePlugins` 下的所有插件（ACL 与 MotionSymphony 除外）移到 `ViteDebloat_Moved`，FSR 4、Anti-Lag 2 与 XeSS 都在其中。需要它们时请选 Custom 路径，或者从 `ViteDebloat_Moved` 里把文件夹拷回来。NVIDIA 那组插件不受影响。

### 任意渲染分辨率

`r.Upscaler.ScreenPercentageOverride` 让 FSR、DLSS 与 XeSS 接受任意内部渲染分辨率，而不只是各自的固定质量档。这些插件自己掌管屏幕百分比，会忽略 `r.ScreenPercentage`，所以要用这个变量来设：`0`（默认）表示仍由质量档预设决定，`1`–`100` 则强制为该屏幕百分比，并会被钳制到当前超分器支持的范围内。它适合做基准测试，也适合给玩家做一个分辨率滑条。


## 选择要打包的内容

您不需要全部启用。每个启用的超分辨率工具都会增加着色器、二进制文件、软件包大小以及一个设置菜单项，这些都需要进行测试。


| 情况 | 建议 |
|---|---|
| PC 发行版，广泛支持各种硬件 | DLSS + FSR 4。支持 NVIDIA 和 AMD 显卡；如果您需要 Intel Arc 功能，则支持 XeSS。 |
| 专注于 NVIDIA 显卡，重度光线追踪 | DLSS 结合光线重建 |
| 原生分辨率下的抗锯齿质量 | DLAA（DLSS 插件的一部分）、FSR 4 的原生 AA，或 Vite 的 [SMAA](./Anti-Aliasing.md) |
| 不做超分，只走原生分辨率 | 此页上的插件一个都不要打包。这是一个合法且受支持的配置。 |

DLAA 值得单独提及。它是将 DLSS 的神经网络应用于原生分辨率，而非作为超分辨率工具，因此它与 SMAA 竞争而非互补。它通常比 SMAA 解析出更精细的细节，但会重新引入时间累积效应，因此会产生一些 SMAA 避免的瑕疵。建议同时提供这两种方法，让玩家自行选择。


## 光线重建

光线重建用神经网络降噪器取代了为光追效果手工调校的降噪器。在 Vite 项目中，它会直接与[光线追踪](./Ray-Tracing.md)配置交互：



- 在运动场景中，它能显著提升[光线追踪反射](./RT-Reflections.md)与 [RTXDI](./RTXDI.md) 的质量，这正是手工调校的降噪器最吃力的时候。
- 对于 [DDGI](./DDGI-Dynamic.md) 来说，它没有任何益处，因为 DDGI 本身就是无噪声的，无需降噪。

如果你的光线追踪配置是基于 DDGI 的，那么光线重建带来的提升远不如基于流明（Lumen）的方案。发货前请务必进行测量。


## 帧生成

引擎随附三种帧生成器：

- **DLSS 帧生成**，经由 Streamline，包含 DLSS-G 的多帧生成倍率。
- **FSR 帧生成**，在呈现时从交换链驱动。它不挑超分器：与 DLSS、XeSS 或原生渲染配合都可以，也能与 FSR 一起用；只要选中了任一帧生成器，就会强制输出完整的速度矢量，避免生成帧出现重影。
- **XeFG**，Intel 的帧生成，通过 XeSS 3 在 UE 4.27 上启用。

帧生成提高显示帧率，但不会降低输入延迟——反而可能让延迟略微变差。启用时请搭配 NVIDIA 硬件上的 **Reflex** 或 AMD 硬件上的 **Anti-Lag 2**。

!!! note
    帧生成在基础帧率本来就高时效果最好。从 30 fps 生成到 60 fps，会在快速移动的 UI 和细薄几何体周围产生可见伪影；从 60 fps 生成到 120 fps 则可信得多。Vite 本来就以高原生帧率为目标，所以帧生成在这里的位置很合适——它用来把 4K60 的目标推向高刷新率显示器，而不是用来挽救 30 fps。


## DLSS 半透明补丁

Vite 包含一个可选的编译时补丁，用于解决放大时的一个特定质量问题：在内部渲染分辨率下分别渲染半透明和体积雾，然后再进行放大，这会导致粒子、玻璃和雾在原本清晰的重建图像上显得模糊。


| 切换 | 默认 | 效果 |
|---|---|---|
| `VITE_DLSS_PATCH` | `0` | 增加输出分辨率的半透明通道，以及原生分辨率的体积雾 |

启用后，渲染器将获得两个额外的半透明渲染通道：`TranslucencyAfterDOFUpscaledRT` 和 `TranslucencyAfterDOFModulateUpscaledRT`。这两个通道会在放大后以输出分辨率渲染选定的半透明图元。图元可以通过 `bRenderInTranslucencyUpscaledRTPass` 相关标志选择是否启用这些通道，并且这些通道仅在时间类超分器真正启用时才会使用。


此外，它还添加了：

| CVar | 默认值 | 效果 |
|---|---|---|
| `r.VolumetricFog.UseUpScaledSizeVolumetricFog` | `0` | 使用输出分辨率而非内部渲染分辨率计算体积雾网格。 |

该开关定义在 [Engine/Source/Runtime/Core/Public/Misc/CoreDefines.h](https://github.com/OpenHUTB/engine/blob/hutb/Engine/Source/Runtime/Core/Public/Misc/CoreDefines.h) 中，默认关闭，因为它会消耗帧时间，并且仅对启用了超分辨率的项目有效。有关如何更改它，请参阅[编译时开关](../Performance/Compile-Time-Switches.md)。


## 启用超分辨率

### 将 DLSS 添加到项目

1. 打开**编辑（Edit）> 插件（Plugins）**，找到 **NVIDIA DLSS 超分辨率/光线重建/DLAA（NVIDIA DLSS Super Resolution/Ray Reconstruction/DLAA）**，启用它并重启编辑器。

2. 对于帧生成，也请启用 **NVIDIA DLSS 帧生成（NVIDIA DLSS Frame Generation）**；要降低延迟就再加上 **Reflex**。

3. 在设置菜单中显示该选项之前，请先通过 DLSS 蓝图库在运行时查询其支持情况。硬件与驱动的支持情况各不相同，一个默默不生效的选项还不如没有。

4. 将质量模式作为玩家设置公开，而不是强制启用。包含一个关闭状态。

5. 测试与[抗锯齿](./Anti-Aliasing.md)设置的交互。DLSS 会替换抗锯齿通道；同时启用 SMAA 会浪费帧时间。

## 另请参阅

- [抗锯齿](./Anti-Aliasing.md)
- [性能目标](../EngineOverview/Performance-Targets.md)
- [光线追踪](./Ray-Tracing.md)
- [已捆绑的插件](../Plugins/Bundled-Plugins.md)
- [编译时开关](../Performance/Compile-Time-Switches.md)
