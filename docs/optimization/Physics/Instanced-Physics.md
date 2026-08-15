# Instanced Physics Subsystem

<tldr>
<p>
A Vite plugin that gives individual instances of an Instanced Static Mesh component their own PhysX
rigid bodies, controlled through stable numeric handles. Built for simulating thousands of objects
where one actor per object would be unaffordable.
</p>
</tldr>

The standard way to simulate many objects in Unreal is one actor per object, each with a primitive
component and a body instance. That works up to a few hundred objects and then falls apart: actor overhead,
component ticking, transform propagation and scene graph updates cost more than the physics itself.

The PhysX Instanced Subsystem separates the two concerns. Rendering goes through a single Instanced Static
Mesh component; physics goes through PhysX bodies the subsystem owns directly. There are no per-object
actors and no per-object components.

| Detail | |
|---|---|
| Plugin | `Engine/Plugins/Runtime/VitePlugins/PhysXInstancedSubsystem` |
| Module | `PhysXInstancedSubsystem` |
| Subsystem | `UPhysXInstancedWorldSubsystem` (tickable world subsystem) |
| Actor | `APhysXInstancedMeshActor` |
| Component | `UPhysXInstancedStaticMeshComponent` |
| Author | NordVader Inc. |

See the [PhysX Instanced Subsystem demo](PhysX-Instanced-Subsystem.md) for a working project.

## Core concepts

**Instances are addressed by handle, not pointer.** `FPhysXInstanceID` wraps a `uint32`, where `0` means
invalid. Handles stay stable even when instances move between actors, which matters because ISM instance
indices shift when instances are removed. Gameplay code holds handles; the subsystem maintains the mapping
from handle to component and index.

**Dynamic and storage instances are different states of the same instance.** A dynamic instance has a
PhysX body and simulates. A storage instance lives on a separate storage actor with no PhysX body at all.
Converting between the two preserves the handle. This is the central performance mechanism: debris that has
come to rest converts to storage, costs nothing to simulate, and stays visible.

**Work is budgeted per frame.** Creating PhysX bodies, applying forces and expiring lifetimes are all
queued and processed against per-frame limits, so spawning ten thousand instances in one frame does not
produce a ten-thousand-body hitch.

## Setting up an actor

Place an `APhysXInstancedMeshActor` in the level and configure it in the details panel under
**Phys X Instance**.

<procedure title="Configure an instanced physics actor" id="setup-instanced-actor">
    <step>Set <b>Static Mesh</b>, and override materials if needed.</step>
    <step>
        Choose a <b>Spawn Mode</b>. <b>Manual</b> uses the <b>Instance Relative Transforms</b> array;
        <b>Grid</b> generates a rows &times; columns &times; layers grid from the spacing settings, which
        is convenient for benchmarks and test scenes.
    </step>
    <step>
        Set <b>Instance Shape Type</b>. Box, Sphere and Capsule are cheapest; <b>Convex Mesh</b> is
        accurate and moderately priced; <b>Triangle Mesh</b> is static or kinematic only.
    </step>
    <step>
        Set <b>Simulate Instances</b> and <b>Instances Use Gravity</b>, plus mass override and damping if
        the defaults are wrong for your object.
    </step>
    <step>
        Leave <b>Disable ISM Physics</b> enabled so the render component does not create its own collision
        alongside the subsystem's bodies.
    </step>
    <step>
        Enable <b>Auto Register On Begin Play</b>, or call <code>BuildAndRegisterInstances</code>, which is
        also callable from the editor.
    </step>
</procedure>

If no collision mesh is specified for Convex or Triangle shapes, the render mesh is used. Mass is computed
from the physical material's density on the collision mesh, converted from g/cm³ to kg/m³, and scaled by the
component's mass scale.

## Auto-stop

Auto-stop is what makes large-scale simulation affordable. It detects instances that have effectively
stopped moving and does something cheaper with them.

Configure it in **Phys X Instance > Runtime > Auto Stop Config**:

| Setting | Default | Purpose |
|---|---|---|
| Enable Auto Stop | `false` | Master enable |
| Condition | PhysX sleep flag | How "stopped" is determined |
| Linear Speed Threshold | `5.0` cm/s | Used by velocity-based conditions |
| Angular Speed Threshold | `5.0` deg/s | Used by velocity-based conditions |
| Min Stopped Time | `0.5` s | How long the condition must hold before acting |
| Action | Destroy Body | What happens when it fires |

Conditions are **PhysX sleep flag only**, **velocity thresholds only**, **sleep OR velocity**, or
**sleep AND velocity**. The sleep flag is cheapest and usually sufficient; velocity thresholds catch bodies
that are drifting slowly enough to be visually stopped but not sleeping.

Actions, in ascending order of aggressiveness:

| Action | Effect |
|---|---|
| Do nothing | Track state only |
| Disable simulation (keep body) | Body stays but stops simulating |
| Destroy body (keep instance) | PhysX body freed, visual instance remains in place |
| Destroy body and remove instance | Both freed. Shifts ISM indices &mdash; use handles, not indices. |
| Convert to storage | Instance moves to a storage actor, body freed, handle preserved |

**Convert to storage** is usually the right choice for debris you want to keep visible. **Destroy body** is
right when you want the object to remain but never move again.

### Safety rules

Two additional rules catch instances that will never stop on their own:

- **Max Fall Time** fires the stop action after an instance has been falling continuously for longer than
  the threshold. Catches objects that fell through world geometry.
- **Max Distance From Actor** fires when an instance travels further than the threshold from its owner.
  Catches objects launched by a bad impulse.

Separately, **Use Custom Kill Z** with a **Custom Kill Z** height and a **Lost Instance Action** handles
instances that fall below a world Z threshold.

## Lifetime

Instances can expire on a timer. Set **Enable Lifetime**, a **Default Life Time Seconds** and a
**Default Lifetime Action** on the actor, or override per spawn through the spawn request.

Expirations are held in a min-heap keyed on expiry time, so the per-frame cost is proportional to the number
of instances actually expiring rather than the total instance count.

## Continuous collision detection

CCD prevents fast bodies tunnelling through thin geometry, and it is not free. **CCD Config** offers four
modes:

| Mode | Behaviour |
|---|---|
| Off | Never |
| Simulating bodies only | Only bodies that are actually dynamic |
| Auto (by velocity) | Enabled above **Min CCD Velocity** (default 2000 cm/s), optionally capped by **Max CCD Velocity** |
| All bodies | Always |

Auto by velocity is the right default for debris: slow-moving chunks skip CCD entirely, and only the fast
ones pay for it.

## Runtime API

Everything is Blueprint-callable. Get the subsystem from the world, then work through handles.

### Spawning

```c++
FPhysXSpawnInstanceRequest Request;
Request.ActorMode = EPhysXInstanceActorMode::FindOrCreateByMeshAndMats;
Request.StaticMesh = DebrisMesh;
Request.InstanceWorldTransform = SpawnTransform;
Request.bStartSimulating = true;
Request.InitialLinearVelocity = LaunchVelocity;
Request.bOverrideLifetime = true;
Request.LifeTimeSeconds = 15.0f;
Request.LifetimeAction = EPhysXInstanceStopAction::ConvertToStorage;

const FPhysXSpawnInstanceResult Result = Subsystem->SpawnPhysicsInstance(Request);
```

**Actor Mode** decides where the instance lands: `AlwaysCreateNew` spawns a fresh actor,
`FindOrCreateByMeshAndMats` reuses an existing actor with matching mesh and materials, and
`UseExplicitActor` targets an actor you supply. `FindOrCreateByMeshAndMats` is the usual choice &mdash; it
keeps instance counts consolidated into as few ISM components as possible, which is what makes the rendering
side efficient.

For bulk registration of instances that already exist on a component, `RegisterInstancesBatch` spreads the
work across frames.

### Forces and queries

Force and impulse functions come in plain and Advanced variants. The Advanced versions take
`bIncludeStorage` and `bConvertStorageToDynamic`, which control whether a storage instance is woken back
into a dynamic body by the call &mdash; that is how a settled debris field reacts to a later explosion.

```c++
Subsystem->AddRadialImpulse(
    ExplosionOrigin, /*Radius=*/500.0f, /*Strength=*/80000.0f,
    /*bVelChange=*/false,
    /*bIncludeStorage=*/true,
    /*bConvertStorageToDynamic=*/true,
    /*bLinearFalloff=*/true);
```

Spatial queries return handles rather than hit results: `RaycastInstanceID`, `SweepSphereInstanceID`,
`OverlapSphereInstanceIDs`, `FindNearestInstance` and `FindNearestInstanceAdvanced`. Each takes an optional
debug mode (`None`, `Basic`, `Detailed`) and draw duration, which is the fastest way to understand why a
query is not hitting what you expect.

### Events

`APhysXInstancedMeshActor` exposes six multicast delegates: `OnInstancePreRemove`, `OnInstancePostRemove`,
`OnInstancePreConvert`, `OnInstancePostConvert`, `OnInstancePrePhysics` and `OnInstancePostPhysics`.

Events are gated by the **Instance Event Mask** bitmask on the actor. Leave the mask empty unless you are
actually binding, since broadcasting to nothing across thousands of instances is wasted work. Remove and
convert events carry a reason (`Explicit`, `Expired`, `AutoStop`, `KillZ`, `Lost`) and the world transform
at the time, which is what you need to spawn a particle effect or play a sound as debris settles.

## Performance tuning

| Setting | Default | Effect |
|---|---|---|
| `MaxAddActorsPerFrame` | `64` | Bodies added to the PhysX scene per frame. `0` means no limit. |
| `MaxInstanceTasksPerFrame` | `4096` | Queued force/impulse/sleep/wake operations per frame. `0` means no limit. |
| `MaxLifetimeExpirationsPerTick` | `4096` | Lifetime expirations processed per tick. `0` means no limit. |

All three are config properties on the subsystem, so they can be set in `DefaultGame.ini` and tuned per
platform.

`MaxAddActorsPerFrame` is the one to adjust first. Adding a PhysX body to a scene is not cheap, and a burst
of spawns is the most likely source of a hitch. Lowering it spreads the cost; raising it reduces the delay
before newly spawned debris starts moving.

The subsystem evaluates instances in parallel where it can. Worker threads read PhysX state and actor
configuration through copied snapshots and never touch UObjects; the game thread applies results back as
batched transform updates per component.

## Guidance

**Use this for** debris, shell casings, destructible fragments after they detach, knocked-over props,
scattered clutter, and anything where you want hundreds or thousands of simulated objects.

**Do not use it for** objects that need their own gameplay logic, components, replication or Blueprint
behaviour. Those are actors, and they should stay actors.

**Watch out for** ISM index shifting. Removing a visual instance changes the indices of instances after it.
The subsystem repairs its own mapping, but any index you cached in your own code is now wrong. Cache
handles.

## See also

- [PhysX](PhysX.md)
- [PhysX Instanced Subsystem demo](PhysX-Instanced-Subsystem.md)
- [Destruction and Cloth](Destruction-And-Cloth.md)
- [Physics Cube Bench](Physics-Cube-Bench.md)
- [Profiling](Profiling.md)
