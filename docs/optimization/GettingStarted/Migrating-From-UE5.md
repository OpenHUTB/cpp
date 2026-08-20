# 从虚幻引擎 5 迁移

<tldr>
<p>
使用 <a href="Propose-Plugins.md">UE Downgrader</a> 插件将资源向下移动，该插件支持 UE 5.8 及更低版本，直至回到 4.27。代码移动比原生 4.27 更容易，因为 Vite 向后移植了许多 UE5 容器、游戏框架和 GAS API。 4.27 中不存在的功能（Nanite、Lumen、VSM、World Partition）没有等效项，需要设计决策，而不是转换。
</p>
</tldr>

将项目从虚幻引擎 5 迁移到 Vite 是真正的迁移，而不是版本升级。本页列出了哪些内容可以干净地迁移，哪些内容需要替换，以及按什么顺序进行。

## 什么迁移了，什么没有迁移

| UE5 特性 | Vite 中的状态 | 该怎么办 |
|---|----------------|---|
| 静态网格物体、纹理、材质、动画 | 迁移      | 使用 UE Downgrader 插件降级 |
| 蓝图 | 迁移      | 降级；逐个节点重新测试，某些仅 UE5 的节点没有目标 |
| C++ 游戏代码 | 迁移      | Vite 向后移植了许多 UE5 API；见下文 |
| 游戏能力系统 | 迁移      | Vite 包括从 UE5 反向移植的 GAS 更新 |
| Nanite | 没有等同的  | 编写常规 LOD；使用 [Tessellation](Tessellation.md) 获取表面细节 |
| 流明 | 替换       | 使用 [DDGI](DDGI-Dynamic.md)，可选择与 [SSGI](SSGI.md) 一起使用 |
| 虚拟阴影贴图 | 没有等同的  | 级联阴影贴图，或[光线追踪阴影](RT-Shadows-And-Ambient-Occlusion.md) |
| TSR | 替换       | [DLSS, FSR, XeSS](Upscalers.md), or native with [SMAA](Anti-Aliasing.md) |
| MegaLights | 替换       | [RTXDI](RTXDI.md) |
| Chaos physics | 替换       | [PhysX](PhysX.md) |
| Chaos Destruction | 替换       | [Apex Destruction](Destruction-And-Cloth.md) 和 [Blast](Destruction-And-Cloth.md) |
| Chaos Cloth | 替换       | [Apex Cloth](Destruction-And-Cloth.md) |
| Substrate | 没有等同的  | 标准材质模型，加上 [Callisto BRDF](Shading-Models.md) |
| 世界分区 | 替换       | 世界构成和关卡流 |
| Niagara | 迁移      | 可用的; [PopcornFX](Propose-Plugins.md) 是更快的替代方案 |
| MetaSounds | 没有等同的  | 声音提示，或 [Wwise](Proproped-Plugins.md) |

## 资产降级

UE Downgrader 插件可将 UE 5.8 及更低版本的资源转换回 4.27 和 4.26。它的工作原理是首先将资源升级到其源版本（通常是最新的 UE5），然后对“.uasset”文件应用补丁，以便目标版本可以读取它们，减去旧格式无法表示的数据。例如，Nanite 数据在 4.27 降级期间被删除。

该插件无法做的是为 4.27 中不存在的功能发明等效项。蒙版材料、新材料节点和新 Niagara 模块上的 Nanite 将无法在旅途中幸存。需要为重新创作这些内容的指定预算。

该插件是商业的。其作者 Ciprian Stanciu 活跃于 [Vite Discord](https://discord.gg/n9zQrYFhMb)，并为多个 Vite 项目提供了直接支持。有一个工作流程的[视频演练](https://youtu.be/yXvJfDNfrSQ)。

## 迁移代码

这是 Vite 上比 Stock 4.27 更容易的部分。该分支故意向后移植 UE5 API 表面，以便 UE5 时代的代码编译时只需较少的更改：更新的容器类、游戏框架更新和游戏能力系统功能都存在。

你仍然会遇到分歧。按以下顺序完成它们：

1. **物理.** 任何涉及“Chaos”命名空间、“FCaosScene”、几何集合或 Chaos 载具的内容都需要转移到 PhysX 等效项。这通常是最大的单个工作块。请参阅 [PhysX 概述](PhysX.md)。
2. **渲染.** 查询或驱动 Lumen、Nanite 或 VSM 控制台变量的代码没有目标。替换为 [Rendering](Rendering.md) 中的 Vite 等效项。
3. **核心 API 漂移.** `FVector` 在 UE5 (`FVector3d`) 中是双精度，在 4.27 中是单精度。大世界坐标假设不成立。需要审核在大世界偏移量下执行精度敏感数学的任何代码。
4. **模块和构建规则.** 引用仅限 UE5 的模块的 .build.cs 文件需要删除或替换这些依赖项。

## 推荐的迁移顺序

### 将 UE5 项目迁移到 Vite

1. 首先创建一个空的 Vite 项目，并确认你的工具链和引擎构建正常。不要同时调试两个问题。

2. 移植不包含任何内容的 C++ 模块。在迁移任何资源之前，确保它们能够针对 Vite 进行编译。在此处检查物理和渲染 API 是否存在兼容性问题。

3. 降级并导入一个具有代表性的小型内容片段——一个角色、一个场景和少量材质。验证其外观和行为是否正确。

4. 替换光照设置。Lumen 没有直接的转换；设置 [DDGI](../Rendering/DDGI-Dynamic.md) 体积并根据刚刚导入的内容片段进行调整。

5. 重建物理设置。使用 [Apex Destruction](../Physics/Destruction-And-Cloth.md) 重新制作物体破损效果，使用 [Apex Cloth](../Physics/Destruction-And-Cloth.md) 重新制作布料效果。

6. 验证内容片段后，批量迁移剩余内容。

7. 针对目标硬件进行性能分析并设置可扩展性。请参阅[性能分析和基准测试](../Performance/Profiling.md)。


## 混合版本工作流程


您不必立即移动所有人。一种常见的安排是程序员在 Vite 中工作，而美术师和内容创作者则继续使用 Epic Games Launcher 4.27 的库存安装，内容以单向流动。为此，启动器端用户需要独立的 [DDGI 1.1.5 插件](https://github.com/GapingPixel/UE4-RTXGI-1.1.5-Latest-Official)，以便照明看起来大致正确。这正是我们在 Vite Studio 所做的，到目前为止，建模、动画和声音设计工作没有出现任何问题。

!!! 警告

    不要在 Vite 项目中使用启动器 DDGI 插件。 Vite 已经将 DDGI 作为引擎的一部分提供，两者会发生冲突。 Vite 中的 DDGI 比 Launcher 版本的 DDGI 插件处理得更好。 （由于引擎端/DDGI插件端均发生变化）


## 另请参阅

- [Vite 简介](Introduction-to-Vite.md)
- [PhysX 概述](PhysX.md)
- [全局照明](Global-Illumination.md)
- [建议的插件](Proposed-Plugins.md)
- [UE4 与 UE5 成本分析](UE4-Versus-UE5-Cost-Analysis.md)
