# 性能和优化

Vite 的目标是在主机级硬件上实现原生分辨率和高帧率。本节将介绍如何衡量是否达到这些目标，Vite 已经为您做了哪些调整，以及还有哪些可以改进的地方。

Vite 的性能优化并非后期制作。[性能目标](../EngineOverview/Performance-Targets.md)就是规范，本节的所有内容都是为了帮助您确保达到这些目标。


## 本节内容

| 主题 | 涵盖内容 |
|---|---|
| [性能分析](Profiling.md) | 找出帧的实际运行位置 |
| [引擎默认设置变更](Engine-Defaults.md) | Vite 相对于 4.27 版本的更改及其对项目的影响 |
| [编译时开关](Compile-Time-Switches.md) | `VITE_*` 宏，包括用于移除光线追踪效果的宏 |
| [着色器编译和 PSO](Shader-Compilation-And-PSO.md) | 排列计数、编译时间和管线状态 |
| [精简指南](Debloat-Guide.md) | 减少引擎磁盘占用空间和编译时间 |

## 通用方法

**在进行任何更改之前先进行测量。** 虚幻引擎优化中最常见的错误就是优化了错误的线程。对于受限于游戏线程的游戏，降低着色器复杂度并不会提升速度；对于受限于 GPU 的游戏，减少 Actor 数量也不会提升速度。

**明确你的目标性能。** [四个性能目标](../EngineOverview/Performance-Targets.md)对应不同的渲染配置，符合 4K30 保真度目标的场景并不一定符合 4K120 风格化目标。务必先确定目标性能。

**优先减少工作量，而不是降低工作成本。** 关闭某个特效的成本总是比优化它更低。Vite 的方法就证明了这一点：[`VITE_RT_PSO_DEBLOAT`](Compile-Time-Switches.md) 会移除未使用的光线追踪排列组合，而不是提升它们的速度。

**明确分配预算。** 在 60 fps 下，你有 16.6 毫秒的延迟。提前将这部分预算分配到游戏线程、渲染线程、GPU 和特定系统上，就能将“这是否太慢？”这个问题转化为一个有答案的问题。

## Vite 的性能优势源自何处？

需要明确的是，Vite 相对于 UE5 的优势并非仅仅体现在某一项优化上。它避免了多项性能退化，这些退化已在 [UE4 versus UE5 Cost Analysis](../EngineOverview/UE4-Versus-UE5-Cost-Analysis.md)中有所记录：更便宜的材质、[使用 PhysX 而非 Chaos](../Physics/PhysX.md)、更低的角色移动成本、更小的内存占用、更精简的 Slate 和 UI，以及更轻量的渲染线程。

在此基础上，Vite 还进行了针对性优化：[优化了 SSAO 路径](../Rendering/Ambient-Occlusion.md)、[精简了光线追踪 PSO](Compile-Time-Switches.md)、减少了引擎节拍运行时间，并[更改了默认设置](Engine-Defaults.md)。


!!! 笔记

    "Engine Tick Reductions"（引擎Tick削减）指的是通过减少或优化每帧执行的Tick（更新）操作，来降低CPU（尤其是游戏线程）的负载，从而提升游戏性能的一系列策略

这意味着，在您进行任何优化之前，大部分性能优势就已经唾手可得。您的任务主要是避免浪费这些优势。

## 另请参阅

- [性能目标](../EngineOverview/Performance-Targets.md)
- [UE4 与 UE5 成本分析](../EngineOverview/UE4-Versus-UE5-Cost-Analysis.md)
- [渲染](../Rendering/Rendering.md)
- [物理](../Physics/Physics.md)
