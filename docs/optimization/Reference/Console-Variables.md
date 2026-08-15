# Console Variables

<tldr>
<p>
Vite-specific console variables and the stock ones you most often need, grouped by subsystem. This is a
curated list, not an exhaustive dump &mdash; use <code>help</code> or the console's autocomplete for the
full set.
</p>
</tldr>

<warning>
Several ray tracing variables in this list are <b>compiled out</b> in a default build. They will set
successfully and render nothing. Availability is noted per variable; see
<a href="Compile-Time-Switches.md">Compile-Time Switches</a>.
</warning>

Set variables at runtime in the console, or persistently in `Config\DefaultEngine.ini`:

```ini
[/Script/Engine.RendererSettings]
r.Vite.SMAA.Mode=1
r.HBAO.Enable=1
```

## Vite-specific

These do not exist in stock UE 4.27.

| Variable | Default | Purpose |
|---|---|---|
| `r.Vite.SMAA.Mode` | 1 | SMAA quality. 0 = Low, 1 = High. See [Anti-Aliasing](Anti-Aliasing.md). |
| `r.Vite.SSAO` | &mdash; | Vite's optimised SSAO path. See [Ambient Occlusion](Ambient-Occlusion.md). |
| `r.AntiAliasing.SMAA.Debug` | &mdash; | SMAA debug visualisation |
| `p.VitePhysXFixedTimestep.Enabled` | 0 | Enable fixed timestep physics |
| `p.VitePhysXFixedTimestep.DeltaTime` | 1/60 | Fixed step duration in seconds |
| `p.VitePhysXFixedTimestep.MaxTimesteps` | &mdash; | Maximum steps per frame |
| `p.VitePhysXFixedTimestep.MaxCumulativeExtraSteps` | &mdash; | Cap on accumulated catch-up steps |
| `p.VitePhysXFixedTimestep.LimitType` | &mdash; | How overload is handled |
| `p.VitePhysXFixedTimestep.InterpolationMode` | &mdash; | Render interpolation between fixed steps |

<note>
The <code>p.VitePhysXFixedTimestep.*</code> variables require a build with
<code>VITE_PHYSX_FIXED_TIMESTEP=1</code>. See <a href="Fixed-Timestep.md">Fixed Timestep</a>.
</note>

## Ray tracing

| Variable | Default | Availability | Page |
|---|---|---|---|
| `r.RayTracing.Reflections` | 0 | Available | [RT Reflections](RT-Reflections.md) |
| `r.RayTracing.Shadows` | 0 | Available | [RT Shadows and AO](RT-Shadows-And-Ambient-Occlusion.md) |
| `r.RayTracing.AmbientOcclusion` | 0 | Available | [Ambient Occlusion](Ambient-Occlusion.md) |
| `r.RayTracing.SkyLight` | &mdash; | Available | [Ray Tracing](Ray-Tracing.md) |
| `r.RayTracing.GlobalIllumination` | -1 | **Compiled out** | [Global Illumination](Global-Illumination.md) |
| `r.RayTracing.Translucency` | 0 | **Compiled out** | [RT Translucency and Caustics](RT-Translucency-And-Caustics.md) |
| `r.RayTracing.SampledDirectLighting` | 0 | **Compiled out** | [RTXDI](RTXDI.md) |

A value of `-1` means the setting is driven by the post-process volume rather than the console variable.

### Culling and scene setup

| Variable | Purpose |
|---|---|
| `r.RayTracing.Culling` | Ray tracing instance culling mode |
| `r.RayTracing.Culling.Radius` | Culling distance |
| `r.RayTracing.Culling.Angle` | Angular culling threshold |
| `r.RayTracing.SceneCaptures` | Whether scene captures build ray tracing scenes |
| `r.RayTracing.PreGather` | Pre-gather pass control |

Culling settings are among the most effective ray tracing performance levers, and Vite changes some of
their defaults &mdash; see [Engine Defaults](Engine-Defaults.md).

### Sky light

| Variable | Purpose |
|---|---|
| `r.RayTracing.SkyLight.SamplesPerPixel` | Sample count |
| `r.RayTracing.SkyLight.MaxRayDistance` | Ray length limit |
| `r.RayTracing.SkyLight.ScreenPercentage` | Resolution scale for the sky light pass |
| `r.RayTracing.SkyLight.Denoiser` | Denoiser selection |
| `r.RayTracing.SkyLight.EnableMaterials` | Evaluate materials on hit |
| `r.RayTracing.SkyLight.EnableTwoSidedGeometry` | Two-sided geometry handling |

`ScreenPercentage` and `SamplesPerPixel` are the two to reach for when the sky light pass is too expensive.

### Translucency

Compiled out by default.

| Variable | Purpose |
|---|---|
| `r.RayTracing.Translucency.HalfRes` | Half-resolution translucency |
| `r.RayTracing.Translucency.HybridDepthThreshold` | Hybrid translucency depth threshold |
| `r.RayTracing.Translucency.HybridDepthBias` | Hybrid translucency depth bias |
| `r.RayTracing.PrimaryRays.IncludeDOF` | Include depth of field in primary rays |

### RTXDI (sampled lighting)

Compiled out by default. `r.RayTracing.SampledDirectLighting` is the master switch; the
`r.RayTracing.SampledLighting.*` family &mdash; over sixty variables covering reservoir counts, spatial and
temporal resampling, ReGIR cell configuration and per-light-type toggles &mdash; tunes it.

The ones worth knowing first:

| Variable | Default | Purpose |
|---|---|---|
| `r.RayTracing.SampledLighting.Mode` | 1 | Sampling mode |
| `r.RayTracing.SampledLighting.InitialSamples` | 8 | Initial candidate samples |
| `r.RayTracing.SampledLighting.Spatial` | 1 | Spatial resampling |
| `r.RayTracing.SampledLighting.Temporal` | 1 | Temporal resampling |
| `r.RayTracing.SampledLighting.NumReservoirs` | -1 | Reservoir count, -1 for automatic |
| `r.RayTracing.SampledLighting.Denoiser` | &mdash; | Denoiser selection |
| `r.RayTracing.SampledLighting.DebugMode` | &mdash; | Debug visualisation |

See [RTXDI](RTXDI.md).

### Caustics

Mesh and water caustics are driven primarily by post-process volume settings rather than console variables,
and both are compiled out by default. Related variables:

| Variable | Purpose |
|---|---|
| `r.RayTracing.BuildRayTracingMeshForCaustics` | Build caustics-capable ray tracing geometry |
| `r.ParallelCausticsMap` | Parallel caustics map rendering |
| `r.PathTracing.ApproximateCaustics` | Approximate caustics in the path tracer |

See [RT Translucency and Caustics](RT-Translucency-And-Caustics.md).

## Global illumination

| Variable | Purpose |
|---|---|
| `r.RTXGI.DDGI` | Master DDGI toggle |
| `r.RTXGI.DDGI.LightingPass.Scale` | Lighting pass resolution scale |
| `r.RTXGI.DDGI.LightingPass.RelativeDistanceThreshold` | Distance rejection threshold |
| `r.RTXGI.DDGI.LightingPass.NormalPower` | Normal weighting |
| `r.RTXGI.DDGI.ProbesTextureVis` | Probe texture visualisation |
| `r.RTXGI.DDGI.ProbesTextureVis.IrradianceScalar` | Visualisation scaling |
| `r.RTXGI.DDGI.StatVolume` | Per-volume statistics |
| `r.RTXGI.MemoryUsed` | Report DDGI memory usage |

The RTXGI plugin is enabled by default. See [Dynamic DDGI](DDGI-Dynamic.md) and
[Global Illumination](Global-Illumination.md).

## Ambient occlusion

| Variable | Default | Purpose |
|---|---|---|
| `r.HBAO.Enable` | 0 | Enable HBAO+. Multiplies over the screen-space AO buffer. |
| `r.HBAO.HighPrecisionDepth` | 0 | 0 = FP16 internal depth, 1 = FP32. Use FP32 to avoid self-occlusion banding on distant objects. |
| `r.HBAO.GBufferNormals` | 1 | 0 = reconstruct normals from depth, 1 = fetch GBuffer normals |

HBAO+ has additional per-volume settings in the post-process volume: power exponent (default 2.0), radius
(2.0), bias (0.1), small-scale AO (1.0), blur radius (2 pixels), blur sharpness (16.0), max view depth
(9500), depth sharpness (50.0), and foreground and background AO distances.

<note>
The <code>r.HBAO.Enable</code> help text describes HBAO+ as DX11-only. This is out of date; there is a
working D3D12 implementation. See <a href="Ambient-Occlusion.md">Ambient Occlusion</a>.
</note>

## Tessellation

| Variable | Default | Purpose |
|---|---|---|
| `r.TessellationAdaptivePixelsPerTriangle` | 48.0 | Global tessellation factor multiplier. Lower means more triangles. |

See [Tessellation](Tessellation.md).

## Hair

| Variable | Purpose |
|---|---|
| `r.TressFX.StrandsMode` | Debug render mode. 0 = off, 1 = simulation strands, 2 = render strands with simulation influence, 3 = hair UV, 4 = root UV, 5 = seed, 6 = dimensions |
| `r.TressFX.Interoplation.FrustumCulling` | Frustum culling for interpolation. Default 1. |
| `r.TressFX.MorphTargetMeshVisualization` | Morph target mesh visualisation |

See [Hair Rendering](Hair-Rendering.md).

## Volumetric fog

| Variable | Purpose |
|---|---|
| `r.RayTracing.VolumeFogMode` | Ray traced volumetric fog mode |
| `r.VolumetricFog.UseUpScaledSizeVolumetricFog` | Render volumetric fog at output rather than internal resolution |

<note>
<code>r.VolumetricFog.UseUpScaledSizeVolumetricFog</code> only exists in builds with
<code>VITE_DLSS_PATCH=1</code>. It exists to fix volumetric fog resolution when upscaling. See
<a href="Upscalers.md">Upscalers</a>.
</note>

## Profiling

Standard Unreal, but worth having in one place:

| Command | Shows |
|---|---|
| `stat unit` | Frame, Game, Draw, GPU and RHIT timings. Start here. |
| `stat unitgraph` | The same over time |
| `stat game` | Game thread breakdown |
| `stat scenerendering` | Render thread breakdown |
| `stat gpu` | GPU pass breakdown |
| `stat physics` | Physics solver time |
| `stat rhi` | RHI resource statistics |
| `stat memory` | Memory usage |
| `profilegpu` | Single-frame GPU capture |
| `recompileshaders changed` | Recompile modified shaders |
| `recompileshaders global` | Recompile global shaders |

See [Profiling](Profiling.md).

## Finding variables yourself

| Command | Effect |
|---|---|
| `help` | Dumps all console variables and commands to the log |
| `<partial name>` then Tab | Autocomplete in the console |
| `<variable>` with no value | Prints the current value and help text |
| `DumpConsoleCommands` | Lists commands |

<warning>
Help text in the engine is occasionally stale &mdash; HBAO+'s DX11-only claim is one example. When
behaviour and help text disagree, the source is authoritative.
</warning>

## See also

- [Compile-Time Switches](Compile-Time-Switches.md)
- [Engine Defaults](Engine-Defaults.md)
- [Profiling](Profiling.md)
- [Glossary](Glossary.md)
