# Introduction to Vite

<tldr>
<p>
Vite is a production-oriented fork of Unreal Engine 4.27, based on NVIDIA's NvRTX Caustics branch.
It keeps PhysX 3.4 and an engine-agnostic ray tracing pipeline, and replaces Epic's UE5 feature stack
(Lumen, Nanite, VSM, TSR, Chaos) with lighter alternatives that hit native resolution at high frame rates.
</p>
</tldr>

Unreal Engine Vite is built for professional game development and supports titles currently in active
production. Its long-term goal is a continuously evolving modern engine that delivers CPU and rendering
throughput competitive with proprietary in-house engines, with ongoing performance, stability and
graphics-pipeline work aimed at contemporary console hardware.

The objective is specific enough to be falsifiable: **beat Epic's UE5 on fidelity per millisecond and on
simulation scale**, and be competitive with the proprietary AAA engines on both. Fidelity per millisecond
is what [Performance Targets](Performance-Targets.md) and
[UE4 versus UE5 Cost Analysis](UE4-Versus-UE5-Cost-Analysis.md) measure. Simulation scale is what the
[physics](Physics-Cube-Bench.md) and [character](400-Characters-CMC-Bench.md) benchmarks measure. Where
Vite does not currently win, this manual says so.

## The design argument

Epic's Unreal Engine 5.7 and 5.8 target roughly 60 FPS at dynamic internal resolutions of 720p&ndash;1080p
on PlayStation 5 using Lumen, Nanite, Virtual Shadow Maps, Temporal Super Resolution and Chaos. That is
what shipped titles on the engine demonstrate in practice.

The virtualized approach &mdash; virtualized geometry, shadows and textures, plus reconstructed resolution
&mdash; adds processing, streaming and memory overhead. Temporal reconstruction, denoising and stochastic
sampling introduce noise, ghosting, instability and blur. Substrate, GPU Scene, RDG, heavier shader models
and general feature expansion increase base renderer overhead, shader permutation counts, bytecode size,
PSO counts, compilation time and cache sizes relative to UE4. Beyond Chaos, CPU cost grows through heavier
scene maintenance, GPU Scene uploads, Lumen updates, Nanite streaming, VSM invalidation, World Partition,
and render-thread and RHI workloads.

Meanwhile the hardware is moving the other way. The Nintendo Switch 2 has shipped with an expected
seven-to-eight year lifespan. Handhelds substantially less powerful than a PS5 are now a mainstream
segment, including Valve's Steam Deck and Steam Machine. Hardware costs are rising under AI demand. Against
that backdrop, UE5's performance targets look increasingly misaligned with the machines games actually ship
on, and the rendering stack is arguably better suited to film, virtual production and high-end PC than to
sustainable long-term game development across mass-market hardware.

Vite takes the opposite position: prioritise high visual fidelity while holding strict frame-time budgets
at high native resolutions on console-class hardware.

> A scene running in Vite with ray-traced global illumination, ray-traced reflections and tessellation
> outperforms the same scene in UE 5.7 with no ray tracing, Lumen, Nanite or tessellation at all. This
> holds at 4K native on an RTX 4080 Super and on an RDNA2 RX 6700 (PS5 equivalent), and at native
> resolution on Steam Deck hardware.
>
{style="note"}

## What Vite is made of

Vite began as a fork of NvRTX 4.27 Caustics, which added DX12, ray tracing and rendering improvements over
Epic's standard 4.27 branch, along with DLSS, NVIDIA Reflex, improved denoisers and comprehensive ray
tracing support including DDGI-lit ray-traced reflections.

On top of that base:

- Epic's UE 4.27 Plus branch is fully merged.
- NVIDIA's NvRTX 5.0 branch is merged.
- Rendering features from AMD's engine branches are integrated.
- More than 300 backports from UE 5.0 through the 5.8 era are in the release branch, with over 1,200
  integrated in internal staging branches.

The integration work is done by engine programmers with extensive Unreal Engine source experience, using
proper code guards, managed shader permutations, and manual adaptation of each cherry-picked UE5 change to
the Vite codebase rather than blind merging.

With PhysX and a combined DDGI plus SSGI lighting pipeline, Vite closely resembles the bespoke Unreal
Engine build used for the launch of *The Finals*.

## Headline features

**[Dynamic DDGI](DDGI-Dynamic.md).** A noise-free global illumination alternative to Lumen. Higher quality
bounce and less light leaking than software Lumen, comparable to hardware Lumen for bounce, and typically
around twice the frame rate. DDGI implementations ship in Metro Exodus, Overwatch 2, The Finals, Control,
The Witcher 3, Warhammer 40,000: Darktide, DOOM: The Dark Ages, Indiana Jones and the Great Circle,
007 First Light, Ghost of Yotei and Star Wars Outlaws including its Switch 2 version.

**[Static DDGI](DDGI-Static.md).** A baked mode with near-instant bake times, higher bounce fidelity than
traditional baked lighting and better coverage of moving objects, viable on GPUs without ray tracing
support at all.

**[PhysX 3.4](PhysX.md).** Stable, commercially proven, and in Vite upgraded to build under newer
Clang versions for meaningful compiler optimisation gains. Internal stress tests show Chaos running over
five times slower than PhysX in physics-bound scenarios.

**[RTXDI](RTXDI.md).** A less noisy alternative to MegaLights, in its standalone form rather than the
Lumen-integrated version found in UE 5.1 and later NvRTX branches.

**[Tessellation](Tessellation.md).** Distance- and displacement-driven geometric detail without Nanite's
overhead.

**[Full ray tracing suite](Ray-Tracing.md).** Reflections, ambient occlusion, shadows, skylight,
translucency, caustics, direct lighting, per-pixel ray-traced GI and path tracing &mdash; the rendering
stack Black Myth: Wukong shipped on.

For the complete list, see [Release Notes](Release-Notes.md).

## Is UE4 not a deprecated codebase?

It is a fair question, and the answer is that Unreal Engine 4 continues to power recent AAA releases:
Final Fantasy VII Rebirth (4.26, 2024), Stellar Blade (4.26, 2024), Days Gone Remastered (4.11, 2025),
Delta Force (4.22, 2026), Mortal Kombat 1 (4.27, 2023), Mario &amp; Luigi: Brothership (4.26, 2024),
Princess Peach: Showtime! (4.26, 2024), Pikmin 4 (4.26, 2023), Square Enix's Dragon Quest VII Reimagined
(4.27, 2026) and the upcoming Final Fantasy VII: Revelation (4.27, 2027). All of them ship PhysX.

These productions stay on UE4 to retain specific features and meet fidelity and performance targets. UE4
also continues to receive updates from major studios through the 4.27 Plus branch, and remains a priority
for Nintendo platforms.

Vite's plan is to keep upgrading that codebase: optimise core systems, modernise the rendering core, improve
the UI and update the toolchains, rather than treat 4.27 as a frozen artifact. See
[Why NvRTX 4.27](Why-NvRTX-427.md) for the technical reasoning behind the base version choice.

## See also

- [Performance Targets](Performance-Targets.md)
- [Why NvRTX 4.27](Why-NvRTX-427.md)
- [UE4 versus UE5 Cost Analysis](UE4-Versus-UE5-Cost-Analysis.md)
- [Getting Started](Getting-Started.md)
