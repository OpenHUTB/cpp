# 破损与布料


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

Apex Cloth 是 PhysX 时代的布料系统，可以通过编辑器内的 Clothing Tool 编写，也可以从外部 DCC 工具生成的 APEX 布料资产导入。

| 细节 | |
|---|---|
| 资产类型 | 骨架网格物体上的 APEX 布料资源或编辑器内服装数据 |
| UE5中的状态 | 被移除以支持 Chaos Cloth |

编辑器内的工作流程将布料参数直接绘制到骨架网格物体上：最大距离、逆止半径和逆止距离。最大距离是主要控制 -它定义每个顶点可以从其蒙皮位置移动多远，因此将其绘制到零针区域并增加它可以让布料自由摆动。

实用注意事项：

- 布料成本随模拟顶点数变化。绘制低分辨率模拟网格并将渲染网格蒙皮到其上，而不是直接模拟渲染网格。
- 布料是根据骨架网格物体组件进行模拟的。布料上十个字符的成本是一个字符的十倍。
- 布料不会与场景的其余部分重叠。快速的角色运动可以产生拉伸，无需调整参数即可解决；答案通常是降低附着点移动的速度。

## NVIDIA Blast

Blast is NVIDIA's successor to Apex Destruction, and the more capable of the two fracture systems.

[Official Blast Plugin Documentation](https://archive.docs.nvidia.com/gameworks/content/gameworkslibrary/blast/1.1/authoring_docs/BlastUe4_QuickStart.html) 


| Detail | |
|---|---|
| Plugin | `Engine/Plugins/GameWorks/Blast` (version 1.0) |
| Platforms | Win64, Linux |
| Enabled by default | No |
| Modules | `BlastLoader`, `BlastRuntime`, `BlastEditor`, `BlastMeshEditor`, `BlastLoaderEditor` |

Blast separates the destruction graph from the physics simulation. A Blast asset describes chunks and the
bonds between them; damage propagates through the bond graph, and chunks become physics bodies only once
they actually detach. This is why Blast handles large structures better than Apex Destruction: an intact
building is a graph, not a thousand sleeping rigid bodies.

Blast also supports runtime fracture, so the fracture pattern can depend on where and how the object was
hit rather than being fully baked at author time.

Enable the plugin from **Edit > Plugins > GameWorks**, then use the Blast Mesh Editor to author assets.

> Blast is not a drop-in replacement for Apex Destruction. The asset types, authoring workflow and runtime
> API are all different. Choose one per project rather than mixing them, and prefer Blast for new work.
>
{style="note"}

## Choosing

| Need | System |
|---|---|
| Existing 4.27 destructible meshes | Apex Destruction &mdash; they still work, do not port them without reason |
| New destruction, large structures | Blast |
| New destruction, small props | Either. Apex Destruction is simpler; Blast scales better. |
| Runtime-dependent fracture patterns | Blast |
| Existing APEX clothing assets | Apex Cloth |
| New clothing | Apex Cloth via the in-editor Clothing Tool |

## Performance

Destruction is one of the easiest ways to destroy a frame budget, because the cost is invisible until
something breaks and then arrives all at once.

- **Cap simultaneous debris.** Set debris timeouts and maximum separation distances on every destructible.
- **Budget chunk counts against the worst case,** not the typical one. A grenade in a room full of
  destructibles is the frame you have to survive.
- **Consider the [instanced physics subsystem](Instanced-Physics.md)** for debris. Chunks that have settled
  and no longer need individual actors can be handled far more cheaply as instances.
- **Profile with `stat physics`** while destroying things, not while standing still.

## Licensing

Apex Destruction, Apex Cloth and Blast are NVIDIA GameWorks technologies inherited through the PhysX and
NvRTX lineage. Their licence terms apply to shipped titles. See the
[GameWorks source SDK EULA](https://developer.nvidia.com/gameworks-source-sdk-eula).

## See also

- [PhysX](PhysX.md)
- [Instanced Physics Subsystem](Instanced-Physics.md)
- [Bundled Plugins](Bundled-Plugins.md)
- [Profiling](Profiling.md)
