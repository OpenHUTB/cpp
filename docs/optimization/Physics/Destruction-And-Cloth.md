# 破坏与布料


Vite 保留了虚幻引擎 5 中删除的 <b>Apex 破损</b> 和 <b>Apex 布料</b>，并添加了 NVIDIA <b>Blast</b> 以实现现代断裂。现有的 4.27 可破损资产和布料资产继续发挥作用。


破损和布料是 [PhysX 决策](PhysX.md) 对内容影响最明显的两个领域。两个 Apex 系统都与 PhysX 绑定；删除 PhysX 就删除了它们。

## Apex 破损

Apex 破损将网格分解成层次结构的块，这些块在冲击下会破裂。资产被创作为可破损的网格，可以在编辑器中通过 Voronoi 分解进行破碎，也可以从外部工具导入。

| 细节 | |
|---|---|
| 插件 | [Engine/Plugins/Runtime/ApexDestruction](https://github.com/OpenHUTB/engine/tree/hutb/Engine/Plugins/Runtime/ApexDestruction) |
| 资产类型 | 可破碎网格 |
| 组件 | [UDestructibleComponent](https://github.com/OpenHUTB/engine/blob/8a229f05c4dca7b900cfe5867a6cb477d23d0670/Engine/Plugins/Runtime/ApexDestruction/Source/ApexDestruction/Private/DestructibleComponent.cpp#L34) |
| UE5中的状态 | 已删除 |

它是一个成熟、易于理解的系统，具有简单的成本模型：块数驱动一切。具有两级层次结构和适度块数的很便宜的可破坏物；一个具有很深层次结构和数百个块的系统则不然。

可破坏网格体资源上的关键设置：

- **深度** 控制存在多少破损级别。每个级别都会增加块数。
- **伤害阈值** and **伤害扩散** 控制块破裂的容易程度以及损坏传播的距离。
- **支撑深度** 确定哪个级别参与结构支撑计算。
- **碎片超时** 和 **碎片最大分离度** 确定后清理大块。设置这些。永不消失的块会不断累积，直到帧时间崩溃。

Apex 破损教学视频 [![MeanLemur 制作的教学视频](https://img.youtube.com/vi/Stn7eL1TFBg/hqdefault.jpg)](https://youtu.be/Stn7eL1TFBg)

## Apex 布料

Apex 布料是 PhysX 时代的布料系统（先模拟，后蒙皮），可以通过编辑器内的 Clothing Tool 编写，也可以从外部 DCC 工具生成的 APEX 布料资产导入。

| 细节 | |
|---|---|
| 资产类型 | 骨架网格物体上的 APEX 布料资源或编辑器内服装数据 |
| UE5 中的状态 | 被移除以支持 Chaos 布料 |

编辑器内的工作流程将布料参数直接绘制到骨架网格物体上：最大距离、逆止半径和逆止距离。最大距离是主要控制 -它定义每个顶点可以从其蒙皮位置移动多远，因此将其绘制到零针区域并增加它可以让布料自由摆动。

实用注意事项：

- 布料成本随模拟顶点数变化。绘制低分辨率模拟网格并将渲染网格蒙皮到其上，而不是直接模拟渲染网格。
- 布料是根据骨架网格物体组件进行模拟的。布料上十个角色的成本是一个角色的十倍。
- 布料不会与场景的其余部分重叠。快速的角色运动可以产生拉伸，无需调整参数即可解决；答案通常是降低附着点移动的速度。

## NVIDIA Blast

Blast 是 NVIDIA 推出的 Apex Destruction 的继任者，也是两款破碎系统中功能更强大的一款。

[Blast 插件官方文档](https://archive.docs.nvidia.com/gameworks/content/gameworkslibrary/blast/1.1/authoring_docs/BlastUe4_QuickStart.html)，[github 仓库地址](https://github.com/NVIDIAGameWorks/Blast)。


| 条目 | 细节 |
|---|---|
| 插件 | `Engine/Plugins/GameWorks/Blast` (1.0版) |
| 平台 | Win64, Linux |
| 默认启用 | No |
| 模块 | `BlastLoader`, `BlastRuntime`, `BlastEditor`, `BlastMeshEditor`, `BlastLoaderEditor` |

Blast 将破坏图与物理模拟分离。Blast 资源描述了块体及其之间的连接；损伤沿着连接图传播，块体只有在实际脱离后才会成为物理实体。这就是为什么 Blast 比 Apex Destruction 更能处理大型结构：一个完整​​的建筑物是一个图，而不是成千上万个静止的刚体。

Blast 还支持运行时破碎，因此破碎模式取决于物体被击中的位置和方式，而不是在创建时完全烘焙。

从**Edit > Plugins > GameWorks**启用插件，然后使用 Blast 网格编辑器创建资源。

!!! 注意

    Blast 不能直接替代 Apex Destruction。资源类型、创建工作流程和运行时 API 都不同。建议每个项目选择一个，而不是混合使用，并且在新项目中优先选择 Blast。


## 选择

| 需求 | 系统 |
|---|---|
| 现有 4.27 可破坏网格 | Apex Destruction — 它们仍然有效，请勿无故移植 |
| 新增破坏效果，大型结构 | Blast |
| 新增破坏效果，小型道具 | 两者皆可。Apex Destruction 更简单；Blast 的扩展性更好 |
| 运行时相关的破碎模式 | Blast |
| 现有 APEX 布料资产 | Apex 布料 |
| 新增布料 | 通过编辑器内的布料工具使用 Apex Cloth |

## 表现

破坏是消耗帧预算最简单的方法之一，因为只有在物体破碎时才会产生成本，而且成本会瞬间显现。

- **限制同时出现的碎片数量。** 为每个可破坏物设置碎片超时时间和最大分离距离。
- **预算块的计算应基于最坏情况，** 而非典型情况。例如，在满是可破坏物的房间里，一颗手榴弹就是你必须存活下来的帧。
- **考虑使用[实例化的物理子系统](Instanced-Physics.md)** 来处理碎片。已经稳定下来且不再需要单独 Actor 的碎片块可以作为实例进行处理，这样成本会低得多。
- 在破坏物体时**使用`stat physics`进行性能分析** ，而不是在静止不动时进行分析。

## 许可

Apex Destruction、Apex Cloth 和 Blast 是 NVIDIA GameWorks 技术，源自 PhysX 和 NvRTX 系列。其许可条款适用于已发售的游戏。请参阅 [GameWorks 源代码 SDK 最终用户许可协议 (EULA)](https://developer.nvidia.com/gameworks-source-sdk-eula)。

## 另请参阅

- [PhysX](PhysX.md)
- [实例化物理子系统](Instanced-Physics.md)
- [已捆绑的插件](../Plugins/Bundled-Plugins.md)
- [性能分析](../Performance/Profiling.md)
