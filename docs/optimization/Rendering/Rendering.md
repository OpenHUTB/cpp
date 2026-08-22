# 渲染

Vite 的渲染器是一个延迟渲染的 SM5 渲染器，采用与引擎无关的 DXR 光线追踪管线。全局光照使用 [DDGI](./DDGI-Dynamic.md) 而非 Lumen，几何细节使用[细分曲面](./Tessellation.md)而非 Nanite，抗锯齿默认使用原生分辨率的 [SMAA](./Anti-Aliasing.md) 而非时间重建。

!!! 笔记
    “SM5 渲染器”指的是使用 Shader Model 5.0（着色器模型 5.0）这一图形特性级别来驱动渲染管线的渲染器。它定义了 GPU（显卡）需要支持哪些指令和功能，主要与 DirectX 11 API 绑定。


这是手册中篇幅最长的部分，因为渲染是 Vite 与 Unreal Engine 4.27 和 UE5 差异最大的地方。

## 渲染器的组成结构

该管线基于 Unreal Engine 4.27 的延迟渲染器，继承了 NvRTX Caustics 的 DirectX 12 和 DXR 改进，并在此基础上添加了 Vite 自身的功能。其重要的结构特性在于光线追踪管线与引擎*无关*：加速结构的构建和更新独立于任何特定的光照解决方案，这使得 DDGI、RTXDI 和各种光线追踪效果能够共存并独立启用。


这与 UE 5.1+ 的架构截然相反，在 UE 5.1+ 中，光线追踪场景构建与 Lumen、Nanite 和 GPU Scene 耦合在一起。参见 [NvRTX 4.27 的相关说明](../EngineOverview/Why-NvRTX-427.md)。

**注意：** Vite 默认在新项目中启用光线追踪效果。这是为了方便用户发现，但也意味着您需要自行决定关闭哪些功能。参见[光线追踪](./Ray-Tracing.md)。


## 章节内容

### 全局光照

- [全局光照](./Global-Illumination.md) — 选择可用的全局光照解决方案
- [动态 DDGI](./DDGI-Dynamic.md) — 实时光线追踪辐照度探针体积
- [静态 DDGI](./DDGI-Static.md) — 烘焙时间近乎瞬时的烘焙探针体积
- [SSGI](./SSGI.md) — 屏幕空间全局光照（Screen-Space GI），以及它为何与 DDGI 并存

### 光线追踪

- [光线追踪](./Ray-Tracing.md) — 概述和主要开关
- [光线追踪反射](./RT-Reflections.md)
- [光线追踪阴影和环境光遮蔽](./RT-Shadows-And-Ambient-Occlusion.md)
- [光线追踪半透明和焦散](./RT-Translucency-And-Caustics.md)
- [RTXDI](./RTXDI.md) — 用于多光源场景的光线追踪直接光照
- [路径追踪](./Path-Tracing.md)

### 表面和几何体

- [着色模型](./Shading-Models.md) — 包括 Callisto BRDF 和 Toon（卡通渲染）
- [细分曲面](./Tessellation.md)
- [毛发渲染](./Hair-Rendering.md) — TressFX

### 图像质量

- [抗锯齿](./Anti-Aliasing.md) &mdash; SMAA, FXAA, TAA 和 MSAA
- [超分辨率和帧生成](./Upscalers.md) &mdash; DLSS, FSR, XeSS, NIS
- [环境光遮蔽](./Ambient-Occlusion.md) — 屏幕空间环境光遮蔽（Screen Space Ambient Occlusion, SSAO）快速路径和 HBAO+
- [色彩管理](./Color-Management.md) — 学院色彩编码系统（Academy Color Encoding System, ACES）、色调映射和 HDR 输出

## 选择渲染配置

合适的起点取决于您的性能目标。以下是[性能目标](../EngineOverview/Performance-Targets.md)中四个目标对应的配置。

| 目标 | 全局光照 | 反射 | 阴影 | 环境光遮蔽             | 几何体 | 抗锯齿（AA）          |
|---|---|---|---|----------------|---|-------------|
| 风格化 4K120 | 动态 DDGI | Raster / SSR | 级联 | SSAO 快速路径 | LODs | SMAA / TAA  |
| 性能 4K60 | DDGI + SSGI | 光线追踪 | 级联 | SSAO / HBAO4+  | 曲面细分 | TAA / SMAA  |
| 保真 4K30 | DDGI + SSGI | 光线追踪 | 级联 | HBAO4+         | 曲面细分 | TAA         |
| 全 RT 1440p30 | DDGI + SSGI | 光线追踪 | 光线追踪 | RTAO           | 曲面细分 | TAA         |
| 无 RT 硬件 | 静态 DDGI | SSR | 级联 | SSAO           | LODs | TAA / SMAA / FXAA |

这些都只是起点，并非最终方案。每项内容对应的控制台变量均已在相应页面中记录，并汇总在[控制台变量参考](../Reference/Console-Variables.md)中。


## 另请参阅

- [性能目标](../EngineOverview/Performance-Targets.md)
- [性能分析与基准测试](../Performance/Profiling.md)
- [着色器编译与 PSO](../Performance/Shader-Compilation-And-PSO.md)
- [控制台变量参考](../Reference/Console-Variables.md)
