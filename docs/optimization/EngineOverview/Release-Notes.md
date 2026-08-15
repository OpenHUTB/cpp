# Release Notes

<tldr>
<p>
Current release branch: <code>ueVite26-JulyMajor-release</code>. Staging work lands in
<code>ueVite26-JulyStaging</code> before promotion. Feature branches such as
<code>ueVite26-HairWorks</code> and <code>ueVite26-JulyRtSettings</code> are merged when stable.
</p>
</tldr>

This page tracks what is in the current major release, what is actively in progress, and what is planned.
Feature lists are cumulative &mdash; everything under "Shipped" is present in the current release branch.

## Branch layout

| Branch | Purpose |
|---|---|
| `ueVite26-JulyMajor-release` | Current major release. This is what you should build against. |
| `ueVite26-JulyStaging` | Integration branch. Backports and features land here first. |
| `ueVite26-JuneMajorRelease` | Previous major release, kept for projects mid-milestone. |
| Feature branches | Work in progress on a single system, merged when stable. |

Work-in-progress changes belong on a branch, not on release. See
[Commit and PR Conventions](Commit-Conventions.md).

## Shipped

### Physics

- **Vite PhysX 3.4** as the shipping physics backend.
- **Library upgrade** for compatibility with newer Clang versions and the latest Android NDK Clang,
  producing a meaningful performance increase &mdash; up to 2x in stress tests, typically around 1.4x
  faster in the Box Container Pile 10 test.
- **PhysX Blast** working alongside Apex Destruction.
- **PhysX Fixed Timestep** as an opt-in feature, fully guarded for zero overhead on the regular path.

### Rendering

- **Callisto BRDF** shading model: single- and dual-lobe GGX specular with specular Fresnel falloff.
- **Toon** shading model, inspired by Guilty Gear.
- **HBAO+** ambient occlusion.
- **Compute-based SMAA**.
- Improved **FXAA** with a higher image quality option.
- Updated **TAA** for better temporal resolve.
- **DLSS 4.5**, **FSR 2**, **FSR 4**, **XeSS**, **NIS** and **AMD Anti-Lag 2** ported and integrated.
- Engine-side DLSS rendering support improvements.
- Improved **DDGI**, including performance work on RT reflections.
- **SSAO fast path** and a significantly cheaper SSAO.
- **TressFX** hair integration.
- Localized IBL.
- Improved **ACES** colour reproduction.
- Rendering optimisations across RHI, RT direct lighting, RT shadows, geometry collections, drawing,
  eye adaptation and shadow/light draw distance, plus AMD optimisations targeted at consoles.
- **Software occlusion**, which was removed after 4.27 upstream.
- Nintendo Switch specialised renderer base, which UE 5.0+ removed.

### CPU and core

- CPU optimisations for containers, friends usage, hash maps, NavMesh, volumetric clouds, the game thread,
  the task graph, SIMD, animation systems, texture handling, streaming and audio systems.
- Optimised runtimes for skeletal meshes and actors.
- Updated classes for easier backporting of UE5 codebases (game framework and containers).
- Gameplay Ability System updates.
- Oodle updates.
- Modernised console systems.

### Build, tooling and content

- Full merge of UE 4.27 Plus, NvRTX 4.27, NvRTX 5.0, DLSS, TressFX, FSR and AMD patches.
- Over **300 backports** from UE 5.0 through the 5.8 era.
- MSVC toolchain compliance up to **14.50 & SDK 10.0.26100 (Visual Studio 2026)**.
- Shader compilation improvements.
- Debloated **runtime PSOs** in Shipping configuration.
- Editor loading improvements and editor quality-of-life backports.
- Project default plugin debloat.
- Batch tooling to produce installed engine builds and debloat the tree.
- Bundled plugins: FSR, Motion Symphony, Houdini, ACL, Kawaii Physics, PhysX Instanced Subsystem,
  Splash Damage Ability System, ImGui and ImGui Tools, and others. See
  [Bundled Plugins](Bundled-Plugins.md).
- **ImGui** integration with benchmarking tools.
- Large volume of toolchain updates, C++ modernisation, faster cooking, engine fixes, resolved memory leaks
  and stall fixes.
- Mobile improvements and fixes.

## In progress

- Flat UI redesign, closer to UE5's visual language.
- Integration of Multi-Threaded FLECS ECS library with Unreal Actors interaction.
- Full C++20 support.
- Further UE5 backport integration &mdash; roughly 1,000 backports pending promotion to release.
- Rendering features: improved mesh handling, GI, shading models, ambient occlusion and specular aliasing
  handling.
- Further ACES upgrades, colour space, HDR handling and tonemappers.
- Large level optimisations.
- Further shader compilation improvements.
- CACAO ambient occlusion.
- PhysX Flex and Flow: GPU-accelerated particles across vendors, with AMD compute and NVIDIA CUDA paths.
- Tessellated water for ocean rendering, integrated into the RT scene for reflections.
- Improved performance for RTAO and RT shadows.
- Engine-level multi-threaded tick aggregation for improved instruction coherency.
- Core C++ library upgrades and a core engine math upgrade.
- Larger ray tracing rendering changes.

## Planned

- [AMD Single Pass Downsampler](https://github.com/GPUOpenSoftware/UnrealEngine/tree/FidelityFXSPD-4.26/UnrealEngine).
- Improved SSGI.
- Bespoke Level Editor

## Tracking

| Resource | Link |
|---|---|
| Work plan | [Trello board](https://trello.com/b/JKyBFS5X/ue-vite-physx-vite-studio-fork) |
| Backport tracker | [GitHub project](https://github.com/users/GapingPixel/projects/1/views/1) |
| Sample projects | [ViteStudio-Tech](https://github.com/ViteStudio-Tech) |

## See also

- [Engine Overview](Engine-Overview.md)
- [Backporting Workflow](Backporting.md)
- [Bundled Plugins](Bundled-Plugins.md)
- [Proposed Plugins](Proposed-Plugins.md)
