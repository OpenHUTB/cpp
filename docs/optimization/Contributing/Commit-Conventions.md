# 提交约定

在每个提交主题前添加一个或多个类别标签。标签的排列顺序应从宽泛到具体。向后移植和插件添加必须链接到原始提交或仓库。

跟踪上游引擎的分支会积累来自多个来源的大量变更历史记录。能够扫描这些历史记录，区分优化、渲染功能和向后移植，是未来合并易于管理的关键。


## 前缀

在提交主题的开头使用以下前缀之一。

| 前缀 | 用途 |
|---|---|
| `[Rendering]` | 渲染器相关工作 — 标签树中最常用的标签 |
| `[Rendering Feature]` | 新增或变更的渲染功能 |
| `[RT]` | 光线追踪 |
| `[Forward]` | 前向渲染器路径 |
| `[Optimization]` | 性能优化，不改变引擎行为 |
| `[Backport]` | 从后续引擎版本中移植的更改 |
| `[Animation]` | 动画系统 |
| `[GameplayFramework]` | 相机、角色移动、AI 以及其他游戏框架组件 |
| `[Memory]` | UObject 内存子系统 |
| `[DOP]` | 面向数据的工作，包括 ECS |
| `[PhysX]` | 物理引擎 |
| `[Plugin]` | 添加、更新或移除插件 |
| `[Lib]` | 第三方库更新 |
| `[Toolchain]` | 编译器、SDK 和构建环境 |
| `[Defaults]` | 对已发布引擎的更改默认设置 |
| `[Debloat]` | 移除或限制默认配置中的某些功能或配置成本 |
| `[Mobile]` | 移动端专属功能 |
| `[VR]` | VR 专属功能 |
| `[AMD]` | AMD 专属功能 |
| `[NVIDIA]` | NVIDIA 专属功能 |
| `[Fix]` | 错误修复 |

### 堆叠前缀

大多数重要的变更都会带有多个标签。请按照从宽泛到具体的顺序排列这些标签，并将向后移植标记放在最后，以便变更的来源显示在末尾：

```
[Rendering][RT][Optimization][Adapted Backport 5.3] Exclude raygen shaders from RTPSOs
unless the corresponding feature is enabled
[Rendering][Forward][Backport 5.6] Fix incorrect alpha from MSAA resolve with explicit fmask
[Rendering][Shading Models] Remove unnecessary branch for the Toon shading model
```

使用 `Adapted Backport` 而不是 `Backport` 表示上游更改未能正确应用，而是针对 Vite 代码库进行了重写。这一点很重要，因为之后有人需要判断上游对原始提交的修复是否也适用于此处。

当更改确实只涉及一项内容时，使用单标签提交是可以接受的：

```
[Toolchain] Compiling on VS 18.8.2 (_MSC_VER 1950) and Windows SDK 10.0.26100
[Plugin] PhysX Blast + engine-side changes for proper support
[Gating] RT translucency guard
[Fix] Guard RTXDI CVar behind ShouldRenderRayTracingSampledLighting
```

如果一项更改涉及完全不相关的领域，请将其拆分，而不是将表格中的所有标签都堆到一个提交中。


## 归属信息

**警告：** 在进行向后移植或添加插件时，请在提交正文中包含指向原始提交或仓库的正确链接。

这并非出于礼貌，而是一项实际要求。如果没有链接，任何人都无法确定集成的是哪个版本，上游是否已修复其中的错误，或者许可证是否允许重新分发。

```
[Plugin] Add Kawaii Physics 1.18.0, backported from UE5

Source: https://github.com/pafuhana1213/KawaiiPhysics
Upstream tag: v1.18.0
Changes: UE5 API calls replaced with 4.27 equivalents in
KawaiiPhysicsEditMode.cpp and AnimNode_KawaiiPhysics.cpp
```

对于单个上游提交的反向移植，请链接提交本身，而不仅仅是代码仓库。请参阅[反向移植](Backporting.md)部分。


## 提交正文

主题行说明更改的内容。正文应说明更改的原因以及审阅者需要了解的信息：

- 更改解决的问题
- 任何声称是优化的更改，都应说明其影响程度
- 任何添加的控制台变量或[编译时开关](Compile-Time-Switches.md)及其默认值
- 任何故意未完成的操作

影响程度至关重要。以`[Optimization]`为前缀且没有具体数值的提交仅是一种断言，而非结果。请参阅[性能分析](../Performance/Profiling.md)部分，了解如何生成可信的更改前后对比。


## 分支

| 规则 | |
|---|---|
| 未完成的工作应放在单独的分支上 | 不要将未完成的工作推送到共享分支 |
| 记录剩余工作 | 在分支或提交正文中添加简短注释 |
| 删除临时分支 | 一旦合并或放弃 |
| 告知其他 fork 用户 | 以免浪费并行工作 |

## 控制台变量和开关

新的控制台变量遵循引擎现有的命名规则。Vite 特有的变量以子系统名称为前缀，后跟 `Vite`：

| 示例 | 子系统 |
|---|---|
| `r.Vite.SMAA.Mode` | 渲染器 |
| `r.Vite.SSAO` | 渲染器 |
| `p.VitePhysXFixedTimestep.Enabled` | 物理引擎 |

编译时开关使用 `VITE_` 前缀，并且必须在 `Engine\Source\Runtime\Core\Public\Misc\CoreDefines.h` 中定义默认值：

```c++
#ifndef VITE_MY_FEATURE
	#define VITE_MY_FEATURE 0
#endif
```

在同一变更中，请将任何新的开关记录在[编译时开关](../Performance/Compile-Time-Switches.md)部分。未记录的开关可能会被调试控制台变量的用户发现，而该变量实际上没有任何作用——这正是 `VITE_RT_PSO_DEBLOAT` 之前导致的故障模式。


## 另请参阅

- [贡献](Contributing.md)
- [编码指南](Coding-Guidelines.md)
- [向后移植](Backporting.md)
- [编译时开关](../Performance/Compile-Time-Switches.md)
