# Stylized Raytracing Demo

<tldr>
<p>
Ray tracing applied to non-photorealistic art direction. A counterexample to the assumption that ray
tracing only serves photorealism.
</p>
</tldr>

<img src="StylizedRTDemo.png" alt="Stylized outdoor scene lit by dynamic DDGI, frame counter reading 811 FPS" border-effect="line"/>

*Dynamic DDGI, no baked lighting. The counter reads 811 FPS at 1440p native on an RTX 4080 Super.*

## Download

[Stylized Raytracing Demo](https://drive.google.com/file/d/1M0H60ESNuvUltF9eePO-CHrlUuBzvFHh/view)

## Why stylized ray tracing

Ray tracing is usually presented as a photorealism feature, which leads stylized projects to skip it. That
is a mistake. What ray tracing actually provides is correct light transport, and stylized rendering
benefits from correct light transport just as much &mdash; arguably more, because stylized art tends to use
strong, deliberate lighting where screen-space artefacts are conspicuous.

Specifically:

| Ray-traced feature | What it gives stylized work |
|---|---|
| [Reflections](RT-Reflections.md) | Reflections of off-screen geometry, which screen-space reflections cannot produce |
| [Shadows](RT-Shadows-And-Ambient-Occlusion.md) | Contact-accurate shadows without shadow map resolution and bias tuning |
| [DDGI](DDGI-Dynamic.md) | Colour bleed that reinforces a limited palette rather than fighting it |
| [Ambient occlusion](Ambient-Occlusion.md) | Grounded contact without the halos screen-space AO produces |

Combining ray-traced lighting with the [Toon shading model](Shading-Models.md) is well supported in Vite
&mdash; Toon is one of the custom shading models the fork adds.

## What to look at

Turn ray-traced reflections off and on in the demo. The difference is largest exactly where screen-space
reflections fail: reflections of things outside the frame, at grazing angles, and behind the camera.

Then look at how the stylized materials respond to the ray-traced lighting rather than to a baked
approximation of it. The lighting is doing work the art direction can rely on being consistent as the
camera moves.

## Availability caveat

<warning>
Reflections, shadows, ambient occlusion and sky light are available in a default Vite build. Translucency,
caustics, RTXDI, path tracing and per-pixel ray-traced GI are <b>compiled out</b> by
<code>VITE_RT_PSO_DEBLOAT</code>, which defaults to <code>1</code>.
<p>
If a console variable from that second group appears to do nothing, this is why. See
<a href="Ray-Tracing.md">Ray Tracing</a> and
<a href="Compile-Time-Switches.md">Compile-Time Switches</a>.
</p>
</warning>

## See also

- [Ray Tracing](Ray-Tracing.md)
- [Shading Models](Shading-Models.md)
- [Dynamic DDGI](DDGI-Dynamic.md)
- [Compile-Time Switches](Compile-Time-Switches.md)
