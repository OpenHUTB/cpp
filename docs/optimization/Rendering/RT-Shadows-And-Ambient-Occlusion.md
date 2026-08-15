# Ray-Traced Shadows and Ambient Occlusion

<tldr>
<p>
<code>r.RayTracing.Shadows 1</code> and <code>r.RayTracing.AmbientOcclusion 1</code>. Both are enabled by
default in new Vite projects. Both are typically the first things to turn off when a project needs frame
time, because cheaper alternatives get close for much less.
</p>
</tldr>

These two effects are grouped together because they share a cost profile, a set of tuning controls, and a
recommendation: enable them when your target has room, disable them first when it does not.

## Ray-traced shadows

Ray-traced shadows replace shadow map sampling with rays traced from the shaded point toward each light.
The benefits are correct contact hardening, accurate soft shadows from area lights, and no shadow map
resolution artefacts, peter-panning or cascade transitions.

The costs are real. Every light casting ray-traced shadows adds rays per pixel, so cost scales with light
count in a way shadow maps do not. Shadow maps amortise across the frame; ray-traced shadows do not.

### Tuning

**Sample count per light.** The dominant quality and cost control. Low counts are noisy and lean on the
denoiser; high counts are expensive.

**Light source radius.** Larger radii produce softer shadows and need more samples to resolve cleanly. A
scene tuned for sharp shadows can use far fewer samples.

**Per-light opt-in.** Rather than enabling ray-traced shadows globally, enable them on the lights where the
difference is visible &mdash; usually the key light and any large area light &mdash; and leave fill lights
on shadow maps. This is by far the most effective optimisation available and it is frequently overlooked.

**Maximum distance.** Bounding shadow ray distance limits cost in large scenes.

Vite includes rendering optimisations specifically targeting RT shadows and RT direct lighting, and further
performance work on RTAO and RT shadows is in progress. See [Release Notes](Release-Notes.md).

## Ray-traced ambient occlusion

RTAO traces short rays into the hemisphere around each shaded point to determine how occluded it is. Unlike
screen-space AO it accounts for geometry that is off-screen or occluded, so it does not exhibit the halo and
disocclusion artefacts SSAO produces under camera motion.

### Tuning

**Radius.** How far occlusion rays travel. This is an art direction control as much as a performance one
&mdash; small radii give tight contact darkening, large radii give broad ambient shaping and cost more.

**Samples per pixel.** Quality against cost, mediated by the denoiser.

**Intensity.** Post-process strength. Cheap to change and worth exhausting before increasing sample counts.

**Resolution.** Half-resolution RTAO is often visually indistinguishable and substantially cheaper.

## The cheaper alternatives

Before committing frame time to either effect, know what you are buying relative to the alternatives.

| Effect | Alternative | Trade-off |
|---|---|---|
| RT shadows | Cascaded shadow maps | Much cheaper. Resolution artefacts, no correct contact hardening, cascade transitions. |
| RT shadows | Distance field shadows | Cheap soft shadows. Approximate, needs mesh distance fields. |
| RTAO | [SSAO fast path](Ambient-Occlusion.md) | Dramatically cheaper. Screen-space artefacts. Vite's implementation is significantly optimised. |
| RTAO | [HBAO+](Ambient-Occlusion.md) | Higher quality than SSAO at moderate cost. DirectX 11 only. |

In practice, most Vite projects targeting 60 FPS or above run cascaded shadow maps and either the SSAO fast
path or HBAO+, and spend the ray tracing budget on [DDGI](DDGI-Dynamic.md) and
[reflections](RT-Reflections.md) instead. Only the 1440p30 "Fidelity, full RT" target in
[Performance Targets](Performance-Targets.md) enables both RT shadows and RTAO.

Note that Vite's SSAO has both a fast path and a memory-access optimisation. It is much cheaper than the
stock 4.27 implementation, which shifts the calculus further away from RTAO than you might expect from
experience with other engines.

## Disabling

```ini
; Config/DefaultEngine.ini
[/Script/Engine.RendererSettings]
r.RayTracing.Shadows=0
r.RayTracing.AmbientOcclusion=0
```

Measure with `stat gpu` before and after. In a scene with many shadow-casting lights, RT shadows are
frequently the single largest ray tracing cost in the frame, ahead of reflections.

## See also

- [Ray Tracing](Ray-Tracing.md)
- [Ambient Occlusion](Ambient-Occlusion.md)
- [RTXDI](RTXDI.md)
- [Performance Targets](Performance-Targets.md)
