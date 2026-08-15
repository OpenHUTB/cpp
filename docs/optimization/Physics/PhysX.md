# PhysX

<tldr>
<p>
Vite integrates <b>NVIDIA PhysX 3.4</b> for rigid-body simulation, collision queries, constraints,
vehicles and skeletal physics. Vite extends the backend with modern compiler support, fixed-timestep
simulation, NVIDIA Blast and a high-count instanced physics subsystem.
</p>
</tldr>

PhysX is exposed through Unreal Engine's standard physics framework. Existing engine types such as body
instances, physical materials, physics assets, collision profiles and constraint components use the PhysX
scene underneath.

## Core simulation features

### Rigid bodies

- static, dynamic and kinematic actors;
- box, sphere, capsule, convex and triangle-mesh collision geometry;
- physical materials with friction, restitution and density;
- gravity, damping, forces, impulses and torque;
- sleeping, waking and active-body tracking;
- continuous collision detection for fast-moving bodies;
- collision filtering and simulation/query shape flags;
- contact, overlap and wake/sleep notifications.

### Scene queries

PhysX provides the collision-query path used throughout the engine:

- raycasts;
- shape sweeps;
- overlaps;
- single-hit and multi-hit queries;
- object-channel, trace-channel and profile filtering;
- synchronous scene queries used by movement, weapons, navigation and gameplay systems.

Query cost depends on collision complexity, broad-phase population, filter callbacks and result count. Use
simple collision for repeated gameplay queries and avoid requesting multi-hit results when only the first
blocking hit is needed.

### Constraints

The integration supports fixed, distance, hinge, spherical and D6-style constraints through Unreal's
constraint framework. Available controls include:

- linear and angular limits;
- motors and drives;
- break force and break torque;
- constraint projection;
- collision enablement between constrained bodies;
- mass and inertia scaling;
- skeletal constraint chains authored in Physics Assets.

### Character and skeletal physics

PhysX supplies collision queries for Character Movement and rigid-body simulation for Physics Assets. This
includes ragdolls, physical animation, hit reactions, body welding and per-body collision configuration.

### Vehicles

Vite retains the UE4.27 PhysX vehicle stack, including wheel simulation, suspension, tire friction, engine,
gearing, differential and drivetrain configuration. Vehicle behavior is authored through the standard
vehicle and tire data assets used by UE4 projects.

## Destruction, cloth and Blast

The Vite physics stack includes the following NVIDIA systems:

| Feature | Function |
|---|---|
| APEX Destruction | Authored destructible meshes, support chunks, damage and fracture events |
| APEX Cloth | Vertex-painted cloth constraints and skeletal-mesh clothing simulation |
| NVIDIA Blast | Destruction assets and fracture workflows integrated alongside the PhysX backend |

See [Destruction and Cloth](Destruction-And-Cloth.md) for authoring and runtime guidance.

## Vite extensions

### Modern toolchain support

Vite's PhysX libraries and build files have been updated for newer MSVC and Clang toolchains, including the
Android NDK Clang path used by current Vite builds. Toolchain changes are validated with the physics test and
stress workloads before release.

### Fixed-timestep simulation

The optional fixed-timestep path decouples the simulation cadence from variable render frames and adds render
interpolation. It is intended for projects that require a stable simulation delta or reproducible captures.
See [Fixed Timestep](Fixed-Timestep.md) for compile-time setup and integration requirements.

### Instanced physics subsystem

The instanced subsystem represents large homogeneous rigid-body sets through instanced meshes instead of one
Actor and component hierarchy per body. It is intended for debris, shells, environmental objects and other
high-count simulations. See [Instanced Physics Subsystem](Instanced-Physics.md).

### Native actor path

Engine-level systems that already own compact simulation state can operate closer to native PhysX actors and
avoid unnecessary high-level Actor/component work. This is a specialized integration path: ownership,
lifetime, transform synchronization, collision filtering and teardown remain the caller's responsibility.

## Configuration

Physics settings are under **Project Settings > Engine > Physics**.

| Setting | Function |
|---|---|
| Default Gravity Z | World gravity; `-980.0` corresponds to 1 uu = 1 cm |
| Substepping | Divides a long frame into smaller simulation steps |
| Max Substep Delta Time | Maximum delta processed by one substep |
| Max Substeps | Upper bound on simulation steps performed for one frame |
| Simulate Skeletal Mesh on Dedicated Server | Enables skeletal rigid-body simulation on server targets |
| Default Degrees Of Freedom | Constrains motion for planar or limited-axis games |

### Substepping

Without substepping, the physics scene advances with the frame delta. Large or variable deltas can reduce
contact and constraint stability. Substepping performs multiple smaller advances when required; its CPU cost
increases with the number of executed substeps.

Choose `Max Substep Delta Time` from the fastest interaction that must remain stable, then set `Max Substeps`
to cap worst-case work. A cap prevents a slow frame from creating an unbounded simulation backlog.

For a fixed simulation cadence rather than frame-triggered substeps, use the
[fixed-timestep path](Fixed-Timestep.md).

## PhysX Visual Debugger

PhysX Visual Debugger can inspect a connected scene, including actors, shapes, contacts, constraints and
simulation state. Use a development build with PVD support, connect before reproducing the issue, and limit
capture duration when the scene contains many bodies.

Viewport collision visualization remains useful for confirming authored geometry and filtering before a
full PVD capture.

## Profiling

| Command | Shows |
|---|---|
| `stat physics` | Physics timing divided by engine phase |
| `stat game` | Physics cost in the context of game-thread work |
| `p.NumPhysScenes` | Number of active physics scenes |
| `show Collision` | Collision geometry in the viewport |

Profile with representative collision geometry, body counts, sleeping behavior and event generation. Record
physics milliseconds, body/shape counts, active-body count, solver settings, substeps and worker configuration
with every comparison.

For high body counts, compare standard Actor-based simulation with the
[instanced subsystem](Instanced-Physics.md) using the same shapes and solver settings.

## See also

- [Physics](Physics.md)
- [Fixed Timestep](Fixed-Timestep.md)
- [Destruction and Cloth](Destruction-And-Cloth.md)
- [Instanced Physics Subsystem](Instanced-Physics.md)
- [Physics Cube Bench](Physics-Cube-Bench.md)
- [PhysX Test](PhysXTest.md)
