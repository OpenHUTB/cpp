# 固定时间步长


可选的 Vite 功能，以独立于帧速率的固定速率步进 PhysX，并通过渲染插值使运动保持平滑。编译时开关<code>VITE_PHYSX_FIXED_TIMESTEP</code>（默认<code>0</code>），运行时CVar <code>p.VitePhysXFixedTimestep.Enabled</code>。


默认情况下，Unreal 使用帧的增量时间来步进物理。物理步长为 8 毫秒的帧，物理步长为 8 毫秒；帧的步长为 33 毫秒，步长为 33 毫秒。因此，模拟结果取决于帧速率，这意味着相同的输入在不同的硬件上会产生不同的结果，并且在一台机器上记录的重放不会在另一台机器上再现。

Vite 的固定时间步长模式将两者解耦。物理总是以相同的增量进步；渲染器在两个最近的物理状态之间进行插值以保持运动平滑。

## 启用

这是一个两阶段的选择加入。该功能是默认编译出来的。

1. 将 [VITE_PHYSX_FIXED_TIMESTEP](https://github.com/GapingPixel/UnrealEngineVite-PhysX/blob/ueVite26-JulyMajor-release/Engine/Source/Runtime/Core/Public/Misc/CoreDefines.h#L28) 设置为 <code>1</code>。请参阅<a href="../Performance/Compile-Time-Switches.md">编译时开关</a>，了解如何在不编辑 [CoreDefines.h](https://github.com/OpenHUTB/engine/tree/hutb/Engine/Source/Runtime/Core/Public/Misc/CoreDefines.h)（集中定义引擎核心层的一些全局宏常量和编译开关，供整个 Unreal/引擎核心代码在预处理阶段使用）的情况下覆盖它。

2. 重新构建引擎。

3. 在运行时或在 <code>DefaultEngine.ini</code> 中设置 <code>p.VitePhysXFixedTimestep.Enabled 1</code>。

4. 在<b>项目设置 > 引擎 > 物理</b>中启用子步进。固定步实现贯穿子步路径。


编译时门的存在是因为该功能改变了物理场景、子步骤任务和动画物理混合中的热路径。不需要确定性的项目不应为额外的分支和双缓冲转换存储付费。

## 控制台变量

所有这些仅在编译`VITE_PHYSX_FIXED_TIMESTEP`时才存在。

| CVar | 默认 | 范围 | 目的 |
|---|---|---|---|
| `p.VitePhysXFixedTimestep.Enabled` | `0` | 0/1 | Master 启用 |
| `p.VitePhysXFixedTimestep.DeltaTime` | `0.01667` (1/60) | 0.0013&ndash;1.0 | 修复模拟步骤（以秒为单位） |
| `p.VitePhysXFixedTimestep.MaxTimesteps` | `16` | 1&ndash;50 | 过载累积之前每个游戏节拍的最大固定步数 |
| `p.VitePhysXFixedTimestep.MaxCumulativeExtraSteps` | `50` | 0&ndash;100 | 限制策略实施前累计过载预算 |
| `p.VitePhysXFixedTimestep.LimitType` | `0` | 0/1 | `0` 限制固定步骤，`1` 回退到可变子步骤 |
| `p.VitePhysXFixedTimestep.InterpolationMode` | `1` | 0/1/2 | `0` 禁用, `1` 每个组件, `2` 总是 |

## 它是如何运作的

该实现维护一个时间累加器。每个游戏节拍都会将帧的增量添加到累加器中，然后运行尽可能多的完整固定步骤。余数保留在累加器中以供下一帧使用。

由于快速帧可能适合零个固定步长，而慢速帧可能适合多个固定步长，因此系统必须仔细处理两件事。

**缓冲区不会在没有物理步骤的帧上旋转。**标准子步骤无条件交换物理目标缓冲区。在固定步长模式下，交换会推迟到实际发生一步为止，这会保留在未步进的帧期间排队的一次性力、运动学目标和自定义物理回调。如果没有这个，施加在零步帧上的脉冲将被默默地丢弃。

**仅在发生步骤时才获取结果。**在没有模拟的帧上跳过`fetchResults`，避免冗余工作和过时的转换读取。

## 过载处理

如果游戏无法跟上——长时间的故障，或者物理负载对于固定速率来说太重——累加器的增长速度快于其消耗速度。如果放任不管，这将变成一个死亡螺旋：每一帧需要更长的时间，这会排队更多的步骤，这使得下一帧的时间更长。

有两个限制可以防止这种情况发生。 “MaxTimesteps”限制每个节拍的步数。超出该上限的过载会根据“MaxCumulativeExtraSteps”进行累积，当预算用完时，“LimitType”将决定会发生什么：

| `LimitType` | 行为 | 什么时候使用 |
|---|---|---|
| `0` &mdash; 夹钳 | 丢弃多余的累积时间。模拟落后于挂钟时间。 | 确定性比实时通信更重要：重播、确定性测试 |
| `1` &mdash; 可变子步骤 | 回退到正常的变步子步进，直到赶上。 | 实时通信更重要：一般 gameplay |

夹钳意味着物理在持续过载的情况下以慢动作运行，但对于给定的步骤序列保持位相同。退回到可变子步骤可以保持实时性，但会牺牲其所涉及的框架的确定性。

## 渲染插值

修复了在 120 fps 渲染时以 60 Hz 步进的问题，该问题会在没有插值的情况下显示每个物理状态两次，从而产生可见的抖动。系统存储每个实体最近完成的两个变换（`PreviousPhysicsTransform`和`LatestPhysicsTransform`），并使用累加器的小数余数在它们之间进行混合。

`InterpolationMode` 控制其应用范围：

- **`0` 禁用。** 没有插值。身体捕捉到最新的物理状态。最便宜，并且如果您的渲染速率与您的固定步长速率完全匹配，则更正。
- **`1` 每个组件（默认）。** 仅对设置了`bInterpolateWhenSubStepping`标志的实体进行插值。引擎会为无法观察到插值的实体（例如仅运动查询实体）清除此标志，因此此模式可以获得视觉优势，而无需为不需要的实体付出代价。
- **`2` 总是。** 无条件地插值每个实体。

插值适用于刚体和骨架网格物体物理混合，因此布娃娃（ragdolls）和物理混合动画保持平滑。

## 选择步进速率

`DeltaTime`是中央调整决策。

| 步速 | 数值 | 笔记 |
|---|---|---|
| 30&nbsp;Hz | `0.03333` | 最便宜的。可接受缓慢、重的物体。快速的物体会形成隧道。 |
| 60&nbsp;Hz | `0.01667` | 默认。不错的一般选择。 |
| 120&nbsp;Hz | `0.00833` | 更适合快速射弹和严格的约束。大约是 CPU 成本的两倍。 |

物理成本与步速成线性关系，因此 120 Hz 步进成本约为 60 Hz 的两倍。选择保持最快移动的模拟物体稳定的最低速率，并使用 CCD 而不是更高的步进速率来解决孤立的隧道问题。

!!! 注意
    尽早选择步进速率，并且在内容调整后不要更改它。物理内容——脉冲幅度、约束刚度、阻尼值——根据步长速率隐式调整。晚了改变它意味着重新调整一切。


## 何时使用这个

**使用固定时间步长**确定性重播、客户端和服务器必须同意的网络物理、断言物理结果的自动测试，以及相同输入必须在机器上产生相同结果的任何游戏。

**请勿将其用于**没有确定性要求的项目。默认的可变步路径更便宜，因此编译时开关默认为关闭。

## 另请参阅

- [PhysX](./PhysX.md)
- [编译时开关](../Performance/Compile-Time-Switches.md)
- [实例物理子系统](./Instanced-Physics.md)
- [性能分析](../Performance/Profiling.md)
