# Rendering

<tldr>
<p>
Vite's renderer is a deferred SM5 renderer with an engine-agnostic DXR ray tracing pipeline. Global
illumination is <a href="DDGI-Dynamic.md">DDGI</a> rather than Lumen, geometric detail is
<a href="Tessellation.md">tessellation</a> rather than Nanite, and anti-aliasing defaults to native
resolution with <a href="Anti-Aliasing.md">SMAA</a> rather than temporal reconstruction.
</p>
</tldr>

This is the largest section of the manual, because rendering is where Vite diverges most from both stock
Unreal Engine 4.27 and from UE5.

## How the renderer is put together

The pipeline is Unreal Engine 4.27's deferred renderer, inheriting NvRTX Caustics' DirectX 12 and DXR
improvements, with Vite's own additions layered on top. The important structural property is that the ray
tracing pipeline is *agnostic*: acceleration structures are built and updated independently of any
particular lighting solution, which is what allows DDGI, RTXDI and the various RT effects to coexist and be
enabled independently.

That is the opposite of the UE 5.1+ arrangement, where ray tracing scene construction is coupled to Lumen,
Nanite and GPU Scene. See [Why NvRTX 4.27](Why-NvRTX-427.md).

> Vite enables Ray Tracing effects by default in new projects. This is intentional for
> discoverability but means you should consciously decide what to turn off. See
> [Ray Tracing](Ray-Tracing.md).
>
{style="warning"}

## Section contents

### Global illumination

- [Global Illumination](Global-Illumination.md) &mdash; choosing between the available GI solutions
- [Dynamic DDGI](DDGI-Dynamic.md) &mdash; real-time ray-traced irradiance probe volumes
- [Static DDGI](DDGI-Static.md) &mdash; baked probe volumes with near-instant bake times
- [SSGI](SSGI.md) &mdash; screen-space GI, and why it belongs alongside DDGI

### Ray tracing

- [Ray Tracing](Ray-Tracing.md) &mdash; overview and the master switches
- [Ray-Traced Reflections](RT-Reflections.md)
- [Ray-Traced Shadows and Ambient Occlusion](RT-Shadows-And-Ambient-Occlusion.md)
- [光线追踪半透明和焦散](RT-Translucency-And-Caustics.md)
- [RTXDI](RTXDI.md) &mdash; ray-traced direct lighting for many-light scenes
- [Path Tracing](Path-Tracing.md)

### Surface and geometry

- [Shading Models](Shading-Models.md) &mdash; including Callisto BRDF and Toon
- [Tessellation](Tessellation.md)
- [Hair Rendering](Hair-Rendering.md) &mdash; TressFX

### Image quality

- [Anti-Aliasing](Anti-Aliasing.md) &mdash; SMAA, FXAA, TAA and MSAA
- [Upscalers and Frame Generation](Upscalers.md) &mdash; DLSS, FSR, XeSS, NIS
- [Ambient Occlusion](Ambient-Occlusion.md) &mdash; SSAO fast path and HBAO+
- [Colour Management](Color-Management.md) &mdash; ACES, tonemapping and HDR output

## Choosing a rendering configuration

The right starting point depends on your performance target. These are the configurations behind the four
targets in [Performance Targets](Performance-Targets.md).

| Target | GI | Reflections | Shadows | AO             | Geometry | AA          |
|---|---|---|---|----------------|---|-------------|
| Stylised 4K120 | Dynamic DDGI | Raster / SSR | Cascaded | SSAO fast path | LODs | SMAA / TAA  |
| Performance 4K60 | DDGI + SSGI | Ray traced | Cascaded | SSAO / HBAO4+  | Tessellation | TAA / SMAA  |
| Fidelity 4K30 | DDGI + SSGI | Ray traced | Cascaded | HBAO4+         | Tessellation | TAA         |
| Full RT 1440p30 | DDGI + SSGI | Ray traced | Ray traced | RTAO           | Tessellation | TAA         |
| No-RT hardware | Static DDGI | SSR | Cascaded | SSAO           | LODs | TAA / SMAA / FXAA |

Every one of these is a starting point, not a prescription. The console variables for each are documented
on the corresponding pages and collected in the
[Console Variable Reference](Console-Variables.md).

## See also

- [Performance Targets](Performance-Targets.md)
- [Profiling and Benchmarking](Profiling.md)
- [Shader Compilation and PSO](Shader-Compilation-And-PSO.md)
- [Console Variable Reference](Console-Variables.md)
