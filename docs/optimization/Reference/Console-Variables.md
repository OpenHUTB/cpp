# 控制台变量

此列表包含 Vite 特有的控制台变量以及您最常用的默认变量，并按子系统分组。这是一个精选列表，并非完整列表——如需查看完整列表，请使用`help`或控制台的自动补全功能。

**警告**：此列表中的几个光线追踪变量在默认构建中会**被编译掉**。它们会成功设置，但不会渲染任何内容。每个变量的可用性都有说明；请参阅[编译时开关](../Performance/Compile-Time-Switches.md)。


您可以在运行时通过控制台设置变量，也可以在 `Config\DefaultEngine.ini` 文件中永久设置这些变量：

```ini
[/Script/Engine.RendererSettings]
r.Vite.SMAA.Mode=1
r.HBAO.Enable=1
```

## Vite 特有的

这些在 UE 4.27 版本中不存在。

| 变量 | 默认值 | 用途 |
|---|---|---|
| `r.Vite.SMAA.Mode` | 1 | SMAA 抗锯齿质量。`0` = 低，`1` = 高。参见[抗锯齿](../Rendering/Anti-Aliasing.md)。  |
| `r.Vite.SSAO` | &mdash; | Vite 的优化 SSAO 路径。参见[环境光遮蔽](../Rendering/Ambient-Occlusion.md)。  |
| `r.AntiAliasing.SMAA.Debug` | &mdash; | SMAA 调试可视化 |
| `p.VitePhysXFixedTimestep.Enabled` | 0 | 启用固定时间步长物理效果 |
| `p.VitePhysXFixedTimestep.DeltaTime` | 1/60 | 固定步长持续时间（秒） |
| `p.VitePhysXFixedTimestep.MaxTimesteps` | &mdash; | 每帧最大步数 |
| `p.VitePhysXFixedTimestep.MaxCumulativeExtraSteps` | &mdash; | 累积追赶步数上限 |
| `p.VitePhysXFixedTimestep.LimitType` | &mdash; | 过载处理方式 |
| `p.VitePhysXFixedTimestep.InterpolationMode` | &mdash; | 固定步长之间的渲染插值 |

**注意：** `p.VitePhysXFixedTimestep.*` 变量需要使用 `VITE_PHYSX_FIXED_TIMESTEP=1` 进行构建。请参阅[固定时间步长](../Physics/Fixed-Timestep.md)。


## 光线追踪

| 变量 | 默认值 | 可用性 | 页面 |
|---|---|---|---|
| `r.RayTracing.Reflections` | 0 | 可用 | [RT 反射](../Rendering/RT-Reflections.md) |
| `r.RayTracing.Shadows` | 0 | 可用 | [RT 阴影和 AO](../Rendering/RT-Shadows-And-Ambient-Occlusion.md) |
| `r.RayTracing.AmbientOcclusion` | 0 | 可用 | [环境光遮蔽](../Rendering/Ambient-Occlusion.md) |
| `r.RayTracing.SkyLight` | &mdash; | 可用 | [光线追踪](../Rendering/Ray-Tracing.md) |
| `r.RayTracing.GlobalIllumination` | -1 | **已编译移除** | [全局光照](../Rendering/Global-Illumination.md) |
| `r.RayTracing.Translucency` | 0 | **已编译移除** | [RT 半透明和焦散](../Rendering/RT-Translucency-And-Caustics.md) |
| `r.RayTracing.SampledDirectLighting` | 0 | **已编译移除** | [RTXDI](../Rendering/RTXDI.md) |

值为 `-1` 表示该设置由后期处理体积驱动，而非控制台变量。


### 剔除和场景设置

| 变量 | 用途 |
|---|---|
| `r.RayTracing.Culling` | 光线追踪实例剔除模式 |
| `r.RayTracing.Culling.Radius` | 剔除距离 |
| `r.RayTracing.Culling.Angle` | 角度剔除阈值 |
| `r.RayTracing.SceneCaptures` | 是否使用场景捕获构建光线追踪场景 |
| `r.RayTracing.PreGather` | 预收集通道控制 |

剔除设置是光线追踪性能最有效的调节手段之一，Vite 修改了其中一些默认设置——请参阅[引擎默认设置](../Performance/Engine-Defaults.md)。


### 天空光照

| 变量 | 用途 |
|---|---|
| `r.RayTracing.SkyLight.SamplesPerPixel` | 采样数 |
| `r.RayTracing.SkyLight.MaxRayDistance` | 光线长度限制 |
| `r.RayTracing.SkyLight.ScreenPercentage` | 天空光通道的分辨率比例 |
| `r.RayTracing.SkyLight.Denoiser` | 降噪器选择 |
| `r.RayTracing.SkyLight.EnableMaterials` | 命中时评估材质 |
| `r.RayTracing.SkyLight.EnableTwoSidedGeometry` | 双面几何体处理 |

当天空光通道的计算量过大时，可以考虑使用 `ScreenPercentage` 和 `SamplesPerPixel` 这两个参数。


### 半透明效果

默认情况下已编译关闭。

| 变量 | 用途 |
|---|---|
| `r.RayTracing.Translucency.HalfRes` | 半分辨率半透明效果 |
| `r.RayTracing.Translucency.HybridDepthThreshold` | 混合半透明深度阈值 |
| `r.RayTracing.Translucency.HybridDepthBias` | 混合半透明深度偏差 |
| `r.RayTracing.PrimaryRays.IncludeDOF` | 在主光线中包含景深 |

### RTXDI（采样直接光照）

默认情况下已编译关闭。`r.RayTracing.SampledDirectLighting` 是主开关；`r.RayTracing.SampledLighting.*` 系列（包含 60 多个变量，涵盖储层计数、空间和时间重采样、ReGIR 单元配置以及每种光照类型的切换）对其进行微调。


首先需要了解的是：

| 变量 | 默认值 | 用途 |
|---|---|---|
| `r.RayTracing.SampledLighting.Mode` | 1 | 采样模式 |
| `r.RayTracing.SampledLighting.InitialSamples` | 8 | 初始候选样本数 |
| `r.RayTracing.SampledLighting.Spatial` | 1 | 空间重采样 |
| `r.RayTracing.SampledLighting.Temporal` | 1 | 时间重采样 |
| `r.RayTracing.SampledLighting.NumReservoirs` | -1 | 储层数量，-1 表示自动 |
| `r.RayTracing.SampledLighting.Denoiser` | &mdash; | 降噪器选择 |
| `r.RayTracing.SampledLighting.DebugMode` | &mdash; | 调试可视化 |

参见 [RTXDI](../Rendering/RTXDI.md)。

### 焦散效果

网格和水体的焦散效果主要由后期处理体积设置驱动，而非控制台变量，并且默认情况下两者都会被编译掉。相关变量：

| 变量 | 用途 |
|---|---|
| `r.RayTracing.BuildRayTracingMeshForCaustics` | 构建支持焦散效果的光线追踪几何体 |
| `r.ParallelCausticsMap` | 并行焦散贴图渲染 |
| `r.PathTracing.ApproximateCaustics` | 在路径追踪器中实现近似焦散效果 |

参见 [光线追踪半透明和焦散](../Rendering/RT-Translucency-And-Caustics.md)。

## 全局光照

| 变量 | 用途 |
|---|---|
| `r.RTXGI.DDGI` | 主 DDGI 开关 |
| `r.RTXGI.DDGI.LightingPass.Scale` | 光照通道分辨率缩放 |
| `r.RTXGI.DDGI.LightingPass.RelativeDistanceThreshold` | 距离拒绝阈值 |
| `r.RTXGI.DDGI.LightingPass.NormalPower` | 法线权重 |
| `r.RTXGI.DDGI.ProbesTextureVis` | 探针纹理可视化 |
| `r.RTXGI.DDGI.ProbesTextureVis.IrradianceScalar` | 可视化缩放 |
| `r.RTXGI.DDGI.StatVolume` | 每个体积的统计信息 |
| `r.RTXGI.MemoryUsed` | 报告 DDGI 内存使用情况 |

RTXGI 插件默认启用。请参阅[动态 DDGI](../Rendering/DDGI-Dynamic.md) 和[全局光照](../Rendering/Global-Illumination.md)。


## 环境光遮蔽

| 变量 | 默认值 | 用途 |
|---|---|---|
| `r.HBAO.Enable` | 0 | 启用 HBAO+。乘以屏幕空间 AO 缓冲区。 |
| `r.HBAO.HighPrecisionDepth` | 0 | 0 = FP16 内部深度，1 = FP32。使用 FP32 可避免远处物体出现自遮挡条带。 |
| `r.HBAO.GBufferNormals` | 1 | 0 = 从深度重建法线，1 = 获取 GBuffer 法线 |

HBAO+ 在后处理体积中提供了额外的体积级设置：幂指数（默认值 2.0）、半径（2.0）、偏移量（0.1）、小尺度 AO（1.0）、模糊半径（2 像素）、模糊锐度（16.0）、最大视深（9500）、深度锐度（50.0）以及前景和背景 AO 距离。

**注意：** `r.HBAO.Enable` 帮助文本将 HBAO+ 描述为仅适用于 DX11。此信息已过时；目前已有可用的 D3D12 实现。请参阅[环境光遮蔽 (Ambient Occlusion)](../Rendering/Ambient-Occlusion.md)。


## 曲面细分

| 变量 | 默认值 | 用途 |
|---|---|---|
| `r.TessellationAdaptivePixelsPerTriangle` | 48.0 | 全局细分因子乘数。数值越低，三角形越多。 |

参见[细分曲面](../Rendering/Tessellation.md).

## 头发

| 变量 | 用途 |
|---|---|
| `r.TressFX.StrandsMode` | 调试渲染模式。0 = 关闭，1 = 模拟发丝，2 = 渲染发丝并受模拟影响，3 = 头发 UV，4 = 根部 UV，5 = 种子，6 = 尺寸 |
| `r.TressFX.Interoplation.FrustumCulling` | 用于插值的视锥体剔除。默认值为 1。 |
| `r.TressFX.MorphTargetMeshVisualization` | 变形目标网格可视化 |

参见 [头发渲染](../Rendering/Hair-Rendering.md)。

## 体积雾

| 变量 | 用途 |
|---|---|
| `r.RayTracing.VolumeFogMode` | 光线追踪体积雾模式 |
| `r.VolumetricFog.UseUpScaledSizeVolumetricFog` | 以输出分辨率而非内部分辨率渲染体积雾 |

**注意：** `r.VolumetricFog.UseUpScaledSizeVolumetricFog` 仅在 `VITE_DLSS_PATCH=1` 的版本中存在。它的作用是在放大时修复体积雾的分辨率。请参阅[超分辨率](../Rendering/Upscalers.md)部分。


## 性能分析

标准的虚幻引擎，但值得集中展示：

| 命令 | 显示 |
|---|---|
| `stat unit` | 帧、游戏、绘制、GPU 和 RHIT 的计时。从这里开始。 |
| `stat unitgraph` | 随时间变化的相同数据 |
| `stat game` | 游戏线程细分 |
| `stat scenerendering` | 渲染线程细分 |
| `stat gpu` | GPU 通道细分 |
| `stat physics` | 物理求解器时间 |
| `stat rhi` | RHI 资源统计信息 |
| `stat memory` | 内存使用情况 |
| `profilegpu` | 单帧 GPU 捕获 |
| `recompileshaders changed` | 重新编译已修改的着色器 |
| `recompileshaders global` | 重新编译全局着色器 |

参见[性能分析](../Performance/Profiling.md)。

## 自行查找变量

| 命令 | 效果 |
|---|---|
| `help` | 将所有控制台变量和命令输出到日志  |
| `<partial name>` 然后按 Tab 键 | 控制台自动补全 |
| `<variable>` 不带值 | 打印当前值和帮助文本 |
| `DumpConsoleCommands` | 列出命令 |

**警告：** 引擎中的帮助文本有时会过时——例如，HBAO+ 声称仅支持 DX11。当行为与帮助文本不一致时，以源代码为准。


## 另请参阅

- [编译时开关](../Performance/Compile-Time-Switches.md)
- [引擎默认设置](../Performance/Engine-Defaults.md)
- [性能分析](../Performance/Profiling.md)
- [术语表](../Reference/Glossary.md)
