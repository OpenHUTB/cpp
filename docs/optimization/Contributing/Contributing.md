# Contributing

<tldr>
<p>
Vite has stricter rules than stock Unreal: no recursion, no new virtuals, no new Blueprint exposure, strict
Clang compliance and absolute ABI stability. Read the guidelines before writing code, not before opening a
pull request.
</p>
</tldr>

Vite exists because UE 4.27 can be made fast and stable. Contributions that compromise either defeat the
point. The rules below are unusually restrictive for a game engine fork, and that is deliberate.

## In this section

| Topic | Covers |
|---|---|
| [Coding Guidelines](Coding-Guidelines.md) | Core principles, forbidden constructs, technical and performance rules |
| [Commit Conventions](Commit-Conventions.md) | Prefixes, attribution, branch hygiene |
| [Backporting](Backporting.md) | Bringing UE5 and upstream features into 4.27 |
| [Documentation](Documentation-Contributions.md) | Contributing to these docs |

## Before you start

<procedure title="Contribution workflow" id="contribution-workflow">
    <step>
        Read the <a href="Coding-Guidelines.md">coding guidelines</a>. Several common C++ patterns are
        banned outright, and finding out after you have written the code is expensive.
    </step>
    <step>
        Check whether the change is already covered. The
        <a href="Compile-Time-Switches.md">compile-time switches</a> and
        <a href="Engine-Defaults.md">engine default changes</a> pages document a lot of existing work.
    </step>
    <step>
        Work on a branch. Push work-in-progress to alternative branches with a note on what remains, and
        make other forkers aware of it.
    </step>
    <step>
        Verify: compiles cleanly under MSVC, Clang compliant, no crashes on startup, shutdown or the
        Tech Showcase project, no log spam, no ABI changes.
    </step>
    <step>
        Work through the <a href="Coding-Guidelines.md">review checklist</a> before opening the pull
        request.
    </step>
</procedure>

## The three rules that reject most pull requests

<deflist>
<def title="ABI stability">
Do not modify ray tracing payload bitfields, shader-visible enums or flags, packed bitmasks used by RHI or
RenderCore, reflection system bitmask definitions, or any CPU/GPU shared struct layout. Breaking these
breaks PSO caching, ray tracing stability, serialization and cross-vendor GPU behaviour. ABI violations
are rejected immediately, regardless of how good the rest of the change is.
</def>
<def title="Performance baseline">
Changes are evaluated against an ARM-class ~1 GHz CPU baseline. Measuring on your desktop and finding it
fast does not clear the bar. See <a href="Performance-Targets.md">Performance Targets</a>.
</def>
<def title="Copyright cleanliness">
Contributions must be original work or permissively licensed (MIT, Apache 2.0, BSD, Zlib). Code copied
from UE5 into a 4.27 fork sits under Epic's licence, which is a different question from a permissive
licence &mdash; see <a href="Backporting.md">Backporting</a>.
</def>
</deflist>

## What makes a good contribution

Contributions that fit Vite well tend to share a shape:

- **Measurable.** A profiling capture before and after, on representative content, beats an argument.
- **Guarded.** Anything not needed in shipping is behind a
  [compile-time switch](Compile-Time-Switches.md) or a console variable, and off by default when it costs
  something.
- **Narrow.** A change touching one subsystem is reviewable. A change touching the renderer, the physics
  layer and the build system is not.
- **Documented.** New console variables, switches and defaults need a documentation page or a section in an
  existing one.

## Getting set up

See [Build from Source](Build-From-Source.md) and
[Toolchain Requirements](Toolchain-Requirements.md). Note that engine development requires a source build
&mdash; [installed builds](Installed-Builds.md) cannot compile engine C++.

## See also

- [Coding Guidelines](Coding-Guidelines.md)
- [Commit Conventions](Commit-Conventions.md)
- [Backporting](Backporting.md)
- [Build from Source](Build-From-Source.md)
- [Performance Targets](Performance-Targets.md)
