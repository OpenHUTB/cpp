# Fixed Timestep

<tldr>
<p>
An optional Vite feature that steps PhysX at a fixed rate independent of frame rate, with render
interpolation so motion stays smooth. Compile-time switch <code>VITE_PHYSX_FIXED_TIMESTEP</code>
(default <code>0</code>), runtime CVar <code>p.VitePhysXFixedTimestep.Enabled</code>.
</p>
</tldr>

By default, Unreal steps physics with the frame's delta time. A frame that took 8&nbsp;ms steps physics by
8&nbsp;ms; a frame that took 33&nbsp;ms steps by 33&nbsp;ms. Simulation results therefore depend on frame
rate, which means the same input produces different outcomes on different hardware, and a replay recorded on
one machine does not reproduce on another.

Vite's fixed timestep mode decouples the two. Physics always advances in identical increments; the renderer
interpolates between the two most recent physics states to keep motion smooth.

## Enabling

This is a two-stage opt-in. The feature is compiled out by default.

<procedure title="Enable fixed timestep physics" id="enable-fixed-timestep">
    <step>
        Set <code>VITE_PHYSX_FIXED_TIMESTEP</code> to <code>1</code>. See
        <a href="Compile-Time-Switches.md">Compile-Time Switches</a> for how to override it without
        editing <code>CoreDefines.h</code>.
    </step>
    <step>Rebuild the engine.</step>
    <step>
        Set <code>p.VitePhysXFixedTimestep.Enabled 1</code> at runtime, or in
        <code>DefaultEngine.ini</code>.
    </step>
    <step>
        Enable substepping in <b>Project Settings &gt; Engine &gt; Physics</b>. The fixed-step
        implementation runs through the substepping path.
    </step>
</procedure>

The compile-time gate exists because the feature changes hot paths in the physics scene, the substep task
and the animation physics blend. Projects that do not need determinism should not pay for the extra branches
and the double-buffered transform storage.

## Console variables

All of these exist only when `VITE_PHYSX_FIXED_TIMESTEP` is compiled in.

| CVar | Default | Range | Purpose |
|---|---|---|---|
| `p.VitePhysXFixedTimestep.Enabled` | `0` | 0/1 | Master enable |
| `p.VitePhysXFixedTimestep.DeltaTime` | `0.01667` (1/60) | 0.0013&ndash;1.0 | Fixed simulation step in seconds |
| `p.VitePhysXFixedTimestep.MaxTimesteps` | `16` | 1&ndash;50 | Maximum fixed steps per game tick before overload accumulates |
| `p.VitePhysXFixedTimestep.MaxCumulativeExtraSteps` | `50` | 0&ndash;100 | Cumulative overload budget before the limit policy applies |
| `p.VitePhysXFixedTimestep.LimitType` | `0` | 0/1 | `0` clamp fixed steps, `1` fall back to variable substeps |
| `p.VitePhysXFixedTimestep.InterpolationMode` | `1` | 0/1/2 | `0` disabled, `1` per component, `2` always |

## How it works

The implementation maintains a time accumulator. Each game tick adds the frame's delta to the accumulator,
then runs as many whole fixed steps as fit. The remainder stays in the accumulator for next frame.

Because a fast frame may fit zero fixed steps and a slow frame may fit several, the system has to handle two
things carefully.

**Buffers are not rotated on frames with no physics step.** Standard substepping swaps the physics target
buffers unconditionally. In fixed-step mode the swap is deferred until a step actually happens, which
preserves one-shot forces, kinematic targets and custom physics callbacks that were queued during a frame
that did not step. Without this, an impulse applied on a zero-step frame would be silently discarded.

**Results are only fetched when a step occurred.** `fetchResults` is skipped on frames with no simulation,
avoiding redundant work and stale transform reads.

## Overload handling

If the game cannot keep up &mdash; a long hitch, or a physics load too heavy for the fixed rate &mdash; the
accumulator grows faster than it drains. Left alone this becomes a death spiral: each frame takes longer,
which queues more steps, which makes the next frame longer still.

Two limits prevent that. `MaxTimesteps` caps steps per tick. Overload beyond that cap accumulates against
`MaxCumulativeExtraSteps`, and when that budget is exhausted, `LimitType` decides what happens:

| `LimitType` | Behaviour | Use when |
|---|---|---|
| `0` &mdash; Clamp | Discard excess accumulated time. Simulation falls behind wall-clock time. | Determinism matters more than real-time correspondence: replays, deterministic tests |
| `1` &mdash; Variable substeps | Fall back to normal variable-step substepping until caught up. | Real-time correspondence matters more: general gameplay |

Clamping means physics runs in slow motion under sustained overload but remains bit-identical for a given
step sequence. Falling back to variable substeps keeps things real-time but sacrifices determinism for the
frames where it engages.

## Render interpolation

Fixed stepping at 60&nbsp;Hz while rendering at 120&nbsp;fps would show each physics state twice without
interpolation, producing visible judder. The system stores the two most recent completed transforms per body
(`PreviousPhysicsTransform` and `LatestPhysicsTransform`) and blends between them using the accumulator's
fractional remainder.

`InterpolationMode` controls how widely this applies:

- **`0` Disabled.** No interpolation. Bodies snap to the latest physics state. Cheapest, and correct if your
  render rate matches your fixed step rate exactly.
- **`1` Per component (default).** Interpolates only bodies whose `bInterpolateWhenSubStepping` flag is set.
  The engine clears this flag for bodies where interpolation cannot be observed &mdash; kinematic
  query-only bodies, for instance &mdash; so this mode gets the visual benefit without paying for bodies
  that do not need it.
- **`2` Always.** Interpolates every body unconditionally.

Interpolation applies to both rigid bodies and the skeletal mesh physics blend, so ragdolls and
physics-blended animation stay smooth.

## Choosing a step rate

`DeltaTime` is the central tuning decision.

| Step rate | Value | Notes |
|---|---|---|
| 30&nbsp;Hz | `0.03333` | Cheapest. Acceptable for slow, heavy objects. Fast bodies will tunnel. |
| 60&nbsp;Hz | `0.01667` | Default. Good general choice. |
| 120&nbsp;Hz | `0.00833` | Better for fast projectiles and tight constraints. Roughly double the CPU cost. |

Physics cost scales linearly with step rate, so 120&nbsp;Hz stepping costs about twice 60&nbsp;Hz. Pick the
lowest rate that keeps your fastest-moving simulated bodies stable, and use CCD rather than a higher step
rate to solve isolated tunnelling problems.

> Choose the step rate early and do not change it after content is tuned. Physics content &mdash; impulse
> magnitudes, constraint stiffness, damping values &mdash; is implicitly tuned against the step rate.
> Changing it late means retuning everything.
>
{style="warning"}

## When to use this

**Use fixed timestep for** deterministic replays, networked physics where client and server must agree,
automated tests that assert on physics outcomes, and any game where identical input must produce identical
results across machines.

**Do not use it for** projects with no determinism requirement. The default variable-step path is cheaper
and the compile-time switch defaults to off for that reason.

## See also

- [PhysX](PhysX.md)
- [Compile-Time Switches](Compile-Time-Switches.md)
- [Instanced Physics Subsystem](Instanced-Physics.md)
- [Profiling](Profiling.md)
