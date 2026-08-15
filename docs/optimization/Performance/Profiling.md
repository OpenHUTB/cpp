# Profiling

<tldr>
<p>
Find the bottleneck before optimising. <code>stat unit</code> tells you which thread is limiting the
frame; everything else follows from that answer.
</p>
</tldr>

Unreal Engine 4.27's profiling tools are mature and well documented. This page covers how to use them
in the context of Vite's [performance targets](Performance-Targets.md), and the Vite-specific things worth
watching.

## Start with stat unit

```
stat unit
```

This is the first command in every profiling session. It reports:

| Line | Meaning |
|---|---|
| **Frame** | Total frame time. This is your actual frame rate. |
| **Game** | Game thread: gameplay, ticks, physics, animation |
| **Draw** | Render thread: building draw commands, RHI submission |
| **GPU** | GPU time |
| **RHIT** | RHI thread, where present |

The largest of Game, Draw and GPU is your bottleneck. Frame time will be roughly equal to it, because the
threads run in parallel and the slowest one gates the rest.

This one reading determines everything that follows. Optimising a system on a thread that is not the
bottleneck produces exactly zero improvement in frame rate, which is the most common wasted effort in Unreal
performance work.

<procedure title="Identify the bottleneck" id="find-bottleneck">
    <step>Run <code>stat unit</code> in a representative scene, in a build configuration close to shipping.</step>
    <step>Note which of Game, Draw or GPU is closest to Frame.</step>
    <step>
        Confirm with <code>r.ScreenPercentage 50</code>. If frame time drops significantly, you are GPU
        bound at pixel cost. If it barely changes, you are not.
    </step>
    <step>Follow the relevant section below.</step>
</procedure>

## Game thread bound

The game thread is where UE4-era engines most often bottleneck, and where Vite has the most inherited
advantage over UE5.

```
stat game        // top-level game thread breakdown
stat physics     // physics simulation cost
stat anim        // animation and skeletal mesh evaluation
stat engine      // tick counts and general engine stats
stat slate       // UI cost
```

The usual suspects:

**Actor tick count.** Every ticking actor costs, even one whose tick does nothing. `stat engine` reports
tick counts. Disable tick on actors that do not need it, and use tick intervals rather than early-outs
inside `Tick`.

**Character movement.** CMC is expensive and scales badly with character count. The
[400 Characters CMC Bench](400-Characters-CMC-Bench.md) demo exists to measure exactly this. If you have
many characters, this is likely your cost.

**Physics.** See [PhysX](PhysX.md). Watch physics time as a fraction of game thread time. Large numbers of
simulated bodies should move to the [instanced subsystem](Instanced-Physics.md).

**Overlap events.** Vite disables overlap events by default on primitive components precisely because they
are a common hidden cost. See [Engine Default Changes](Engine-Defaults.md).

**Blueprint.** Blueprint VM execution is slower than native C++. Hot per-frame logic belongs in C++.

## Render thread bound

```
stat scenerendering    // render thread breakdown
stat initviews         // visibility and culling cost
stat rhi               // draw calls, primitives, triangles
```

Render thread cost is dominated by draw call count and visibility work.

**Draw calls.** `stat rhi` reports the count. Reduce with instancing, mesh merging and fewer material slots
per mesh. Note that Vite has no Nanite, so draw call discipline matters more than it does in UE5.

**Visibility.** `stat initviews` shows culling cost. Very high primitive counts make culling itself
expensive. Precomputed visibility and occlusion culling help; so does simply having fewer primitives.

## GPU bound

```
stat gpu               // GPU pass breakdown
profilegpu             // detailed single-frame GPU capture
r.ScreenPercentage 50  // A/B test for pixel-cost bound
```

`profilegpu` is the important one. It produces a per-pass breakdown of a single frame, which tells you
whether your GPU time is in base pass, shadows, ray tracing, post processing or something unexpected.

For a Vite project, the passes worth scrutinising:

| Pass | Notes |
|---|---|
| Ray tracing passes | See [Ray Tracing](Ray-Tracing.md). `r.RayTracing.ForceAllRayTracingEffects 0` establishes how much of the frame they account for. |
| DDGI | Should be modest and stable. See [Dynamic DDGI](DDGI-Dynamic.md). |
| Base pass | Driven by material complexity and overdraw |
| Shadow depths | Driven by shadow-casting light count and cascade configuration |
| Translucency | Driven by overdraw. Particles are the usual cause. |
| SMAA | Small and fixed. If it is large, check `r.Vite.SMAA.Mode`. |

## View modes

Viewport view modes are often faster than a profile capture for locating a problem:

| View mode | Reveals |
|---|---|
| Shader Complexity | Expensive materials and overdraw |
| Quad Overdraw | Small triangles wasting quad occupancy |
| Light Complexity | Overlapping dynamic lights |
| Lightmap Density | Lightmap resolution problems |
| Wireframe | Runaway [tessellation](Tessellation.md) and unexpected geometry density |

## Session Frontend

**Window > Developer Tools > Session Frontend > Profiler** captures a timeline you can scrub through, which
is the right tool for intermittent hitches. A hitch that happens once every thirty seconds will not show up
in `stat unit`.

Common hitch sources in a Vite project:

- Shader compilation on first encounter with a material. See
  [Shader Compilation and PSO](Shader-Compilation-And-PSO.md).
- Ray tracing acceleration structure builds when large amounts of geometry stream in.
- Physics body creation bursts. See `MaxAddActorsPerFrame` in
  [Instanced Physics](Instanced-Physics.md).
- Level streaming and asset loading.
- Garbage collection.

## Profiling against a target

Reading a number is only useful against a budget. Vite's [performance targets](Performance-Targets.md)
give you one:

| Target | Frame budget |
|---|---|
| Stylised 4K120 | 8.3&nbsp;ms |
| Performance High End 4K60 | 16.6&nbsp;ms |
| Fidelity High End 4K30 | 33.3&nbsp;ms |
| Fidelity Full RT 1440p30 | 33.3&nbsp;ms |

Profile on hardware representative of your minimum spec, in a Development or Test build rather than in the
editor, in a scene representative of worst-case gameplay rather than an empty level. Editor overhead alone
can be several milliseconds, and a profile taken in an empty level tells you nothing about the frame you
have to ship.

## See also

- [Performance Targets](Performance-Targets.md)
- [UE4 versus UE5 Cost Analysis](UE4-Versus-UE5-Cost-Analysis.md)
- [Engine Default Changes](Engine-Defaults.md)
- [Shader Compilation and PSO](Shader-Compilation-And-PSO.md)
