# 引擎概述

Vite 是基于虚幻引擎 4.27.2 (`++UE4+Release-4.27`) 的分支版本，它完全融合了 NvRTX 4.27 的焦散效果，并整合了 NvRTX 5.0 和 AMD 分支的特性，此外还移植了 300 多个来自 UE 5.0 至 5.8 的组件。

本节将解释 Vite 架构背后的原因：为什么该分支版本基于虚幻引擎 4.27 而非 UE5，它的设计性能范围，以及两个引擎版本之间实际成本差异的来源。

如果您只想运行引擎，请前往[入门指南](../GettingStarted/Getting-Started.md)部分。


## 基础版本

| 属性           | 值                                                |
|--------------------|------------------------------------------------------|
| 引擎版本     | 4.27.2                                               |
| 仓库名称          | `UnrealEngineVite-PhysX`                             |
| 上游基础版本      | NvRTX 4.27 Caustics（焦散）                                  |
| 已合并的分支    | UE 4.27 Plus, NvRTX 5.0, [AMD GPUOpen 引擎分支](https://github.com/GPUOpenSoftware/UnrealEngine/tree/EngineOpt-4.24) |
| UE5 向后移植      | 发布版本超过 300 个，内部测试版本超过 1000 个          |
| 物理后端    | Vite PhysX                                           |
| 主路径渲染器 | 延迟的、与技术无关的DXR光线追踪管线          |

## 章节内容

### [为何选择 NvRTX 4.27](Why-NvRTX-427.md)

基础版本的技术论证：UE 5.1 光线追踪场景构建的改变、Lumen 耦合为何会使其他全局光照集成更加困难，以及移除 PhysX 为何至关重要。

### [性能目标](Performance-Targets.md)

Vite 针对四种 PS5 级配置进行了优化，从 4K120p 风格化到 1440p30（开启完整光线追踪效果套件）。

### [UE4 与 UE5 成本分析](UE4-Versus-UE5-Cost-Analysis.md)

帧时间实际消耗在哪里：着色器指令数、物理效果、角色移动、内存、Slate、骨骼网格、tick 成本、渲染线程开销、蓝图原生化和体积渲染。


### [发布说明](Release-Notes.md)

当前版本分支包含哪些内容、正在进行哪些开发以及未来计划包含哪些内容。

## 设计原则

代码库中几乎所有决策都受到三项承诺的影响，值得明确阐述，因为它们解释了那些看似保守的选择。


**久经考验的技术优于自研技术。** Epic Games 为 UE5 构建了一套全新的系统，而 Vite 则更倾向于采用行业标准解决方案，这些方案广泛应用于 AAA 级游戏中：PhysX 而非 Chaos，DDGI 而非 Lumen，硬件曲面细分而非 Nanite，TressFX 而非 Groom。这些选择并非出于怀旧，而是基于对已在众多游戏中得到验证的解决方案的考量。

**原生分辨率优于重建。** Vite 的性能目标设定为原生 4K 和原生 1440p。虽然支持并集成了升频器（Upscalers），但升频器被视为超越目标速度的手段，而非达到目标的途径。

**帧时间预算是设计约束，而非优化阶段。** 性能目标决定了产品的最终功能集和用户体验质量。 Vite 为其引入的每个主要功能都制定了迭代优化计划，而不是将优化推迟到项目后期。

[引擎编码指南](../Contributing/Coding-Guidelines.md)将这些原则转化为具体的规则，供贡献者参考：禁止递归、禁止无理由创建新的虚函数、禁止破坏 ABI 的更改，以及严格遵守 Clang 规范，并以 ARM 级 CPU 为基准性能目标。


## 另请参阅

- [Vite 简介](../GettingStarted/Introduction-to-Vite.md)
- [渲染](../Rendering/Rendering.md)
- [物理](../Physics/Physics.md)
- [引擎编码指南](../Contributing/Coding-Guidelines.md)
