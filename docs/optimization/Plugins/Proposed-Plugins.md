# Proposed Plugins

<tldr>
<p>
Candidates for bundling into the engine, plus recommended plugins that projects should add themselves.
Everything here must be compatible with UE 4.21&ndash;4.27 &mdash; UE5-only plugins are out of scope.
</p>
</tldr>

Not everything useful belongs in the engine repository. A plugin bundled with Vite increases the size of
every clone, the length of every build and the surface area of every integration. This page tracks what has
been proposed and which tier it belongs in.

![](../../img/optimization/Plugins1.png)

*资源管理器视图显示引擎/插件/运行时/VitePlugins 目录下的集成插件文件夹*

*插件已实时集成到 `Engine\Plugins\Runtime\VitePlugins` 中。*


## Tiers

<deflist>
<def title="Engine repository">
Core functionality and native runtime. Small, fast to compile, useful to nearly everyone. These get
bundled &mdash; see <a href="Bundled-Plugins.md">Bundled Plugins</a>.
</def>
<def title="Staples">
Relevant to general game production, frequently used in AAA, representing major functionality. Worth
bundling if licensing and size allow; otherwise documented so teams can add them.
</def>
<def title="Common use">
Non-essential or high-level tooling, typically Blueprint extensions. Projects add these themselves.
</def>
</deflist>

## Staple plugins

### PhysX Instanced Subsystem &mdash; integrated, free

A world subsystem plus instanced actor workflow for managing large numbers of PhysX-backed instanced
bodies. Instead of spawning thousands of separate `AActor` / `UPrimitiveComponent` objects, you keep one or
a few instanced mesh actors for rendering while the subsystem creates and updates per-instance PhysX rigid
bodies, writing their poses back into ISM/HISM instance transforms.

Integrated at version 1.11. See [Instanced Physics](Instanced-Physics.md).

[Repository](https://github.com/Dragomirson/PhysXInstancedSubsystem) ·
[Demo project](https://drive.google.com/file/d/1NulunBP2Qre5vLyYnkiqywovsycNuWdQ/view)

### Houdini Engine &mdash; integrated, commercial/free use

Procedural workflow through Houdini Digital Assets. Artists adjust asset parameters interactively in the
editor and use Unreal assets as inputs; Houdini's procedural engine cooks the asset and the result appears
in the editor without baking.

[HoudiniEngineForUnreal, Houdini 20.0 / Unreal 4.27 branch](https://github.com/sideeffects/HoudiniEngineForUnreal/tree/Houdini20.0-Unreal4.27)

### Motion Symphony &mdash; integrated

Ubisoft-style motion matching and pose matching. Bundled at 1.09; see
[Bundled Plugins](Bundled-Plugins.md).

### PopcornFX &mdash; commercial/free use

Proprietary particle system used in several AAA titles including Blizzard games and racing titles. Version
2.18.6 is the latest fully 4.27-compatible release. Better GPU performance than Niagara.

[Repository](https://github.com/PopcornFX/UnrealEnginePopcornFXPlugin) ·
[v2.18.6 download](https://github.com/PopcornFX/UnrealEnginePopcornFXPlugin/archive/refs/tags/v2.18.6.zip)

A patched build by bunnyofficial fixes a compile error:
[patched version](https://drive.google.com/file/d/1hckpY_1zSLsW6mBqBlDeEgPVSijg9Z_y/view?usp=drive_link).

### Azure PlayFab &mdash; commercial/free use

Backend platform for live games: player authentication, data storage, matchmaking, multiplayer networking,
analytics and LiveOps such as events and A/B testing, on Azure infrastructure.

![](../../img/optimization/Plugins2.png)

*Azure PlayFab 后端服务概述*


[PlayFab Unreal Marketplace Plugin](https://github.com/PlayFab/UnrealMarketplacePlugin)

### Wwise &mdash; commercial/free use

The de facto standard audio middleware for games, with hundreds of shipped titles.

[Wwise Unreal integration](https://www.audiokinetic.com/en/public-library/2025.1.3_9039/?source=UE4&id=index.html)

### Asset Downgrader &mdash; paid

Downgrades assets to 5.6.1, 5.5.4, 5.4.4, 5.3.2, 5.2.1, 5.1.1, 5.0.3, 4.27 and 4.26. It first upgrades
assets to the source version (5.6), then applies patches to the `.uasset` files to make them compatible
with the target version, minus the newer data &mdash; Nanite data is stripped for a 4.27 downgrade, for
example.

<warning>
Features that do not exist in the older version cannot be ported: Nanite on masked materials, new material
nodes and new Niagara modules among them. The downgrader moves data, not capability.
</warning>

![](../../img/optimization/AssetDwongrader.png)

*Asset Downgrader 插件界面显示目标版本选择*

*Target versions the downgrader supports, from 5.6.1 down to 4.26.*

This is the practical route for using UE5 marketplace content in Vite. See
[Migrating from UE5](Migrating-From-UE5.md). The author, Ciprian Stanciu, is active on the Vite Discord and
has provided direct help on several Vite projects.

![](../../img/optimization/CiprianStanciu.png)

*Vite Discord 上的 Asset Downgrader 作者*


### HTN Planner &mdash; FAB / paid

Hierarchical Task Network AI framework for Unreal. Goal-driven strategy built by separating *what* to do
from *how* to do it. Version 1.18.3 was recently backported to 4.27.

![](../../img/optimization/HTN.png)

*Unreal 编辑器中的 HTN Planner 任务网络*


*A hierarchical task network separates the goal from the method used to reach it, which is what makes plans
composable across agents.*

[HTN plugin for Unreal Engine](https://maksmaisak.github.io/htn/front.html)

## Common use plugins

These are recommended but not bundled. Add them per project.

### FFMPEG Media Player

Support for more video formats and alpha videos than the stock media framework.

[bakjos/FFMPEGMedia](https://github.com/bakjos/FFMPEGMedia)

### UI Navigation

Blueprint-driven UI navigation for gamepad and keyboard menu traversal.

[goncasmage1/UINavigation, 4.27 branch](https://github.com/goncasmage1/UINavigation/tree/4.27_3.0)

### Multiplayer Movement (SMN2)

Blueprint multiplayer movement. Officially supported to 4.26 but compiles and works on 4.27, and can be
extended further. Deprecated upstream.

[Reddy-dev/SMN2](https://github.com/Reddy-dev/SMN2)

### Root Motion Source

Fully functional on 4.26 and 4.27, and effectively the 4.x equivalent of UE5's Motion Warping. In stock
UE4, root motion source is only correctly networked through GAS; this plugin overrides that and exposes
full root motion source functionality directly in Blueprints.

Field testing on a laggy network reports that following normal CMC logic gives clean results without
jitter or server-side corrections during root motion animations.

![](../../img/optimization/Plugins3.png)

*根运动源（Root Motion Source）蓝图节点*

[VJien/RootMotionSource](https://github.com/VJien/RootMotionSource) ·
[write-up](https://supervj.top/2022/03/24/RootMotionSource/?highlight=root+motion+source)

## Proposing a plugin

Before proposing, check:

| Criterion | Requirement |
|---|---|
| Engine version | Compatible with 4.21&ndash;4.27. UE5-only is out of scope. |
| Licensing | Redistributable, or clearly documented as something users obtain themselves |
| Size | Bundled plugins are cloned by everyone. Large binary dependencies need justification. |
| Compile cost | Adds to every engine build. See [Shader Compilation and PSO](Shader-Compilation-And-PSO.md). |
| Overlap | Does it duplicate something already bundled or stock? |

Then say which tier you think it belongs in and why.

## See also

- [Bundled Plugins](Bundled-Plugins.md)
- [Plugins](Plugins.md)
- [Migrating from UE5](Migrating-From-UE5.md)
- [Contributing](Contributing.md)
