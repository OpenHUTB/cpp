# Path Tracing

<tldr>
<p>
Vite includes NVIDIA's path tracing technology, related to the rendering stack featured in Black Myth: Wukong.
The Editor Pathtracer is used for reference imagery, lighting validation, previsualisation and marketing captures &mdash; not as a
runtime target.
</p>
<p>
<b>Compiled out in a default build.</b> Requires rebuilding with
<code>VITE_RT_PSO_DEBLOAT=0</code>.
</p>
</tldr>

Path tracing produces a ground-truth image by tracing full light transport paths, accumulating samples over
many frames until the result converges. It is the reference against which the real-time techniques in this
section are approximations.

## What it is for

**Lighting validation.** Path tracing tells you what the scene *should* look like. When
[DDGI](DDGI-Dynamic.md) output looks wrong, and you cannot tell whether the problem is probe density, volume
placement or the lighting rig itself, a converged path-traced frame from the same camera answers the
question.

**Reference imagery.** Marketing shots, key art and promotional captures where a converged still is
acceptable and frame time is irrelevant.

**Pre-visualization.** Establishing a lighting target before building the real-time approximation of it.

**Material authoring.** Verifying that a complex material behaves correctly under full light transport
before checking how the real-time path approximates it.

## What it is not for

Path tracing is not a main shipping runtime configuration in Vite, and it is not one of the
[performance targets](Performance-Targets.md). Convergence takes many frames; a camera cut restarts
accumulation from scratch. *The Editor Path tracer path setup is not the same as what can be shipped for runtime title. 

Black Myth: Wukong shipped a Path Tracing mode based on the same PT tech that's available in Vite; for this title 
it is offered as a high-end PC option to be used with upscaling and frame generation, not as the console rendering path.

## Using it

Path tracing shaders are compiled out when `VITE_RT_PSO_DEBLOAT` is `1`, which is the default. Rebuild the
engine with `VITE_RT_PSO_DEBLOAT=0` first &mdash; see [Compile-Time Switches](Compile-Time-Switches.md).

Because path tracing is a tool you use occasionally rather than something the game ships with, the cleanest
arrangement is a separate editor target configuration with the debloat switch off, used for reference
capture, while the game target keeps the default.

Once enabled, path tracing is enabled through the standard Unreal Engine 4.27 controls:

```
r.PathTracing 1
```

The view mode can also be selected from the viewport's view mode dropdown. Accumulation restarts whenever
the camera or scene changes, so let it converge before evaluating an image.

Key considerations:

- **Sample count** determines convergence. More samples, less noise, longer wait.
- **Maximum bounces** determines how much indirect light is captured. Interiors need more than exteriors.
- **Movie Render Queue** is the right tool for producing converged sequences, since it can hold each frame
  until it has accumulated the requested number of samples.

## Comparing against the real-time path

The most useful workflow is A/B comparison from a fixed camera.

<procedure title="Validate real-time lighting against path tracing" id="validate-lighting">
    <step>Place a camera at a representative viewpoint and lock it.</step>
    <step>Capture the real-time image with your shipping rendering configuration.</step>
    <step>Switch to path tracing and let the frame converge fully.</step>
    <step>
        Compare. Differences in overall brightness and bounce colour usually indicate DDGI probe density or
        volume placement problems. Differences confined to contact areas indicate you need
        <a href="SSGI.md">SSGI</a> or stronger <a href="Ambient-Occlusion.md">ambient occlusion</a>.
        Differences in reflections point at <a href="RT-Reflections.md">reflection</a> settings.
    </step>
    <step>Adjust the real-time configuration and repeat, rather than adjusting the lighting rig to compensate.</step>
</procedure>

That last step is the important one. If the real-time approximation is wrong, fix the approximation.
Compensating by distorting the lighting rig produces a scene that only looks correct from one camera and
under one configuration.

## See also

- [Ray Tracing](Ray-Tracing.md)
- [Global Illumination](Global-Illumination.md)
- [Dynamic DDGI](DDGI-Dynamic.md)
- [Compile-Time Switches](Compile-Time-Switches.md)
- [Colour Management](Color-Management.md)
