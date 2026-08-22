# 编译时开关

Five <code>VITE_*</code> macros in
<code>Engine/Source/Runtime/Core/Public/Misc/CoreDefines.h</code> control features that are decided at
compile time rather than by console variable. <b><code>VITE_RT_PSO_DEBLOAT</code> defaults to
<code>1</code> and compiles out several ray tracing effects entirely</b> &mdash; read that section before
concluding a feature is broken.

Some Vite features cannot be console variables. Either they change shader permutation sets, which are
decided when shaders are compiled, or they alter hot code paths where a runtime branch would cost more than
the feature saves.

## The switches

| Macro | Default | Effect when enabled |
|---|---|---|
| `VITE_RT_PSO_DEBLOAT` | `1` | Compiles out a large set of ray tracing shader permutations |
| `VITE_O_SSAO` | `1` | Vite's optimised SSAO memory access path |
| `VITE_PHYSX_FIXED_TIMESTEP` | `0` | Deterministic fixed-step PhysX with render interpolation |
| `VITE_DLSS_PATCH` | `0` | Output-resolution translucency and volumetric fog when upscaling |
| `VITE_NVRTX_TRANSLUCENCY_DEPTH` | `0` | Separate translucency depth texture from the NvRTX branch |

Each is defined with an `#ifndef` guard, so any of them can be overridden from the build system without
editing the header.

## Overriding a switch

<procedure title="Change a compile-time switch" id="override-switch">
    <step>
        Open your project's <code>Source/&lt;Project&gt;.Target.cs</code>, or the engine target file if
        you are changing it engine-wide.
    </step>
    <step>
        Add the definition to <code>GlobalDefinitions</code>:
        <code-block lang="c#">
public MyGameTarget(TargetInfo Target) : base(Target)
{
    Type = TargetType.Game;
    DefaultBuildSettings = BuildSettingsVersion.V2;
    ExtraModuleNames.Add("MyGame");

    GlobalDefinitions.Add("VITE_RT_PSO_DEBLOAT=0");
}
        </code-block>
    </step>
    <step>Regenerate project files.</step>
    <step>
        Rebuild the engine, and wipe the shader cache if you changed
        <code>VITE_RT_PSO_DEBLOAT</code> or any switch that affects shader permutations. See
        <a href="../Tools/Cache-Management.md">Cache Management</a>.
    </step>
</procedure>

Prefer `GlobalDefinitions` in a target file over editing `CoreDefines.h`. Editing the header changes the
value for every target and creates a diff against the engine repository that you will have to carry through
every merge.

## VITE_RT_PSO_DEBLOAT

This is the switch that surprises people, so it gets the most detail.

Ray tracing pipeline state objects are expensive in a way that is easy to miss. Every ray generation shader
permutation that *could* be used has to be compiled, packaged and bound into the ray tracing pipeline, even
if the effect it belongs to is disabled at runtime by a console variable. A CVar set to `0` does not stop
its shaders existing. The result is long shader compile times, large packaged builds, high PSO counts and
slow ray tracing pipeline creation at runtime.

`VITE_RT_PSO_DEBLOAT` cuts the permutation set down to the effects Vite actually ships, by returning `false`
from `ShouldCompilePermutation` for everything else.

### What is compiled out at the default value

With `VITE_RT_PSO_DEBLOAT=1`, these effects are **unavailable regardless of their console variables**:

| Effect | Normally controlled by |
|---|---|
| Per-pixel ray-traced global illumination | `r.RayTracing.GlobalIllumination` |
| RTXDI sampled direct lighting | `r.RayTracing.SampledDirectLighting` |
| Path tracing | `r.PathTracing`, path tracing view mode |
| Ray-traced translucency | `r.RayTracing.Translucency` |
| Mesh caustics | NvRTX caustics CVars |
| Water caustics | NvRTX water caustics CVars |
| Single layer water ray-traced reflections | `r.Water.SingleLayer.RTR` |
| Ray-traced reflection captures and reflection probes | `r.RayTracing.Reflections.RayTraceEnvironmentCaptures` |

Setting the console variable will appear to succeed and the effect will not render.

### What still works

| Effect | Notes |
|---|---|
| Ray-traced reflections | Forced to the sorted deferred algorithm. The older non-deferred path is compiled out. |
| Ray-traced shadows | Unaffected |
| Ray-traced ambient occlusion | Unaffected |
| Ray-traced sky light | Unaffected |
| DDGI | Unaffected. DDGI is a plugin and does not go through these permutations. |

The set that remains is exactly Vite's recommended configuration: [DDGI](../Rendering/DDGI-Dynamic.md) for indirect
lighting, [RT reflections](../Rendering/RT-Reflections.md) via the deferred path, and
[RT shadows and AO](../Rendering/RT-Shadows-And-Ambient-Occlusion.md). See
[Global Illumination](../Rendering/Global-Illumination.md) for why DDGI is preferred over per-pixel ray-traced GI in the
first place &mdash; the debloat switch encodes that recommendation into the build.

### Turning it off

Set `VITE_RT_PSO_DEBLOAT=0` and rebuild if you need path tracing for
[reference imagery](../Rendering/Path-Tracing.md), RTXDI for a many-lights scene, or ray-traced translucency and
caustics.

<warning>
Turning this off substantially increases shader compilation time, packaged build size and ray tracing PSO
count. Consider maintaining a separate target configuration &mdash; debloat off for an internal
reference-rendering build, debloat on for the shipping build &mdash; rather than turning it off globally.
</warning>

## VITE_O_SSAO

Enables Vite's rewritten memory access pattern for the screen-space ambient occlusion pass. Same visual
result as stock, lower cost.

Unusually among these switches, it also has a runtime CVar &mdash; but only in development builds:

```
r.Vite.SSAO 0   // stock UE path
r.Vite.SSAO 1   // optimised path
```

Shipping builds are locked to the compile-time value; the CVar does not exist. The runtime toggle exists so
the two paths can be A/B compared during development, which is how the optimisation was validated. There is
no reason to turn the compile-time switch off. See [Ambient Occlusion](../Rendering/Ambient-Occlusion.md).

## VITE_PHYSX_FIXED_TIMESTEP

Off by default. Enables deterministic fixed-step PhysX simulation with render interpolation, plus the
`p.VitePhysXFixedTimestep.*` console variables.

The compile-time gate exists because the feature adds branches and double-buffered transform storage to the
physics scene, substep task and animation physics blend. Projects with no determinism requirement should
not pay for it. Full documentation is in [Fixed Timestep](../Physics/Fixed-Timestep.md).

## VITE_DLSS_PATCH

Off by default. Adds `TranslucencyAfterDOFUpscaledRT` and `TranslucencyAfterDOFModulateUpscaledRT` mesh
passes so selected translucent primitives render at output resolution after upscaling, and adds
`r.VolumetricFog.UseUpScaledSizeVolumetricFog` so the fog grid can be computed from output resolution.

Only relevant to projects shipping with an upscaler enabled. See
[Upscalers and Frame Generation](../Rendering/Upscalers.md).

## VITE_NVRTX_TRANSLUCENCY_DEPTH

Off by default. Allocates and writes a separate translucency depth texture alongside separate translucency
colour, inherited from the NvRTX branch. Required by some NvRTX effects that need translucent depth; costs
an extra render target when enabled.

## Checking what a build has

Compile-time switches are invisible at runtime, which makes "is this feature even compiled in?" a real
question when debugging.

The most reliable check is to set the relevant CVar and see whether the effect appears. If
`r.RayTracing.SampledDirectLighting 1` produces no change in a scene with hundreds of lights,
`VITE_RT_PSO_DEBLOAT` is on.

For the switches with runtime CVars, checking whether the CVar exists at all is a direct test: in a Shipping
build, `r.Vite.SSAO` will not be found because it is compiled out.

## See also

- [Shader Compilation and PSO](Shader-Compilation-And-PSO.md)
- [Ray Tracing](../Rendering/Ray-Tracing.md)
- [Fixed Timestep](../Physics/Fixed-Timestep.md)
- [Cache Management](../Tools/Cache-Management.md)
- [Build from Source](../GettingStarted/Build-From-Source.md)
