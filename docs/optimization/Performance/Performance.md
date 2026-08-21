# 性能和优化

<tldr>
<p>
Vite exists to hit native-resolution, high-frame-rate targets on console-class hardware. This section
covers how to measure whether you are hitting them, what Vite already changed on your behalf, and what
levers remain.
</p>
</tldr>

Performance in Vite is not a post-production activity. The [performance targets](Performance-Targets.md)
are the specification, and everything in this section exists to help you stay inside them.

## In this section

| Topic | Covers |
|---|---|
| [Profiling](Profiling.md) | Finding out where the frame actually goes |
| [Engine Default Changes](Engine-Defaults.md) | What Vite changed from stock 4.27, and why it affects your project |
| [Compile-Time Switches](Compile-Time-Switches.md) | The `VITE_*` macros, including the one that removes ray tracing effects |
| [Shader Compilation and PSO](Shader-Compilation-And-PSO.md) | Permutation counts, compile times and pipeline state |
| [Debloat Guide](Debloat-Guide.md) | Reducing engine disk footprint and build time |

## The general approach

**Measure before changing anything.** The single most common failure in Unreal optimisation is optimising
the wrong thread. A game that is game-thread bound will not get faster if you reduce shader complexity, and
one that is GPU bound will not get faster if you reduce actor count.

**Know which target you are building for.** The [four performance targets](../EngineOverview/Performance-Targets.md) have
different rendering configurations, and a scene that fits the 4K30 fidelity target will not fit 4K120
stylised. Decide first.

**Prefer removing work over making work cheaper.** Turning off an effect is always cheaper than optimising
it. Vite's own approach demonstrates this: [`VITE_RT_PSO_DEBLOAT`](Compile-Time-Switches.md) removes
unused ray tracing permutations rather than making them faster.

**Budget explicitly.** At 60&nbsp;fps you have 16.6&nbsp;ms. Assigning that budget across game thread,
render thread, GPU and specific systems in advance turns "is this too slow?" into a question with an answer.

## Where Vite's performance comes from

It is worth being clear that Vite's advantage over UE5 is not one optimisation. It is the absence of
several regressions, documented in [UE4 versus UE5 Cost Analysis](../EngineOverview/UE4-Versus-UE5-Cost-Analysis.md):
cheaper materials, [PhysX rather than Chaos](../Physics/PhysX.md), lower character movement cost, smaller memory
footprint, cheaper Slate and UI, and a lighter render thread.

On top of that baseline, Vite adds targeted work: the [optimised SSAO path](../Rendering/Ambient-Occlusion.md),
[ray tracing PSO debloat](Compile-Time-Switches.md), engine tick reductions, and
[changed defaults](Engine-Defaults.md).

The practical implication is that most of the win is already yours before you optimise anything. Your job is
mostly not to spend it.

## See also

- [Performance Targets](../EngineOverview/Performance-Targets.md)
- [UE4 versus UE5 Cost Analysis](../EngineOverview/UE4-Versus-UE5-Cost-Analysis.md)
- [Rendering](../Rendering/Rendering.md)
- [Physics](../Physics/Physics.md)
