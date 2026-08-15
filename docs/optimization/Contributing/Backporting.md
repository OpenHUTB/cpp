# Backporting

<tldr>
<p>
Vite July Release has currently over 300 backports from UE5.0 to UE 5.8 
</p>
</tldr>

Vite is a NvRTX 4.27 Plus fork that keeps pace with technology developed for newer engines. That means backporting is
a routine activity in order to upgrade the source.

## What has been backported

| Source | Examples                                                                                                                              |
|---|---------------------------------------------------------------------------------------------------------------------------------------|
| UE5 | Non Trivial Rendering Optimizations, comprehensive CPU Optimizations, Plugins, QoL improvements, Animation features, Third Party Libs |
| Vendor SDKs | [DLSS 4.5, FSR 4, XeSS 3.0.5, Streamline](Upscalers.md), TressFX 5.0, Blast                                                           |
| Third-party plugins | ACL, Motion Symphony, [Kawaii Physics](Bundled-Plugins.md)                                                                            |

## Before you backport

<procedure title="Backport feasibility check" id="backport-check">
    <step>
        <b>Check the licence.</b> UE5 code is under Epic's licence, which permits use in an Unreal
        Engine fork but not arbitrary redistribution. Third-party code must be permissively licensed
        &mdash; MIT, Apache 2.0, BSD, Zlib. See
        <a href="Coding-Guidelines.md">Coding Guidelines</a>.
    </step>
    <step>
        <b>Check the dependency graph.</b> A UE5 feature that depends on Nanite, Lumen, the UE5 RDG API
        surface or the Chaos physics interface will not backport cleanly. Establish what it actually needs
        before writing code.
    </step>
    <step>
        <b>Check the ABI implications.</b> If the feature requires changing a shader-visible struct or a
        packed bitmask, it cannot be backported as-is. See the
        <a href="Coding-Guidelines.md">ABI rules</a>.
    </step>
    <step>
        <b>Check whether it is worth it.</b> A UE5 feature that exists to solve a UE5 problem may have no
        value in Vite. Focus on optimizations, Third Party Lib Updates, Plugin updates,SDK updates and fixes. 
    </step>
</procedure>

## Common obstacles

<deflist>
<def title="RDG API differences">
UE5's Render Dependency Graph API diverged substantially from 4.27's. Pass declarations, resource
transitions and uniform buffer creation all differ. This is usually mechanical to translate but touches
every line of a render pass.
</def>
<def title="Chaos versus PhysX">
Anything in UE5 that touches physics assumes Chaos. Vite is PhysX &mdash; see
<a href="PhysX.md">PhysX</a>. Physics-adjacent backports need the interface rewritten, not translated.
</def>
<def title="Core type changes">
UE5 changed several core types, most visibly the move from <code>FVector</code> as float to double
precision. Backporting means reverting those changes throughout, and watching for places where the
precision was load-bearing.
</def>
<def title="Module reorganisation">
UE5 split and renamed modules. Includes, build dependencies and module names all need remapping to their
4.27 locations.
</def>
<def title="Shading model slots">
Shading models are a limited enum. Vite already adds <a href="Shading-Models.md">Callisto BRDF, Toon and
Lit Reactive</a>. Adding another consumes a slot and adds shader permutations across the board &mdash; see
<a href="Shader-Compilation-And-PSO.md">Shader Compilation and PSO</a>.
</def>
</deflist>

## Doing the work

### Keep the change traceable

Mark backported regions with the existing inline comment convention so they are identifiable after future
upstream merges:

```c++
// AKCHANGES START
// Backported from UE5.3, CL 12345678
...
// AKCHANGES END
```

### Guard what costs something

If the feature has a runtime or compile-time cost that not every project wants, put it behind a
[compile-time switch](Compile-Time-Switches.md) or a console variable, defaulted off. Vite's existing
switches follow this pattern:

| Switch | Default | Guards |
|---|---|---|
| `VITE_PHYSX_FIXED_TIMESTEP` | 0 | Fixed timestep physics |
| `VITE_RT_PSO_DEBLOAT` | 1 | Compiles out most ray tracing permutations |
| `VITE_O_SSAO` | 1 | Optimised SSAO path |
| `VITE_DLSS_PATCH` | 0 | DLSS translucency and volumetric fog fixes |
| `VITE_NVRTX_TRANSLUCENCY_DEPTH` | 0 | NvRTX translucency depth handling |

Note that `VITE_RT_PSO_DEBLOAT` defaults to the *restrictive* value. When a feature's cost is
shader-permutation count, the default that keeps builds fast wins, and the feature is documented as opt-in.

### Test against the baseline

Backported features need measurement against Vite's [performance targets](Performance-Targets.md), not
against how they performed in UE5. A feature tuned for UE5's frame budget may be unaffordable in a 4.27
project running on the hardware Vite targets.

## Documenting a backport

Every backport needs:

| Item | Where |
|---|---|
| Source link, upstream commit or tag | Commit body &mdash; see [Commit Conventions](Commit-Conventions.md) |
| What was changed during translation | Commit body |
| Any new console variable or switch | A documentation page, and [Compile-Time Switches](Compile-Time-Switches.md) if applicable |
| Availability caveats | The feature's own page, prominently |

That last one matters more than it looks. A backported feature that is compiled out by default, or that
requires a plugin to be enabled, will otherwise be reported as broken by everyone who tries to use it.

## Asset backporting

Backporting *assets* from UE5 is a different problem, handled by the Asset Downgrader rather than by code.
See [Migrating from UE5](Migrating-From-UE5.md) and
[Proposed Plugins](Proposed-Plugins.md).

## See also

- [Contributing](Contributing.md)
- [Coding Guidelines](Coding-Guidelines.md)
- [Commit Conventions](Commit-Conventions.md)
- [Compile-Time Switches](Compile-Time-Switches.md)
- [Migrating from UE5](Migrating-From-UE5.md)
