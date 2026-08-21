# Why NvRTX 4.27

<tldr>
<p>
Unreal Engine 4.27 is the last iteration with an <b>engine-agnostic ray tracing pipeline</b>, closer in
design to other AAA engines. From UE 5.1 onward, BLAS/TLAS management, culling and ray hit shading became
progressively coupled to Lumen, Nanite, VSM and GPU Scene, and the PhysX API was removed.
</p>
</tldr>

Choosing a base version is the single most consequential decision in a long-lived engine fork. This page
sets out why Vite is built on NvRTX 4.27 Caustics rather than a UE5 branch.

## The ray tracing pipeline argument

Unreal Engine 4.27 has an agnostic ray tracing pipeline. Acceleration structures are built and updated in a
way that is not tied to any particular lighting solution, so a third-party GI or reflection technique can be
integrated by hooking into the scene representation the renderer already maintains.

From UE 5.1 onward, the default rendering path &mdash; including ray tracing scene construction and update
&mdash; became increasingly integrated around Lumen, Nanite, Virtual Shadow Maps and Temporal Super
Resolution. Concretely:

- **BLAS/TLAS management** was adapted to support GPU Scene, Nanite fallback meshes and streamed ray
  tracing geometry.
- **Culling** was reworked around the same systems.
- **Ray hit shading** increasingly relied on Lumen's separate Surface Cache and mesh-card representation
  rather than shading the actual hit surface.

Each of these is a reasonable decision in service of Lumen. Together they mean that integrating an
alternative RTGI or reflection technique stops being a matter of consuming the scene and becomes a matter of
fighting the scene. That is the concrete reason DDGI integrates cleanly into 4.27 and awkwardly into 5.x.

In parallel, as PhysX was deprecated in favour of Chaos, the remaining PhysX-specific APIs and compatibility
code were gradually removed &mdash; which independently rules out the UE5 branches for a fork whose physics
argument depends on PhysX.

> For detailed reasoning and proofs, see the Technical Reference document on moving Vite base to UE4 Latest.
> [4.27 versus 5.0](https://docs.google.com/document/d/1gA0MGkzeWWzKkgwBDOP5xRPouSKaOIW6xlPZ2q6BXO0/edit?usp=sharing).
>
{style="note"}

## Why the NvRTX Caustics branch specifically

NvRTX 4.27 Caustics is NVIDIA's ray tracing branch of Unreal Engine 4.27.1. On top of Epic's 4.27 it adds:

- DirectX 12 and DXR improvements, and a range of ray tracing performance optimisations tested against
  real Marketplace content rather than synthetic scenes.
- DLSS, NVIDIA Reflex and improved denoisers.
- Ray-traced mesh caustics and water caustics.
- Enhanced translucency, including hybrid translucency modes that let rasterised and ray-traced
  translucency coexist.
- Multi-bounce refraction optimisation with absorption and total internal reflection.
- ReStir GI with a new SVGF-based denoiser, emissive material support and a reservoir-resampling final
  gather.
- Engine-side DDGI upgrades, including DDGI-lit ray-traced reflections.

That last item matters more than it sounds. The stock 4.27 launcher DDGI plugin is a plugin; the NvRTX
integration reaches into the ray tracing pipeline, which is what makes probe-based ray-traced reflections
possible. See [DDGI Dynamic](../Rendering/DDGI-Dynamic.md).

Starting from a branch that already had this work done saved the fork a very large amount of integration
effort, and meant the ray tracing features were already validated against shipped NVIDIA sample content.

## What has been merged on top

Vite is not frozen at its base. The following are fully merged:

| Source | What it contributes                                                                   |
|---|---------------------------------------------------------------------------------------|
| Epic UE 4.27 Plus | Ongoing EpicGames fixes & updates to 4.27, plus the last toolchain compliance updates |
| NvRTX 5.0 | Further NVIDIA rendering work backported down                                         |
| AMD GPUOpen engine branches | FSR, AMD-specific rendering optimisations highly relevant to console GPUs             |
| UE 5.0&ndash;5.8 | 300+ backports in release; 1,000+ at internal staging                                 |

Backports are generally not straight cherry-picks. They are adapted properly to Vite codebase, 
by engineers with several years of experience with Unreal Engine private forks. 
[Backporting Workflow](../Contributing/Backporting.md) page documents the process.

## The counter-argument, and the answer

The obvious objection is that Unreal Engine 4 is a dead codebase. In practice it is not. Recent and upcoming
AAA releases on UE4 include Stellar Blade (4.26, 2024),
Days Gone Remastered (4.11, 2025), Delta Force (4.22, 2026), Mortal Kombat 1 (4.27, 2023),
Mario &amp; Luigi: Brothership (4.26, 2024), Princess Peach: Showtime! (4.26, 2024), Pikmin 4 (4.26, 2023), 
and Square Enix's Final Fantasy VII Rebirth (4.26, 2024)
Dragon Quest VII Reimagined (4.27, 2026) and Final Fantasy VII: Revelation (4.27, 2027).

All of them ship PhysX. These teams stayed on UE4 to retain specific features and hit fidelity and
performance targets &mdash; the same reasoning Vite is built on. UE4 also continues to receive updates from
major studios via the 4.27 Plus branch, and remains a priority target for Nintendo platforms.

The difference between "using a deprecated codebase" and "maintaining a fork" is whether anyone is still
improving it. Vite's plan is continued optimisation of core systems, rendering core modernisation, UI work
and toolchain updates &mdash; see [Release Notes](Release-Notes.md) for what that has produced so far.

## See also

- [UE4 versus UE5 Cost Analysis](UE4-Versus-UE5-Cost-Analysis.md)
- [Performance Targets](Performance-Targets.md)
- [Ray Tracing](../Rendering/Ray-Tracing.md)
- [PhysX Overview](../Physics/PhysX.md)
- [Backporting Workflow](../Contributing/Backporting.md)
