# 400 角色 CMC 基准测试

<tldr>
<p>
400 simultaneously moving characters driven by the Character Movement Component. Measures the game thread
cost that dominates crowd-heavy scenes.
</p>
</tldr>

![](../../img/optimization/400CharCMCBench.png)

*并排统计数据对比，Vite 帧率为 73.4 FPS，而虚幻引擎 5.6 在 400 个角色的情况下帧率为 22.7 FPS。*

*同一硬件上运行相同的 400 角色场景。Vite（左）帧率为 73.4 FPS，虚幻引擎 5.6（右）帧率为 22.7 FPS。世界帧延迟分别为 11.56 毫秒和 40.45 毫秒；角色移动总延迟分别为 5.73 毫秒和 12.49 毫秒。*


## 下载

[400 Characters CMC Bench](https://drive.google.com/file/d/1RrOXCeJEhO4H2x1QK9FlBv0GDD8qSsN7/view)

## 测试内容

The Character Movement Component is one of the most expensive stock Unreal components per instance. Every moving character runs collision sweeps, floor checks, root motion evaluation and network-prediction bookkeeping every tick, on the game thread.

At one character it is invisible. At 400 it is often the entire frame budget, and it is the reason so many projects hit a game thread wall long before their GPU is saturated.

This benchmark exists to make that cost measurable rather than theoretical.

## 运行

Enable `stat unit` and watch **Game** rather than **Frame**. If Game is the largest number, you are
CPU-bound on the game thread and no amount of rendering optimisation will help.

Then narrow it down:

| Command | Shows |
|---|---|
| `stat game` | Game thread breakdown by category |
| `stat character` | Character movement specifics |
| `stat anim` | Animation evaluation cost, usually the second contributor |
| `stat physics` | Collision query cost from movement sweeps |

See [Profiling](Profiling.md).

## 这对 Vite 为什么重要

Game thread cost is where UE 4.27 has a structural advantage over UE5, and it is central to
[Vite's rationale](Why-NvRTX-427.md). The base cost of core classes, the tick pipeline and the movement
component are all lighter in 4.27, and Vite trims them further &mdash; see
[Engine Defaults](Engine-Defaults.md) for the specific changes.

The relevant comparison numbers are in
[UE4 versus UE5 Cost Analysis](UE4-Versus-UE5-Cost-Analysis.md).

## 在自己的项目中减少 CMC 消耗

If this benchmark's shape matches your bottleneck:

- **更少的节拍。** Distant or off-screen characters do not need full-rate movement. Significance Manager and
  tick interval both apply.
- **简化碰撞。** Movement sweeps cost in proportion to collision complexity.
- **Budget 动画。** The Animation Budget Allocator plugin is stock 4.27 and directly targets this.
- **考虑不使用 CMC.** For crowds that do not need networked prediction, a simpler custom movement
  path avoids most of the cost.
- **考虑 ECS.** For very large entity counts, the bundled [Flecs ECS](Bundled-Plugins.md) integration
  sidesteps actor-per-entity overhead entirely.

## 另请参阅

- [性能目标](Performance-Targets.md)
- [性能分析](Profiling.md)
- [UE4 和 UE5 的消耗对比分析](UE4-Versus-UE5-Cost-Analysis.md)
- [引擎默认](Engine-Defaults.md)
