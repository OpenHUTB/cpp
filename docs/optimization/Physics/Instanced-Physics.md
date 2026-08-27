# 实例化物理子系统

Vite 插件，它为实例化静态网格组件的每个实例赋予独立的 PhysX 刚体，并通过稳定的数值控制柄进行控制。该插件专为模拟数千个物体而设计，避免了为每个物体使用一个 Actor 所带来的成本负担。


在虚幻引擎中模拟多个物体的标准方法是为每个物体使用一个 Actor，每个 Actor 包含一个基本组件和一个刚体实例。这种方法在处理几百个物体时效果不错，但之后就会出现问题：Actor 的开销、组件的触发、变换传播和场景图更新的成本甚至超过了物理本身的计算量。

PhysX 实例化子系统将渲染和物理计算分离。渲染通过单个实例化静态网格组件进行；物理计算则通过子系统直接拥有的 PhysX 刚体进行。没有每个物体的 Actor，也没有每个物体的组件。


| 详情 | |
|---|---|
| 插件 | `Engine/Plugins/Runtime/VitePlugins/PhysXInstancedSubsystem` |
| 模块 | `PhysXInstancedSubsystem` |
| 子系统 | `UPhysXInstancedWorldSubsystem`（可更新的世界子系统） |
| 参与者 | `APhysXInstancedMeshActor` |
| 组件 | `UPhysXInstancedStaticMeshComponent` |
| 作者 | NordVader Inc. |

请参阅 [PhysX 实例子系统演示](../ProjectsAndDemos/PhysX-Instanced-Subsystem.md)以获取可运行的项目。


## 核心概念

**实例通过句柄而非指针进行寻址。** `FPhysXInstanceID` 封装了一个 `uint32` 类型，其中 `0` 表示无效。即使实例在不同 Actor 之间移动，句柄也保持稳定，这一点至关重要，因为实例被移除时 ISM 实例索引会发生改变。游戏代码持有句柄；子系统维护从句柄到组件和索引的映射关系。

**动态实例和存储实例是同一实例的不同状态。** 动态实例拥有 PhysX 实体并进行模拟。存储实例存在于一个独立的存储 Actor 中，完全没有 PhysX 实体。两者之间的转换会保留句柄。这是核心的性能机制：静止的碎片会转换为存储，无需模拟即可保持可见。

**工作量按帧分配。** 创建 PhysX 物体、施加力和使物体寿命到期等操作都会排队并根据每帧的限制进行处理，因此在一帧中生成一万个实例不会产生一万个物体的卡顿。

## 设置参与者

将`APhysXInstancedMeshActor`放置在关卡中，并在 **Phys X 实例** 下的详细信息面板中配置它。

### 配置实例化物理参与者

1. 设置**静态网格（Static Mesh）**，并根据需要覆盖材质。

2. 选择**生成模式（Spawn Mode）**。**手动** 使用 **Instance Relative Transforms** 数组；**网格（Grid）** 根据间距设置生成“行×列×层”的网格，方便基准测试场景。

3. 设置**实例形状类型（Instance Shape Type）**。包围盒、球体和胶囊最便宜；**凸面网格（Convex Mesh）**准确且成本适中；**三角形网格（Triangle Mesh）**仅为静态或动态网格。

4. 设置 **模拟实例（Simulate Instances）** 和 **实例使用重力（Instances Use Gravity）**，如果对象的默认值错误，则设置质量替代和阻尼。

5. 使**禁用 ISM 物理（Disable ISM Physics）** 处于启用状态，以便渲染组件不会在子系统的实体旁边创建自己的碰撞。

6. 启用**在开始播放时自动注册（Auto Register On Begin Play）**，或调用 `BuildAndRegisterInstances`，这也可以从编辑器中调用。

如果没有为凸面或三角形指定碰撞网格，则使用渲染网格。质量是根据碰撞网格上物理材质的密度计算的，从 g/cm³ 转换为 kg/m³，并按组件的质量按比例进行缩放。


## 自动停止

自动停止是使大规模模拟承担得起的因素。它检测有效停止移动的实例，并对它们执行消耗更小的操作。

在 **Phys X实例（Phys X Instance）>运行时（Runtime）>自动停止配置（Auto Stop Config）** 中配置：

| 设置 | 默认 | 用途 |
|---|---|---|
| 启用自动停止 | `false` | 主开关 |
| 条件 | PhysX 休眠标志 | “停止”的判定方式 |
| 线速度阈值 | `5.0` 厘米/秒 | 用于基于速度的条件 |
| 角速度阈值 | `5.0` 度/秒 | 用于基于速度的条件 |
| 最小停止时间 | `0.5` 秒 | 条件在执行前必须保持的时间 |
| 动作 | 销毁刚体 | 触发时发生的情况 |

条件**仅限于 PhysX 睡眠标志**、**仅限速度阈值**、**睡眠或速度**，或**睡眠和速度**。睡眠标志开销最小且通常足够；速度阈值可以捕捉那些漂移足够慢以至于视觉上已经停止但尚未进入睡眠的物体。

动作，按攻击性递增顺序：

| 动作 | 效果 |
|---|---|
| 不执行任何动作 | 仅跟踪状态 |
| 禁用模拟（保留实体） | 实体保持，但停止模拟 |
| 销毁实体（保留实例） | PhysX 实体释放，视觉实例保持在原位 |
| 销毁实体并移除实例 | 二者均释放。会改变 ISM 索引 — 使用句柄，不要使用索引。 |
| 转换为存储 | 实例移至存储 Actor，实体释放，句柄保留 |

**转换为存储**通常是保留可见碎片的正确选择。**销毁主体**则适用于希望对象保留但永不再移动的情况。


### 安全规则

另外两条规则用于捕捉那些永远不会自行停止的实例：

- **最大下落时间（Max Fall Time）** 在实例连续下落超过阈值时间后触发停止动作。用于捕捉穿透世界几何体而下落的物体。
- **与参与者最大距离（Max Distance From Actor）** 在实例距离其所有者超过阈值时触发。用于捕捉由于不良冲力而被发射的物体。

另外，**自定义 Kill Z（Use Custom Kill Z）** 配合 **自定义 Kill Z** 高度和 **丢失实例操作（Lost Instance Action）** 处理那些下落到世界 Z 阈值以下的实例。


## 生命周期

实例可以在定时器上到期。可以在 Actor 上设置**启用生命周期（Enable Lifetime）**、**默认生命周期秒数（Default Life Time Seconds）**和**默认生命周期动作（Default Lifetime Action）**，或者通过生成请求按生成覆盖这些设置。

到期事件存储在以到期时间为键的最小堆中，因此每帧的开销与实际到期的实例数量成正比，而不是与实例总数成正比。


## 连续碰撞检测

连续碰撞检测（Continuous collision detection, CCD）可以防止高速物体穿透薄几何体，但它并非无开销。CCD 配置提供四种模式：


| 模式 | 行为 |
|---|---|
| 关闭（Off） | 从不 |
| 仅模拟实体 | 仅实际动态的物体 |
| 自动（按速度） | 在超过 **最小 CCD 速度** 时启用（默认2000厘米/秒），可选择由最大CCD速度限制 |
| 所有实体 | 始终 |

按速度自动选择是碎片的正确默认设置：移动缓慢的碎片完全跳过 CCD，只有快速的碎片才需要处理。

## 运行时 API

所有内容都可以通过蓝图调用。从世界中获取子系统，然后通过句柄进行操作。

### 生成

```c++
FPhysXSpawnInstanceRequest Request;
Request.ActorMode = EPhysXInstanceActorMode::FindOrCreateByMeshAndMats;
Request.StaticMesh = DebrisMesh;
Request.InstanceWorldTransform = SpawnTransform;
Request.bStartSimulating = true;
Request.InitialLinearVelocity = LaunchVelocity;
Request.bOverrideLifetime = true;
Request.LifeTimeSeconds = 15.0f;
Request.LifetimeAction = EPhysXInstanceStopAction::ConvertToStorage;

const FPhysXSpawnInstanceResult Result = Subsystem->SpawnPhysicsInstance(Request);
```

**参与者模式（Actor Mode）** 决定实例的落点：`AlwaysCreateNew` 会生成一个新的参与者，`FindOrCreateByMeshAndMats` 会重用具有匹配网格和材质的现有参与者，而 `UseExplicitActor` 则针对您提供的参与者。通常选择 `FindOrCreateByMeshAndMats` —— 它将实例数量尽可能集中在少数 ISM 组件中，从而提高渲染效率。

对于已存在于组件上的大量实例的批量注册，`RegisterInstancesBatch` 会将工作分散到多个帧中进行。


### 力和查询

力和冲量函数有普通版和高级版。高级版本具有 `bIncludeStorage` 和 `bConvertStorageToDynamic` 参数，这些参数控制调用是否会将存储实例唤醒回动态物体——这就是已静止的碎片场对后续爆炸的反应方式。

```c++
Subsystem->AddRadialImpulse(
    ExplosionOrigin, /*Radius=*/500.0f, /*Strength=*/80000.0f,
    /*bVelChange=*/false,
    /*bIncludeStorage=*/true,
    /*bConvertStorageToDynamic=*/true,
    /*bLinearFalloff=*/true);
```

空间查询返回的是句柄而非碰撞结果：`RaycastInstanceID`、`SweepSphereInstanceID`、`OverlapSphereInstanceIDs`、`FindNearestInstance` 和 `FindNearestInstanceAdvanced`。每个方法都可以选择可选的调试模式（`None`、`Basic`、`Detailed`）和绘制持续时间，这是理解查询未命中预期目标的最快方式。


### 事件

`APhysXInstancedMeshActor` 暴露了六个多播委托：`OnInstancePreRemove`、`OnInstancePostRemove`、`OnInstancePreConvert`、`OnInstancePostConvert`、`OnInstancePrePhysics` 和 `OnInstancePostPhysics`.

事件由 Actor 的 **实例事件掩码（Instance Event Mask）** 位掩码控制。除非你确实绑定了事件，否则请将掩码留空，因为在数千个实例中向无效目标广播是浪费工作。移除和转换事件会携带一个原因（`Explicit`、`Expired`、`AutoStop`、`KillZ`、`Lost`）以及事件发生时的世界变换，这正是你生成粒子效果或在碎片沉降时播放声音所需的信息。


## 性能调优

| 设置 | 默认值 | 效果 |
|---|---|---|
| 每帧最大添加参与者数 `MaxAddActorsPerFrame` | `64` | 每帧添加到 PhysX 场景中的实体数量。`0` 表示无限制。 |
| 每帧最大实例任务数 `MaxInstanceTasksPerFrame` | `4096` | 每帧排队的力/冲击/睡眠/唤醒操作数量。`0` 表示无限制。 |
| 每节拍最大生命周期到期处理数量 `MaxLifetimeExpirationsPerTick` | `4096` | 每节拍处理的生命周期到期数量。`0` 表示无限制。 |

这三者都是子系统的配置属性，因此可以在 `DefaultGame.ini` 中设置，并可针对每个平台进行调整。

首先应调整的是 `MaxAddActorsPerFrame`。向场景中添加一个 PhysX 物体开销不小，而一阵突然的生成最可能导致卡顿。降低该值可以分散开销；提高该值则可以减少新生成碎片开始移动之前的延迟。

子系统会尽可能并行评估实例。工作线程通过复制的快照读取 PhysX 状态和物体配置，从不直接操作 UObject；游戏线程则将结果以按组件批量变换更新的方式应用回去。


## 指南

**使用此方法处理** 碎片、弹壳、脱离后的可破坏碎片、被撞倒的道具、散落的杂物，以及任何你希望有数百甚至数千个模拟对象的情况。

**不要将其用于** 需要自身游戏逻辑、组件、复制或蓝图行为的对象。那些是参与者（Actors），应保持为参与者。 

**注意** ISM索引的变化。移除一个可视实例会改变之后实例的索引。子系统会修复自身的映射，但你在自己代码中缓存的任何索引现在都是错误的。请缓存句柄。

## 另请参见

- [PhysX](PhysX.md)
- [PhysX 实例化子系统演示](../ProjectsAndDemos/PhysX-Instanced-Subsystem.md)
- [破坏与布料](./Destruction-And-Cloth.md)
- [物理立方体基准测试](../ProjectsAndDemos/Physics-Cube-Bench.md)
- [性能分析](../Performance/Profiling.md)
