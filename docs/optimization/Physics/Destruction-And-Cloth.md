# Destruction and Cloth

<tldr>
<p>
Vite keeps <b>Apex Destruction</b> and <b>Apex Cloth</b>, both removed in Unreal Engine 5, and adds
NVIDIA <b>Blast</b> for modern fracture. Existing 4.27 destructible and clothing assets continue to work.
</p>
</tldr>

Destruction and cloth are the two areas where the [PhysX decision](PhysX.md) has the most visible
consequences for content. Both Apex systems are tied to PhysX; removing PhysX removed them.

## Apex Destruction

Apex Destruction fractures a mesh into a hierarchy of chunks that break apart under impact. Assets are
authored as Destructible Meshes, either fractured in-editor with Voronoi decomposition or imported from
external tools.

| Detail | |
|---|---|
| Plugin | `Engine/Plugins/Runtime/ApexDestruction` |
| Asset type | Destructible Mesh |
| Component | `UDestructibleComponent` |
| Status in UE5 | Removed |

It is a mature, well-understood system with a straightforward cost model: chunk count drives everything. A
destructible with a two-level hierarchy and modest chunk counts is cheap; one with deep hierarchies and
hundreds of chunks is not.

Key settings live on the Destructible Mesh asset:

- **Depth** controls how many fracture levels exist. Each level multiplies chunk count.
- **Damage Threshold** and **Damage Spread** control how readily chunks break and how far damage propagates.
- **Support Depth** determines which level participates in structural support calculations.
- **Debris Timeout** and **Debris Max Separation** clean up chunks after they settle. Set these. Chunks that
  never despawn accumulate until frame time collapses.

Instructional video on Apex Destruction [![Instructional video by MeanLemur](https://img.youtube.com/vi/Stn7eL1TFBg/hqdefault.jpg)](https://youtu.be/Stn7eL1TFBg)

## Apex Cloth

Apex Cloth is the PhysX-era clothing system, authored either through the in-editor Clothing Tool or imported
from APEX clothing assets produced in external DCC tools.

| Detail | |
|---|---|
| Asset type | APEX Clothing asset or in-editor clothing data on a skeletal mesh |
| Status in UE5 | Removed in favour of Chaos Cloth |

The in-editor workflow paints cloth parameters directly onto a skeletal mesh: max distance, backstop
radius and backstop distance. Max distance is the primary control &mdash; it defines how far each vertex may
move from its skinned position, so painting it to zero pins a region and increasing it lets the cloth
swing free.

Practical notes:

- Cloth cost scales with simulated vertex count. Paint a low-resolution simulation mesh and skin the render
  mesh to it rather than simulating the render mesh directly.
- Cloth is simulated per skeletal mesh component. Ten characters in cloth cost ten times one character.
- Cloth does not substep with the rest of the scene. Fast character motion can produce stretching that no
  amount of parameter tuning fixes; the answer is usually reducing how fast the attachment point moves.

## NVIDIA Blast

Blast is NVIDIA's successor to Apex Destruction, and the more capable of the two fracture systems.

[Official Blast Plugin Documentation](https://archive.docs.nvidia.com/gameworks/content/gameworkslibrary/blast/1.1/authoring_docs/BlastUe4_QuickStart.html) 


| Detail | |
|---|---|
| Plugin | `Engine/Plugins/GameWorks/Blast` (version 1.0) |
| Platforms | Win64, Linux |
| Enabled by default | No |
| Modules | `BlastLoader`, `BlastRuntime`, `BlastEditor`, `BlastMeshEditor`, `BlastLoaderEditor` |

Blast separates the destruction graph from the physics simulation. A Blast asset describes chunks and the
bonds between them; damage propagates through the bond graph, and chunks become physics bodies only once
they actually detach. This is why Blast handles large structures better than Apex Destruction: an intact
building is a graph, not a thousand sleeping rigid bodies.

Blast also supports runtime fracture, so the fracture pattern can depend on where and how the object was
hit rather than being fully baked at author time.

Enable the plugin from **Edit > Plugins > GameWorks**, then use the Blast Mesh Editor to author assets.

> Blast is not a drop-in replacement for Apex Destruction. The asset types, authoring workflow and runtime
> API are all different. Choose one per project rather than mixing them, and prefer Blast for new work.
>
{style="note"}

## Choosing

| Need | System |
|---|---|
| Existing 4.27 destructible meshes | Apex Destruction &mdash; they still work, do not port them without reason |
| New destruction, large structures | Blast |
| New destruction, small props | Either. Apex Destruction is simpler; Blast scales better. |
| Runtime-dependent fracture patterns | Blast |
| Existing APEX clothing assets | Apex Cloth |
| New clothing | Apex Cloth via the in-editor Clothing Tool |

## Performance

Destruction is one of the easiest ways to destroy a frame budget, because the cost is invisible until
something breaks and then arrives all at once.

- **Cap simultaneous debris.** Set debris timeouts and maximum separation distances on every destructible.
- **Budget chunk counts against the worst case,** not the typical one. A grenade in a room full of
  destructibles is the frame you have to survive.
- **Consider the [instanced physics subsystem](Instanced-Physics.md)** for debris. Chunks that have settled
  and no longer need individual actors can be handled far more cheaply as instances.
- **Profile with `stat physics`** while destroying things, not while standing still.

## Licensing

Apex Destruction, Apex Cloth and Blast are NVIDIA GameWorks technologies inherited through the PhysX and
NvRTX lineage. Their licence terms apply to shipped titles. See the
[GameWorks source SDK EULA](https://developer.nvidia.com/gameworks-source-sdk-eula).

## See also

- [PhysX](PhysX.md)
- [Instanced Physics Subsystem](Instanced-Physics.md)
- [Bundled Plugins](Bundled-Plugins.md)
- [Profiling](Profiling.md)
