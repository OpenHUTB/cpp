# Engine Overview

<tldr>
<p>
Vite is Unreal Engine 4.27.2 (<code>++UE4+Release-4.27</code>) forked from NvRTX 4.27 Caustics merged fully with UE 4.27
Plus, NvRTX 5.0 and AMD branch features merged in, plus 300+ backports from UE 5.0&ndash;5.8.
</p>
</tldr>

This section explains the reasoning behind Vite's architecture: why the fork is based on Unreal Engine 4.27
rather than UE5, what performance envelope it is designed for, and where the measured cost differences
between the two engine generations come from.

If you just want to get the engine running, go to [Getting Started](Getting-Started.md) instead.

## Base version

| Property           | Value                                                |
|--------------------|------------------------------------------------------|
| Engine version     | 4.27.2                                               |
| Repo name          | `UnrealEngineVite-PhysX`                             |
| Upstream base      | NvRTX 4.27 Caustics                                  |
| Merged branches    | UE 4.27 Plus, NvRTX 5.0, AMD GPUOpen engine branches |
| UE5 backports      | 300+ in release, 1,000+ in internal staging          |
| Physics backend    | Vite PhysX                                           |
| Main Path Renderer | Deferred, agnostic DXR Ray Tracing pipeline          |

## Section contents

### [Why NvRTX 4.27](Why-NvRTX-427.md)

The technical argument for the base version: what changed in UE 5.1's ray tracing scene construction, why
Lumen coupling makes alternative GI integrations harder, and why PhysX removal matters.

### [Performance Targets](Performance-Targets.md)

The four PS5-class configurations Vite is tuned against, from 4K120 stylised through to 1440p30 with the
full ray tracing effect suite.

### [UE4 versus UE5 Cost Analysis](UE4-Versus-UE5-Cost-Analysis.md)

Where the frame time actually goes: shader instruction counts, physics, character movement, memory,
Slate, skeletal meshes, tick cost, render thread overhead, Blueprint nativization and volumetrics.

### [Release Notes](Release-Notes.md)

What is in the current release branch, what is in progress, and what is planned.

## Design principles

Three commitments shape almost every decision in the codebase, and they are worth stating explicitly
because they explain choices that otherwise look conservative.

**Battle-tested technology over in-house technology.** Where Epic built a new system for UE5, Vite prefers
the industry-standard solutions that are widely shipped in AAA titles: PhysX rather than Chaos, DDGI rather than
Lumen, HW Tessellation rather than Nanite, TressFX rather than Groom. These are not nostalgic choices; they are choices
about which solutions has been through the most shipped games.

**Native resolution over reconstruction.** Vite's performance targets are stated at native 4K and native
1440p. Upscalers are supported and integrated, but they are treated as a way to go faster than the target,
not as a way to reach it.

**Frame-time budgets are a design constraint, not an optimisation phase.** Performance targets define a
product's end feature set and the quality of the user experience. Vite maintains an iterative optimisation
plan for every major feature it introduces, rather than deferring optimisation to the end of a project.

The [Engine Coding Guidelines](Coding-Guidelines.md) encode these principles as concrete rules for
contributors: no recursion, no new virtuals without justification, no ABI-breaking changes, and strict
Clang compliance with an ARM-class CPU as the baseline performance target.

## See also

- [Introduction to Vite](Introduction-to-Vite.md)
- [Rendering](Rendering.md)
- [Physics](Physics.md)
- [Engine Coding Guidelines](Coding-Guidelines.md)
