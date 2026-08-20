# Ray-Traced Reflections

<tldr>
<p>
Enabled with <code>r.RayTracing.Reflections 1</code>. Vite's reflections are heavily optimised and can
run at 4K native 60 FPS on PS5-class hardware. When <a href="DDGI-Dynamic.md">DDGI</a> is active,
reflection rays sample probe irradiance for their secondary bounce, which is a large quality win.
</p>
</tldr>

Ray-traced reflections replace screen-space reflections with rays traced against the actual scene, which
resolves the two fundamental SSR failures: reflections of off-screen geometry, and reflections of surfaces
facing away from the camera.

![](../../img/optimization/SSRArtifacts.png)

*反射表面显示屏幕空间反射丢失，即反射几何体离开画面的位置。*

*图中圈出的故障模式是：反射在反射几何体离开屏幕处终止，因为屏幕空间反射已经没有可供采样的内容了。*


## Performance

This is one of the areas Vite has invested the most optimisation work in, because reflections are usually
the second most expensive ray tracing effect after per-pixel GI.

Vite's reflections are capable of running at 4K native 60 FPS on a PS5-class GPU, as demonstrated by the
Unreal Tournament Vite demo, which combines them with DDGI and tessellation. That configuration is the
"Performance, high end" row in [Performance Targets](Performance-Targets.md).

Further RT reflection performance work is ongoing, with the stated goal of reaching performance
characteristics comparable to RE Engine's implementation as seen in Devil May Cry 5 Special Edition on PS5.

## DDGI-lit reflections

The most important interaction in the renderer, and one of the concrete reasons Vite is built on the NvRTX
branch rather than stock 4.27.

A reflection ray hits a surface. That surface needs to be shaded, which requires knowing the indirect light
arriving at it. Without a global illumination representation available to the ray tracing pipeline, the
options are to return direct lighting only (reflections of shadowed areas go black), or to fall back to a
cubemap (reflections look wrong and static).

With DDGI active, the reflection ray samples probe irradiance at the hit point. Reflected surfaces are lit
consistently with the rest of the scene, shadowed areas in reflections retain bounce light, and the whole
image becomes coherent.

This is enabled by the engine-side integration rather than by a separate switch: run DDGI and RT reflections
together and you get it.

## Tuning

The controls that matter, in rough order of impact on cost:

**Resolution.** Reflections can be traced at reduced resolution and upscaled. This is usually the largest
single saving available and the quality cost is modest on rough surfaces.

**Maximum roughness.** Surfaces rougher than this threshold fall back to cheaper techniques. Lowering it is
very effective, because rough reflections are exactly the case where the expensive accurate answer is least
visible.

**Maximum bounces.** Additional bounces are expensive and rarely visible outside of scenes deliberately
built with facing mirrors.

**Ray distance.** Limiting how far reflection rays travel bounds cost in large open scenes.

**Denoiser settings.** Reflections use a denoiser, unlike DDGI. If you see boiling or ghosting in
reflections under motion, this is where to look.

## Which reflection algorithm runs

Vite has two ray-traced reflection implementations inherited from 4.27 and NvRTX: the older
`FRayTracingReflectionsRGS` path, which supports hybrid reflections and reflected translucency, and the
newer sorted deferred path.

In a default build, `VITE_RT_PSO_DEBLOAT` forces the **sorted deferred** path and compiles out the older
one. This is the faster of the two and the one Vite's [performance targets](Performance-Targets.md) are
measured against.

Consequences worth knowing:

- `r.RayTracing.Reflections.ExperimentalDeferred` has no effect &mdash; the deferred path is used
  unconditionally.
- Hybrid reflections and the reflected translucency controls below are unavailable.
- Ray-traced reflection captures and reflection probes
  (`r.RayTracing.Reflections.RayTraceEnvironmentCaptures`) are compiled out.
- Single layer water ray-traced reflections (`r.Water.SingleLayer.RTR`) are compiled out.

See [Compile-Time Switches](Compile-Time-Switches.md) to restore the full set.

## Reflected translucency

<note>
Requires <code>VITE_RT_PSO_DEBLOAT=0</code>. These controls are permutation dimensions on the non-deferred
reflection shader, which is compiled out by default.
</note>

By default, Unreal Engine 4 only blends the emissive colour of reflected translucent meshes when rendering
ray-traced reflections, which makes glass and similar surfaces look wrong in reflections. Vite inherits the
NvRTX option to render fully ray-traced translucent objects inside reflections:

| Console variable | Purpose |
|---|---|
| `r.RayTracing.Reflections.ReflectedTranslucencyMode` | 0 emissive only, 1 shading, 2 shading and refraction, 3 shading, refraction and absorption. Default 0. |
| `r.RayTracing.Reflections.ReflectedTranslucencyMaxBounces` | Maximum ray-traced translucency bounces inside reflections. Default 8. |
| `r.RayTracing.Reflections.ReflectedTranslucencyTransmissionThreshold` | Stops the translucency walk when accumulated transmission falls below this. Default 0.1. |

Mode 3 gives the best result and the highest cost. In scenes with a lot of glass visible in reflections, the
difference is substantial; in scenes without, leave it at 0.

## When to use SSR instead

Screen-space reflections remain the right answer for the 4K120 stylised target and for any project whose
minimum spec lacks DXR. SSR is dramatically cheaper, and in scenes without large flat reflective surfaces
its failures are frequently not noticeable.

The cases where RT reflections earn their cost are wet streets, polished floors, water, glass architecture
and vehicle paint &mdash; anywhere a large, smooth surface reflects something the camera cannot see.

## See also

- [Ray Tracing](Ray-Tracing.md)
- [Dynamic DDGI](DDGI-Dynamic.md)
- [RT Translucency and Caustics](RT-Translucency-And-Caustics.md)
- [Compile-Time Switches](Compile-Time-Switches.md)
- [Performance Targets](Performance-Targets.md)
