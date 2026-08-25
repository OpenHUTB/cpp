# 超分辨率和帧生成

Vite 捆绑了 DLSS 4.5（深度学习超采样，Deep Learning Super Sampling）、DLSS 帧生成、FSR2、FSR 4、XeSS 3 和 NIS 等功能。这些功能是可选的额外性能，并非渲染方案的核心。即使不启用任何这些功能，Vite 也能在原生分辨率下达到其[性能目标](../EngineOverview/Performance-Targets.md)。
请查看 [UpscalerTest 示例项目](https://github.com/ViteStudio-Tech/ueVite-UpscalerTest)。

虚幻 5 将超分辨率视为强制性功能：其渲染器的设计理念是先以部分输出分辨率进行渲染，然后重建剩余部分。Vite 则采取相反的做法。其目标是原生分辨率，而超分辨率功能仅供希望将性能提升空间用于其他用途的用户使用。

这种区别会影响您使用此页面上插件的方式。如果您的项目只有在启用 DLSS Performance 后才能达到帧目标，则说明项目超出了预算——请先解决这个问题，然后再提供超分辨率选项。


## 已捆绑的插件

所有这些插件都位于引擎仓库的 `Engine/Plugins/Runtime` 目录下。默认情况下均未启用；请在 **编辑（Edit）> 插件（Plugins）** 中启用您的项目提供的插件。


| 插件 | 版本 | 功能 | 位置 |
|---|---|---|---|
| NVIDIA DLSS | 8.7.0-NGX310.7.0 (DLSS 4.5) | 超分辨率、光线重建、DLAA | `Nvidia/DLSS` |
| NVIDIA DLSS 帧生成 | 1.3.0-SL2.4.0 (Streamline) | 帧生成和 Reflex | `Nvidia/Streamline` |
| NVIDIA DeepDVC | Streamline | 深度学习动态增强 | `Nvidia/StreamlineDeepDVC` |
| NVIDIA NIS | &mdash; | 图像缩放、空间放大和锐化 | `Nvidia/NIS` |
| DLSS 影片渲染管线支持 | &mdash; | DLSS 集成到影片渲染队列 | `Nvidia/DLSSMoviePipelineSupport` |
| AMD FSR 4 | 4.1.1 | FidelityFX 超分辨率 4 和帧生成DX12 | `VitePlugins/FSR4-427` |
| Intel XeSS | 3.0.5 | Xe 超采样 | `VitePlugins/XeSS_UE4.27_Plugin_v3.0.5` |

FSR 4 和 XeSS 3 值得注意：它们都是向后移植的，在 4.27 版本中并不存在。FSR 4 使用原生 `ffx-api`，需要 DirectX 12。


## 选择要打包的内容

您不需要全部启用。每个启用的超分辨率工具都会增加着色器、二进制文件、软件包大小以及一个设置菜单项，这些都需要进行测试。


| 情况 | 建议 |
|---|---|
| PC 发行版，广泛支持各种硬件 | DLSS + FSR 4。支持 NVIDIA 和 AMD 显卡；如果您需要 Intel Arc 功能，则支持 XeSS。 |
| 专注于 NVIDIA 显卡，重度光线追踪 | DLSS 结合光线重建 |
| 原生分辨率下的抗锯齿质量 | DLAA（DLSS 插件的一部分）或 Vite 的 [SMAA](./Anti-Aliasing.md) |
| 不支持超分辨率，仅支持原生分辨率 | 请勿从此页面下载任何内容。这是一个有效且受支持的配置。 |

DLAA 值得单独提及。它是将 DLSS 的神经网络应用于原生分辨率，而非作为超分辨率工具，因此它与 SMAA 竞争而非互补。它通常比 SMAA 解析出更精细的细节，但会重新引入时间累积效应，因此会产生一些 SMAA 避免的瑕疵。建议同时提供这两种方法，让玩家自行选择。


## 光线重建

光线重建使用神经网络降噪器替换了手动调校的光线追踪效果降噪器。在 Vite 项目中，它会直接与[光线追踪](./Ray-Tracing.md)配置交互：



- 它能显著提升[光线追踪的反射率](./RT-Reflections.md)和 [RTXDI](./RTXDI.md) 质量，而手动调校的降噪器往往难以胜任。 
- 对于 [DDGI](./DDGI-Dynamic.md) 来说，它没有任何益处，因为 DDGI 本身就是无噪声的，无需降噪。

如果你的光线追踪配置是基于 DDGI 的，那么光线重建带来的提升远不如基于流明（Lumen）的方案。发货前请务必进行测量。


## 帧生成

DLSS 帧生成（通过 Streamline）和 FSR 4 帧生成都会合成中间帧。

帧生成可以提高显示的帧率，但不会降低输入延迟——它甚至可能会略微增加延迟，因此 Streamline 插件中同时包含了 Reflex。启用帧生成时，也请启用 Reflex。

**注意：** 帧生成在基础帧率已经很高的情况下效果最佳。从 30 fps 生成到 60 fps 会在快速移动的 UI 和细长几何体周围产生可见的伪影；而从 60 fps 生成到 120 fps 则效果更佳。由于 Vite 本身就以高原生帧率为目标，因此帧生成在这里非常适用——它是一种将 4K60 目标推向高刷新率显示器的方法，而不是一种拯救 30 fps 显示器的方法。


## DLSS 半透明补丁

Vite 包含一个可选的编译时补丁，用于解决放大时的一个特定质量问题：在内部渲染分辨率下分别渲染半透明和体积雾，然后再进行放大，这会导致粒子、玻璃和雾在原本清晰的重建图像上显得模糊。


| 切换 | 默认 | 效果 |
|---|---|---|
| `VITE_DLSS_PATCH` | `0` | 添加超分辨率半透明效果和原生分辨率体积雾 |

启用后，渲染器将获得两个额外的半透明渲染通道：`TranslucencyAfterDOFUpscaledRT` 和 `TranslucencyAfterDOFModulateUpscaledRT`。这两个通道会在放大后以输出分辨率渲染选定的半透明图元。图元可以通过 `bRenderInTranslucencyUpscaledRTPass` 相关标志选择是否启用这些通道，并且这些通道仅在时间放大器实际处于活动状态时才会使用。


此外，它还添加了：

| CVar | 默认值 | 效果 |
|---|---|---|
| `r.VolumetricFog.UseUpScaledSizeVolumetricFog` | `0` | 使用输出分辨率而非内部渲染分辨率计算体积雾网格。 |

该开关定义在 [Engine/Source/Runtime/Core/Public/Misc/CoreDefines.h](https://github.com/OpenHUTB/engine/blob/hutb/Engine/Source/Runtime/Core/Public/Misc/CoreDefines.h) 中，默认关闭，因为它会消耗帧时间，并且仅对启用了超分辨率的项目有效。有关如何更改它，请参阅[编译时开关](../Performance/Compile-Time-Switches.md)。


## 启用超分辨率

### 将 DLSS 添加到项目

1. 打开**编辑（Edit）> 插件（Plugins）**，找到 **NVIDIA DLSS 超分辨率/光线重建/DLAA（NVIDIA DLSS Super Resolution/Ray Reconstruction/DLAA）**，启用它并重启编辑器。

2. 对于帧生成，也请启用 **NVIDIA DLSS 帧生成（NVIDIA DLSS Frame Generation）**。

3. 在设置菜单中显示该选项之前，请先通过 DLSS 蓝图库在运行时查询其支持情况。硬件和驱动程序的支持情况各不相同，一个默默无闻的选项还不如没有选项。

4. 将质量模式作为玩家设置公开，而不是强制启用。包含一个关闭状态。

5. 测试与[抗锯齿](./Anti-Aliasing.md)设置的交互。DLSS 会替换抗锯齿通道；同时启用 SMAA 会浪费帧时间。

## 另请参阅

- [抗锯齿](./Anti-Aliasing.md)
- [性能目标](../EngineOverview/Performance-Targets.md)
- [光线追踪](./Ray-Tracing.md)
- [已捆绑的插件](../Plugins/Bundled-Plugins.md)
- [编译时开关](../Performance/Compile-Time-Switches.md)
