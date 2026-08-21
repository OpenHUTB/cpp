# 向后移植


Vite 七月版本目前有超过 300 个从 UE5.0 到 UE 5.8 的向后移植。


Vite 是一款 NvRTX 4.27 Plus 分叉，与最新引擎开发的技术保持同步。这意味着向后移植是升级源代码的例行活动。

## 已向后移植的内容

| 来源 | 示例                                                                                                                              |
|---|---------------------------------------------------------------------------------------------------------------------------------------|
| UE5 | 重要的渲染优化、全面的 CPU 优化、插件、QoL 改进、动画功能、第三方库 |
| Vendor SDKs | [DLSS 4.5, FSR 4, XeSS 3.0.5, Streamline](../Rendering/Upscalers.md), TressFX 5.0, Blast                                                           |
| 第三方插件 | ACL, Motion Symphony, [Kawaii Physics](../Plugins/Bundled-Plugins.md)                                                                            |

!!! 笔记
    UE的QoL（Quality of Life，生活质量）改进，指的就是那些不直接增加新功能，但能显著提升开发者效率、减少重复劳动、优化体验的细节改进。

## 在向后移植之前

### 向后移植可行性检查

1. <b>检查许可证。</b> UE5 代码已获得 Epic 的许可证，该许可证允许在虚幻引擎分支中使用，但不允许任意重新分发。第三方代码必须获得许可——MIT、Apache 2.0、BSD、Zlib。请参阅[编码指南](Coding-Guidelines.md)。

2. <b>检查依赖关系图。</b>依赖于 Nanite、Lumen、UE5 RDG API 表面或 Chaos 物理接口的 UE5 功能将无法完全向后移植。在编写代码之前确定实际需要的内容。

3. <b>检查 ABI 影响。</b>如果该功能需要更改着色器可见结构或打包位掩码，则无法按原样向后移植。请参阅[ABI 规则](Coding-Guidelines.md)。

4. <b>检查是否值得。</b>为了解决UE5问题而存在的UE5功能在Vite中可能没有价值。专注于优化、第三方库更新、插件更新、SDK 更新和修复。

## 常见障碍

* **RDG API 差异**

    UE5 的渲染依赖图（Render Dependency Graph, RDG） API 与 4.27 的有很大不同。传递声明、资源转换和统一缓冲区创建都不同。这通常是机械翻译，但涉及渲染通道的每一行。

* **Chaos 与 PhysX**

    UE5 中任何涉及物理的内容都假定为 Chaos。 Vite 是 PhysX — 请参阅 <a href="../Physics/PhysX.md">PhysX</a>。物理相邻的向后移植需要重写接口，而不是翻译。

* **核心类型变化**

    UE5 更改了多个核心类型，最明显的是从浮点型 FVector 转变为双精度型。向后移植意味着从头到尾恢复这些更改，并观察精度承受负载的地方。

* **模块重组**

    UE5 拆分并重命名模块。包含、构建依赖项和模块名称都需要重新映射到其 4.27 位置。

* **着色模型槽**

    着色模型是一个有限的枚举。 Vite 已经添加了 <a href="../Rendering/Shading-Models.md">Callisto BRDF、Toon 和 Lit Reactive</a>。添加另一个会消耗一个插槽并全面添加着色器排列 -请参阅<a href="../Performance/Shader-Compilation-And-PSO.md">着色器编译和 PSO</a>。


## 执行相关工作

### 保持变更可追溯性

使用现有的内联注释约定标记向后移植的区域，以便在将来向上游合并后能够识别它们：


```c++
// AKCHANGES START
// Backported from UE5.3, CL 12345678
...
// AKCHANGES END
```

### 保护那些会带来成本的功能

如果某个功能会带来运行时或编译时的成本，而并非所有项目都需要，那么就将其置于[编译时开关](../Performance/Compile-Time-Switches.md)或控制台变量之后，默认值为关闭。Vite 现有的开关遵循这种模式：

| 开关 | 默认值 | 保护 |
|---|---|---|
| `VITE_PHYSX_FIXED_TIMESTEP` | 0 | 固定时间步物理 |
| `VITE_RT_PSO_DEBLOAT` | 1 | 编译出大多数光线追踪排列 |
| `VITE_O_SSAO` | 1 | 优化的SSAO路径 |
| `VITE_DLSS_PATCH` | 0 | DLSS 半透明度和体积雾修复 |
| `VITE_NVRTX_TRANSLUCENCY_DEPTH` | 0 | NvRTX 半透明深度处理 |

!!! 注意
    `VITE_RT_PSO_DEBLOAT`默认为*限制性(restrictive)*值。当某个功能的成本是着色器排列计数时，保持快速构建的默认值会获胜，并且该功能被记录为选择加入。

### 与基准进行测试

移植到 Vite 的功能需要根据 Vite 的[性能目标](../EngineOverview/Performance-Targets.md)进行评估，而不是根据它们在 UE5 中的表现。针对 UE5 帧预算优化的功能，在运行于 Vite 目标硬件上的 4.27 项目中可能无法承受。


## 记录向后移植

每个向后移植都需要：

| 项 | 在哪里 |
|---|---|
| 源链接、上游提交或标签 | 提交正文——另请参阅[提交约定](Commit-Conventions.md) |
| 转变过程中发生了什么变化 | 提交正文 |
| 任何新的控制台变量或开关 | 文档页面和 [编译时开关](Compile-Time-Switches.md)（如果适用） |
| 可用性警告 | 该功能自己的页面，显着 |

最后一项比看起来更重要。默认情况下编译出来的向后移植功能，或者需要启用插件的功能，否则每个尝试使用它的人都会报告为损坏。

## 资产向后移植

从 UE5 向后移植*资产*是一个不同的问题，由资产降级器而不是代码处理。请参阅[从 UE5 迁移](../GettingStarted/Migrating-From-UE5.md) 和 [建议的插件](../Plugins/Proposed-Plugins.md)。

## 另请参阅

- [贡献](Contributing.md)
- [编码指南](Coding-Guidelines.md)
- [提交约定](Commit-Conventions.md)
- [编译时开关](../Performance/Compile-Time-Switches.md)
- [从 UE5 迁移](../GettingStarted/Migrating-From-UE5.md)
