# Migrating from Unreal Engine 5

<tldr>
<p>
Assets move down with the <a href="Proposed-Plugins.md">UE Downgrader</a> plugin, which supports UE 5.8 and
below back to 4.27. Code moves more easily than on stock 4.27, because Vite backports many UE5 container,
game framework and GAS APIs. Features that do not exist in 4.27 &mdash; Nanite, Lumen, VSM, World Partition
&mdash; have no equivalent and need design decisions, not conversion.
</p>
</tldr>

Moving a project from Unreal Engine 5 down to Vite is a real migration, not a version bump. This page sets
out what transfers cleanly, what needs replacing, and in what order to do it.

## What transfers, what does not

| UE5 feature | Status on Vite | What to do |
|---|----------------|---|
| Static meshes, textures, materials, animations | Transfers      | Downgrade with the UE Downgrader plugin |
| Blueprints | Transfers      | Downgrade; re-test node-for-node, some UE5-only nodes have no target |
| C++ gameplay code | Transfers      | Vite backports many UE5 APIs; see below |
| Gameplay Ability System | Transfers      | Vite includes GAS updates backported from UE5 |
| Nanite | No equivalent  | Author conventional LODs; use [Tessellation](Tessellation.md) for surface detail |
| Lumen | Replaced       | Use [DDGI](DDGI-Dynamic.md), optionally with [SSGI](SSGI.md) |
| Virtual Shadow Maps | No equivalent  | Cascaded shadow maps, or [ray-traced shadows](RT-Shadows-And-Ambient-Occlusion.md) |
| TSR | Replaced       | [DLSS, FSR, XeSS](Upscalers.md), or native with [SMAA](Anti-Aliasing.md) |
| MegaLights | Replaced       | [RTXDI](RTXDI.md) |
| Chaos physics | Replaced       | [PhysX](PhysX.md) |
| Chaos Destruction | Replaced       | [Apex Destruction](Destruction-And-Cloth.md) and [Blast](Destruction-And-Cloth.md) |
| Chaos Cloth | Replaced       | [Apex Cloth](Destruction-And-Cloth.md) |
| Substrate | No equivalent  | Standard material model, plus [Callisto BRDF](Shading-Models.md) |
| World Partition | Replaced       | World Composition and level streaming |
| Niagara | Transfers      | Available; [PopcornFX](Proposed-Plugins.md) is a faster alternative |
| MetaSounds | No equivalent  | Sound Cues, or [Wwise](Proposed-Plugins.md) |

## Downgrading assets

The UE Downgrader plugin converts assets from UE 5.8 and below back to 4.27 and 4.26. It works by first
upgrading assets to its source version(usually latest UE5), then applying patches to the `.uasset` files so they are readable by
the target version, minus the data the older format cannot represent. Nanite data, for instance, is stripped
during a 4.27 downgrade.

What the plugin cannot do is invent 4.27 equivalents for features that do not exist there. Nanite on masked
materials, new material nodes and new Niagara modules will not survive the trip. Budget for reauthoring
those.

The plugin is commercial. Its author, Ciprian Stanciu, is active on the
[Vite Discord](https://discord.gg/n9zQrYFhMb) and has provided direct support for several Vite projects.
There is a [video walkthrough](https://youtu.be/yXvJfDNfrSQ) of the workflow.

## Migrating code

This is the part that is easier on Vite than on stock 4.27. The fork deliberately backports UE5 API surface
so that UE5-era code compiles with fewer changes: updated container classes, game framework updates, and
Gameplay Ability System functionality are all present.

You will still hit differences. Work through them in this order:

1. **Physics.** Anything touching `Chaos` namespaces, `FChaosScene`, geometry collections or Chaos vehicles
   needs to move to PhysX equivalents. This is usually the largest single chunk of work. See
   [PhysX Overview](PhysX.md).
2. **Rendering.** Code that queries or drives Lumen, Nanite or VSM console variables has no target. Replace
   with the Vite equivalents in [Rendering](Rendering.md).
3. **Core API drift.** `FVector` is double-precision in UE5 (`FVector3d`) and single-precision in 4.27.
   Large-world-coordinate assumptions do not hold. Audit any code doing precision-sensitive maths at large
   world offsets.
4. **Module and build rules.** `.Build.cs` files referencing UE5-only modules need those dependencies
   removed or replaced.

## Recommended migration order

<procedure title="Migrate a UE5 project to Vite" id="migration-order">
    <step>
        Stand up an empty Vite project first and confirm your toolchain and engine build are healthy.
        Do not debug two problems at once.
    </step>
    <step>
        Port the C++ modules with no content. Get them compiling against Vite before any assets move.
        Check physics and rendering API breakage here.
    </step>
    <step>
        Downgrade and import a small, representative slice of content &mdash; one character, one
        environment, a handful of materials. Validate that it looks and behaves correctly.
    </step>
    <step>
        Replace the lighting setup. Lumen has no direct translation; set up
        <a href="DDGI-Dynamic.md">DDGI</a> volumes and tune them against the slice you just imported.
    </step>
    <step>
        Rebuild the physics setup. Re-author destruction with
        <a href="Destruction-And-Cloth.md">Apex Destruction</a> and cloth with
        <a href="Destruction-And-Cloth.md">Apex Cloth</a>.
    </step>
    <step>
        Bulk-migrate the remaining content once the slice is proven.
    </step>
    <step>
        Profile against your target hardware and set scalability. See
        <a href="Profiling.md">Profiling and Benchmarking</a>.
    </step>
</procedure>

## A mixed-version workflow

You do not have to move everyone at once. A common arrangement is for programmers to work in Vite while
artists and content creators continue in a stock Epic Games Launcher install of 4.27, with content flowing
one way. For that to work, the launcher-side users need the standalone
[DDGI 1.1.5 plugin](https://github.com/GapingPixel/UE4-RTXGI-1.1.5-Latest-Official) so that lighting looks
approximately right on their end. This is exactly what we do at Vite Studio, and has work so far with no issues
for modeling, animation and sound design work.

> Do not use the launcher DDGI plugin into a Vite project. Vite already ships DDGI as part of the
> engine, and the two will conflict. *DDGI in Vite is handled better than DDGI plugin for Launcher Version.
> (Due to both Engine side/DDGI plugin Side changes)
>
{style="warning"}

## See also

- [Introduction to Vite](Introduction-to-Vite.md)
- [PhysX Overview](PhysX.md)
- [Global Illumination](Global-Illumination.md)
- [Proposed Plugins](Proposed-Plugins.md)
- [UE4 versus UE5 Cost Analysis](UE4-Versus-UE5-Cost-Analysis.md)
