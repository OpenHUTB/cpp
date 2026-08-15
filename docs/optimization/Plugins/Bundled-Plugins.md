# Bundled Plugins

<tldr>
<p>
Everything Vite adds on top of stock UE 4.27, with versions and default enablement. Stock 4.27 plugins are
not listed &mdash; only Vite's additions and the vendor integrations.
</p>
</tldr>

Versions are as of the July major integration. Check the `.uplugin` file for the authoritative value in
your tree.

<img src="VitePlugins.png" alt="Contents of Engine/Plugins/Runtime/VitePlugins in Explorer" border-effect="line"/>

*Vite's own additions live under `Engine\Plugins\Runtime\VitePlugins`. Vendor integrations from NVIDIA sit
separately under `Engine\Plugins\Runtime\Nvidia`.*

## Upscaling and frame generation

| Plugin | Version | Default | Path |
|---|---|---|---|
| NVIDIA DLSS Super Resolution / Ray Reconstruction / DLAA | 8.7.0 (NGX 310.7.0) | Off | `Runtime\Nvidia\DLSS` |
| NVIDIA DLSS Frame Generation (Streamline) | 1.3.0-SL2.4.0 | Off | `Runtime\Nvidia\Streamline` |
| NVIDIA NIS | 1.2.1 | Off | `Runtime\Nvidia\NIS` |
| NVIDIA RTX Dynamic Vibrance (DeepDVC) | 1.3.0-SL2.4.0 | Off | `Runtime\Nvidia\StreamlineDeepDVC` |
| AMD FSR 4 | 4.1.1 | &mdash; | `Runtime\VitePlugins\FSR4-427` |
| AMD AntiLag 2 | 2.0.4 | &mdash; | `Runtime\VitePlugins\AntiLag2.0.4` |
| Intel XeSS | 3.0.5 | &mdash; | `Runtime\VitePlugins\XeSS_UE4.27_Plugin_v3.0.5` |
| Movie Render Queue DLSS/DLAA support | 2.3.3 | &mdash; | `Runtime\Nvidia\DLSSMoviePipelineSupport` |

FSR 4 uses the native ffx-api and is DX12-only. It was backported to 4.27 specifically for Vite. Full
detail in [Upscalers](Upscalers.md).

## Ray tracing and rendering

| Plugin | Version | Default | Path |
|---|---|---|---|
| NVIDIA RTX Global Illumination (RTXGI) | 1.1.50 | **On** | `Runtime\Nvidia\RTXGI` |
| NVIDIA Real-Time Denoiser (NRD/ReLAX) | 2.10.00-relax | **On** | `Runtime\Nvidia\NRD` |
| NVIDIA Reflex | &mdash; | &mdash; | `Runtime\Nvidia\Reflex` |
| NVIDIA Ansel | &mdash; | &mdash; | `Runtime\Nvidia\Ansel` |
| Graphics Card Info Utilities | 1.0 | Off | `Runtime\Nvidia\GraphicsCardInfoUtils` |

RTXGI provides the DDGI implementation that is Vite's recommended dynamic GI solution. NRD/ReLAX denoises
ray-traced output. Both being on by default is deliberate: they are the basis of Vite's recommended
lighting setup. See [Global Illumination](Global-Illumination.md) and
[Dynamic DDGI](DDGI-Dynamic.md).

## Hair

| Plugin | Version | Default | Path |
|---|---|---|---|
| TressFX 5.0 | 5.0 | &mdash; | `Runtime\TressFX` |
| Groom (HairStrands) | 1.0 | Off | `Runtime\HairStrands` |

Two independent hair systems with different authoring pipelines. See
[Hair Rendering](Hair-Rendering.md).

## Physics and destruction

| Plugin | Version | Default | Path |
|---|---|---|---|
| Blast (runtime) | 1.0 | Off | `GameWorks\Blast` |
| Blast Plugin (authoring) | 0.1 | Off | `Experimental\BlastPlugin` |
| PhysX Instanced Subsystem | 1.11 | &mdash; | `Runtime\VitePlugins\PhysXInstancedSubsystem` |
| Kawaii Physics | 1.18.0 | &mdash; | `Runtime\VitePlugins\KawaiiPhysics` |

Apex Destruction, Apex Cloth and PhysX Vehicles are stock 4.27 plugins that Vite retains &mdash; they were
removed in UE5 when Chaos replaced PhysX. See [Destruction and Cloth](Destruction-And-Cloth.md).

Kawaii Physics is a UE5 secondary-motion bone solver backported to Vite. It gives hair, cloth and
accessories physical follow-through from a single animation node, far more cheaply than a full cloth
simulation.

The PhysX Instanced Subsystem manages large numbers of PhysX-backed instanced bodies through a world
subsystem, writing poses back into ISM/HISM instance transforms rather than spawning individual actors.
See [Instanced Physics](Instanced-Physics.md).

## Animation

| Plugin | Version | Path |
|---|---|---|
| Animation Compression Library (ACL) | 2.1.0 | `Runtime\VitePlugins\ACLPlugin` |
| Motion Symphony | 1.09 | `Runtime\VitePlugins\MotionSymphony` |

ACL replaces Unreal's built-in animation compression with a substantially better size/quality curve. On a
project with a large animation set the memory saving is significant, and it is one of the cheapest wins
available &mdash; see [Performance Targets](Performance-Targets.md).

Motion Symphony provides motion matching and pose matching, the Ubisoft-style approach to animation
synthesis. It is the closest 4.27 equivalent to UE5's Motion Matching.
[Documentation](https://www.wikiful.com/@AnimationUprising/motion-symphony/motion-matching) ·
[Sample project](https://github.com/Animation-Uprising/MotionSymphony_ExampleProject/tree/main)

## Gameplay

| Plugin | Version | Path |
|---|---|---|
| Abilities (SplashAbilities) | 1.1 | `Runtime\VitePlugins\SplashAbilities` |
| Flecs ECS | 1.0 (Flecs 3.2.12) | `Runtime\FlecsECS` |

Abilities is a lighter alternative to `GameplayAbilities` for projects that want an ability system without
GAS's complexity and replication model.

Flecs ECS is an engine-level integration of the Flecs entity component system, providing a world subsystem,
Blueprint entity handles, an ISM-based rendering demo and optional Flecs Explorer support. It is off by
default. Useful when actor-per-entity overhead is the bottleneck &mdash; see the
[UE4 vs UE5 cost analysis](UE4-Versus-UE5-Cost-Analysis.md) on core class base costs.

## Debug and profiling

| Plugin | Version | Path |
|---|---|---|
| Dear ImGui | 0.1.0 | `Runtime\VitePlugins\UnrealImGui` |
| ImGui Tools | 1.0 | `Runtime\VitePlugins\ImGuiTools` |
| Intel GPA | 1.0 | `Runtime\VitePlugins\GPAPlugin` |
| Automatron | 1.1a | `Runtime\VitePlugins\Automatron` |

ImGui gives you immediate-mode debug UI that works in the viewport and in standalone builds, where Slate
debug widgets are awkward. ImGui Tools builds functional development tools on top of it.

The GPA plugin integrates Intel Graphics Performance Analyzers into the editor. Automatron improves
automated testing for C++ and Blueprints. See [Profiling](Profiling.md).

## Content and utilities

| Plugin | Version | Default | Path |
|---|---|---|---|
| Custom Splash Preload Screen | 1.0 | **On** | `Runtime\PostSplashScreen` |
| Impostor Baker | 1.0 | &mdash; | `Experimental\ImpostorBaker` |
| Shallow Water | 1.0 | Off | `Experimental\ShallowWater` |

The splash screen plugin displays a custom screen after the system splash during engine preinit, covering
the gap before the first frame.

Impostor Baker generates impostors for use as distant mesh LODs &mdash; the standard technique for
rendering large numbers of distant trees or props cheaply, and worth reaching for before accepting a draw
call cost you cannot afford.

## Default enablement summary

Only three bundled plugins are on by default:

| Plugin | Why |
|---|---|
| RTXGI | Provides DDGI, Vite's recommended dynamic GI |
| NRD | Denoises ray-traced output, needed by the ray tracing suite |
| Custom Splash Preload Screen | Cosmetic, negligible cost |

Everything else is opt-in. If a feature appears not to work, check that its plugin is enabled before
debugging further &mdash; and for ray tracing features, check the
[compile-time switch availability table](Ray-Tracing.md) as well.

## See also

- [Plugins](Plugins.md)
- [Proposed Plugins](Proposed-Plugins.md)
- [Upscalers](Upscalers.md)
- [Hair Rendering](Hair-Rendering.md)
- [Instanced Physics](Instanced-Physics.md)
- [Debloat Guide](Debloat-Guide.md)
