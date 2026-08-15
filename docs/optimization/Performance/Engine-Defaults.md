# Engine Default Changes

<tldr>
<p>
Vite changes a number of stock Unreal Engine 4.27 defaults for performance. These changes affect your
project directly &mdash; some of them change runtime behaviour, not just cost. Read this page before
concluding that something is broken.
</p>
</tldr>

Stock Unreal defaults are chosen to make everything work out of the box, which means they enable features
most projects never use. Vite changes several of them to favour performance, on the principle that a
feature you want should be something you turn on rather than something you forget to turn off.

The trade is that a project moved from stock 4.27 to Vite may behave differently. This page is the list.

## Runtime behaviour changes

<warning>
These change behaviour, not just performance. If gameplay logic depends on the stock default, it will need
attention.
</warning>

### Overlap events disabled by default

Primitive components no longer generate overlap events unless explicitly enabled.

Overlap event generation costs on every component that has it, whether or not anything is bound to the
event. In a stock project the majority of primitives generate overlap events that nothing listens to.

**What this means for you:** components that need overlap events must set **Generate Overlap Events**
explicitly. Trigger volumes, pickup detection and anything driven by `OnComponentBeginOverlap` need the
flag set. This is the most likely source of "my trigger stopped working" after migrating a project.

<img src="../../img/optimization/OverlapEventsDisabled.png" alt="Commit diff replacing SetGenerateOverlapEvents(true) with bGenerateOverlapEvents = false in PrimitiveComponent.cpp" border-effect="line"/>

*One line in `UPrimitiveComponent`, applied to every primitive in every project.*

[Commit](https://github.com/GapingPixel/UE5-PhysX-Vite/commit/02d7c0ad0a7542b382a70dc3e37877d6ec052d76)

### Optimised actor runtime

Actor runtime defaults are changed to reduce per-actor overhead in `AActor::InitializeDefaults`:

| Default | Stock 4.27 | Vite | Why |
|---|---|---|---|
| `SetCanBeDamaged` | `true` | `false` | Only actors that use the damage system need it |
| `bRelevantForNetworkReplays` | `true` | `false` | Keeps actors out of demo net recording unless wanted |
| `bRelevantForLevelBounds` | `true` | `false` | Avoids level-bounds iteration over actors that do not define bounds |

<img src="../../img/optimization/OptimizedActorRuntime.png" alt="Commit diff in Actor.cpp changing SetCanBeDamaged, bRelevantForNetworkReplays and bRelevantForLevelBounds defaults" border-effect="line"/>

*Large meshes, blocking volumes and foliage that must define world bounds need `bRelevantForLevelBounds`
set back to `true`.*

[Commit](https://github.com/GapingPixel/UE5-PhysX-Vite/commit/970cbb989c3712f13be1fa370b778e769e5d864c)

### Skeletal mesh optimised configuration

Skeletal meshes are among the top CPU offenders in most projects, so `USkeletalMeshComponent` ships with
performance-oriented defaults instead of Epic's fully featured ones. In-world assets often contain many
skeletal meshes, which makes enabling optimised settings per component impractical; configuring the cheap
path as the default and opting into the expensive options where they are needed is the workable order.

| Default | Stock 4.27 | Vite |
|---|---|---|
| `VisibilityBasedAnimTickOption` | `AlwaysTickPoseAndRefreshBones` | `OnlyTickPoseWhenRendered` |
| `bEnableUpdateRateOptimizations` | `false` | `true` |
| `bHasCustomNavigableGeometry` | `Yes` | `No` |
| `bDisablePostProcessBlueprint` | `false` | `true` |
| `bUpdateOverlapsOnAnimationFinalize` | `true` | `false` |

<img src="../../img/optimization/SkeletalMeshesOptimizedConfig.png" alt="Commit diff in SkeletalMeshComponent.cpp showing the five changed defaults against Epic's originals" border-effect="line"/>

*Each changed line keeps Epic's original value in a trailing comment, so the stock behaviour is recoverable
without consulting upstream.*

`VisibilityBasedAnimTickOption` is the one to watch. `OnlyTickPoseWhenRendered` means an off-screen
character stops evaluating its pose entirely; gameplay that reads bone transforms or sockets on unrendered
characters &mdash; weapon muzzle positions, IK targets, attach points on a distant actor &mdash; must set
the component back to `AlwaysTickPose` or `AlwaysTickPoseAndRefreshBones`.

`bDisablePostProcessBlueprint = true` is the second: post-process anim Blueprints, commonly used for IK
and bone corrections, no longer run unless re-enabled per component.

<img src="../../img/optimization/SkeletalMeshDefault.png" alt="SkeletalMeshComponent.cpp constructor showing the surrounding default block" border-effect="line"/>

*The surrounding constructor block in `SkeletalMeshComponent.cpp`, for context on where these defaults are
set.*

[Commit](https://github.com/GapingPixel/UE5-PhysX-Vite/commit/33fe7c638829b8120e8a02ecc639acda761df835)

### Generate Lightmap UVs disabled

Static mesh import no longer generates lightmap UVs by default.

Most Vite projects use [DDGI](DDGI-Dynamic.md) rather than baked lighting, so generating lightmap UVs for
every imported mesh is wasted import time and wasted UV channels. If your project bakes lighting, enable
the setting in the static mesh import options.

[Commit](https://github.com/GapingPixel/UE5-PhysX-Vite/commit/acce6fbe432fe9fe5a2536f0979befd5dcd741d1)

## Scalability

Shadow Quality 4 is used for the Medium shadow setting, raising shadow quality in the middle of the
scalability range.

[Commit](https://github.com/GapingPixel/UE5-PhysX-Vite/commit/1718cd66e08a2b47222f895162e3be2b2c98ee6a)

## Disabled plugins

A number of plugins that ship enabled in stock 4.27 are disabled by default. This reduces editor startup
time, module load count and packaged build size.

**VR plugins in particular must be re-enabled if you need them.**

[Commit 1](https://github.com/GapingPixel/UnrealEngineVite-PhysX/commit/e9aebc2ef9f8acb7326a7e989f288ef68969342f) &middot;
[Commit 2](https://github.com/GapingPixel/UE5-PhysX-Vite/commit/6c5948bf12ea61f82784193ccaa5d43c4f574ae9)

## Tick optimisations

### SpeedTree tick

The SpeedTree tick in `LevelTick.cpp` is optimised. SpeedTree ticking runs regardless of whether a project
uses SpeedTree assets, so this is a saving every project gets.

<img src="SpeedTreeTick.png" alt="LevelTick.cpp showing the UpdateSpeedTreeWind call inside the world tick" border-effect="line"/>

*`Scene->UpdateSpeedTreeWind` in the world tick &mdash; unconditional in stock 4.27.*

[Source](https://github.com/GapingPixel/UE5-PhysX-Vite/blob/3e4a16aa89de4f4c37da300c945d6a14dc62edd7/Engine/Source/Runtime/Engine/Private/LevelTick.cpp#L1709)

### Niagara tick

Niagara ticks whenever the plugin is enabled. Disable the Niagara plugin at project level if you do not use
it; Cascade remains available in 4.27.

## Ray tracing culling

Ray tracing culling respects each primitive's minimum draw distance:

```c++
GEngine->Exec(nullptr, TEXT("r.RayTracing.Culling.UseMinDrawDistance 1"));
```

This is a cheap and generally safe win in scenes with many small detail meshes, since geometry too small to
be drawn is also too small to matter in the acceleration structure.

[Commit](https://github.com/GapingPixel/UE5-PhysX-Vite/commit/595f376f6de0912606b05768739aa3d24ac4f61a)

## Editor quality-of-life

These do not affect runtime performance but change editor behaviour:

- Animation assets always open in a new tab
  ([commit](https://github.com/GapingPixel/UE5-PhysX-Vite/commit/8bdae919e6eae61a27ef8d09d38027592a473d8c))
- A config variable to disable the new plugins popup
  ([commit](https://github.com/GapingPixel/UE5-PhysX-Vite/commit/9af0349c1832b7094ceae7f47ba8ca5d261e0e69))
- `bDisableAllTutorialAlerts=True`
  ([commit](https://github.com/GapingPixel/UE5-PhysX-Vite/commit/d9fd593e11581fd93d0ff60b2933794f150b4780))
- Assorted safe backports from later engine versions
  ([commit](https://github.com/GapingPixel/UE5-PhysX-Vite/commit/fe7a6f4d7c54d8725d6308820d5b4fd546b9ff49))

## Optional debloat you can do yourself

These cannot be disabled in the fork release branch for compatibility reasons, but are available to
individual projects.

**Vite plugin removal.** Removing the added Vite plugins improves compile times. Measured on a Ryzen 9
9950X3D: full repository 15 minutes, without Vite plugins 12 minutes. Houdini is the largest single
contributor.

See the [Debloat Guide](Debloat-Guide.md) for the tooling.

## Migrating an existing project

<procedure title="Check a migrated project against Vite defaults" id="check-defaults">
    <step>
        Test every trigger volume and overlap-driven interaction. Overlap events are the most common
        breakage.
    </step>
    <step>
        Check characters using leader/follower skeletal mesh setups for missing curve propagation.
    </step>
    <step>
        If the project bakes lighting, re-enable lightmap UV generation and confirm existing meshes still
        have valid lightmap UVs.
    </step>
    <step>Re-enable any plugins your project needs, VR in particular.</step>
    <step>Review scalability settings, since Medium shadows now differ from stock.</step>
</procedure>

## See also

- [Migrating from UE5](Migrating-From-UE5.md)
- [Debloat Guide](Debloat-Guide.md)
- [Profiling](Profiling.md)
- [Compile-Time Switches](Compile-Time-Switches.md)
