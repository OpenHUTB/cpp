# PhysX Instanced Subsystem Demo

<tldr>
<p>
Demonstrates rigid body counts that conventional actor-per-body simulation cannot reach, by backing
instanced mesh transforms with PhysX bodies managed from a world subsystem.
</p>
</tldr>

## Download

[Demo project](https://drive.google.com/file/d/1NulunBP2Qre5vLyYnkiqywovsycNuWdQ/view) ·
[Plugin repository](https://github.com/Dragomirson/PhysXInstancedSubsystem)

<img src="../../img/optimization/PhysXInstancedSubsystem.png" alt="Large number of instanced rigid bodies simulated by the PhysX instanced subsystem" border-effect="line"/>

*Instanced rendering with real per-instance PhysX bodies. The body count here is well past what
actor-per-body simulation reaches on the same budget.*

## What it demonstrates

Conventional Unreal physics spawns an `AActor` with a `UPrimitiveComponent` per body. That works to a few
thousand bodies and then the actor and component overhead, not the solver, becomes the limit &mdash; as the
[Physics Cube Bench](Physics-Cube-Bench.md) shows.

The subsystem keeps one or a few instanced mesh actors for rendering, creates per-instance PhysX rigid
bodies, and writes their poses back into ISM/HISM instance transforms. Rendering is instanced, simulation
is real, and the actor overhead is gone.

Run this alongside the Physics Cube Bench for the comparison that makes the difference concrete.

## What to look at

| Feature | Where it shows |
|---|---|
| Per-frame budgets | Spawn and simulation work is capped per frame rather than spiking |
| Auto-stop conditions | Settled instances stop simulating and convert to storage |
| Dynamic versus storage instances | Only what needs simulating is simulated |
| Lifetime management | Instances expire without leaving actors behind |
| CCD modes | Fast-moving bodies without tunnelling |

The budget and auto-stop behaviour is the part worth understanding. Large body counts are affordable
because most bodies are asleep most of the time; the subsystem makes that automatic rather than something
you manage.

Full API and configuration detail in [Instanced Physics](Instanced-Physics.md).

## When to use it

Debris, rubble, shell casings, foliage knocked loose, destruction fallout &mdash; anything where you want
many physically simulated objects that do not each need their own actor identity, gameplay logic or
replication.

It is not a replacement for actor-based physics where you need per-object gameplay behaviour. It is for the
case where the objects are scenery that happens to move.

## Status

Bundled at version 1.11 in `Engine\Plugins\Runtime\VitePlugins\PhysXInstancedSubsystem`. See
[Bundled Plugins](Bundled-Plugins.md).

## See also

- [Instanced Physics](Instanced-Physics.md)
- [Physics Cube Bench](Physics-Cube-Bench.md)
- [PhysX](PhysX.md)
- [Bundled Plugins](Bundled-Plugins.md)
