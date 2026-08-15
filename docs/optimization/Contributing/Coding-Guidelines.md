# Coding Guidelines

<tldr>
<p>
Stability first, performance-focused, strict Clang 22 compliance, copyright-clean, ABI-safe. No recursion,
no new virtuals, no new Blueprint exposure, minimal templates. Gate anything that adds overhead or changes
behaviour behind a guard in <code>CoreDefines.h</code>.
</p>
</tldr>

These rules are stricter than stock Unreal's own coding standard. They exist because Vite's value
proposition is predictable performance on Console Class hardware, and because a fork that breaks binary
compatibility with its own shaders is unusable.

## Core principles

<deflist>
<def title="Stability first">
Code must remain predictable and safe under both MSVC and Clang. Behaviour that happens to work on one
compiler is not acceptable.
</def>
<def title="Performance-focused">
All changes must consider CPU, GPU and memory impact, with an ARM-class ~1 GHz CPU as the baseline target.
Performance measured on a local desktop CPU must not be treated as sufficient or representative of
real-world target hardware.
</def>
<def title="Strict Clang 22 compliance">
No MSVC-specific behaviour, compiler extensions or undefined constructs. Code that only builds because MSVC
is permissive about it does not meet this bar, even when MSVC is the compiler you personally use.
</def>
<def title="Licensing clarity">
All contributions must be copyright-clean. Code may be included only if it is original work by the
contributor, or licensed permissively &mdash; MIT, Apache 2.0, BSD, Zlib.
</def>
<def title="ABI safety">
Do not modify engine bitmask layouts, payload structures, serialization formats, or any CPU/GPU-shared
binary contracts.
</def>
</deflist>

## Forbidden

### Recursion

Banned. Use iterative patterns. Recursion makes stack usage unbounded and unpredictable, which matters on
the hardware Vite targets.

### New virtual functions

Do not add new virtuals unless strictly required. Where you can, cache existing engine virtuals rather than
calling through them repeatedly.

### Kismet / Blueprint API additions

No new `BlueprintCallable` or `BlueprintPure` functions without explicit approval. Every Blueprint-exposed
function adds reflection metadata, binary size and a call path that is slower than native C++. At the same type 
the Blueprint Nativization system needs to remain stable.

### Template-heavy patterns

Avoid templates unless they provide a clear performance or architectural benefit. Do not introduce patterns
that increase compile times, binary size or code complexity. See
[Shader Compilation and PSO](Shader-Compilation-And-PSO.md) for why compile times are taken seriously here.

### ABI-breaking modifications

<warning>
Do not modify any of the following:
<list>
<li>Ray tracing payload bitfields</li>
<li>Shader-visible enums or flags</li>
<li>Packed bitmasks used by RHI or RenderCore</li>
<li>Reflection system bitmask definitions</li>
<li>Any CPU/GPU shared struct layouts</li>
</list>
<p>
Such modifications break PSO caching, ray tracing stability, serialization, and cross-platform and
cross-vendor GPU behaviour. This is the single most common cause of immediate rejection.
</p>
</warning>

## Technical standards

1. No undefined behaviour or non-standard extensions.
2. Avoid implicit type conversions.
3. Maintain memory correctness. Avoid large stack allocations; minimise dynamic memory.
4. Prefer `constexpr`, `FORCEINLINE` and zero-cost abstractions.

## Performance rules

1. Favour contiguous memory and cache-friendly layouts.
2. Avoid unpredictable branching in hot paths.
3. No `FString` processing, reflection calls, dynamic allocation or virtual dispatch inside per-frame
   loops.
4. Use SIMD-friendly math (SSE/AVX/NEON) where appropriate.

Rule 3 is the one most often violated by otherwise reasonable code. An `FString` format call in a per-actor
tick is invisible in a test scene and catastrophic at scale &mdash; see the
[400-character CMC benchmark](400-Characters-CMC-Bench.md) for what per-frame cost looks like when
multiplied out.

## Work in progress

1. Commit work in progress to alternative branches.
2. Give a brief commentary on what is left to do.
3. Delete temporary branches once they are no longer needed.
4. Make other forkers aware of the work so effort is not duplicated.

## Verification requirements

### Compilation

Must compile cleanly under MSVC and be Clang compliant. "Compiles on my machine with warnings" is not
clean.

### Stability

- No crashes on startup, shutdown, or on the Tech Showcase project.
- No log spam.
- Guard all code not necessary for shipping.

### ABI

- All shader-visible structs must remain bit-for-bit identical.
- No changes to payloads, bitmask layouts, uniform buffers or reflection flags.
- ABI violations result in immediate rejection.

### Code gating

Anything that adds overhead or changes existing behaviour must sit behind a guard defined in
`Engine\Source\Runtime\Core\Public\Misc\CoreDefines.h`:

```c++
#ifndef VITE_MY_FEATURE
	#define VITE_MY_FEATURE 0
#endif
```

Two rules follow from that. The default must leave the existing path untouched &mdash; a guard whose
default value changes behaviour is not a guard. And the guarded code must actually be excluded when the
switch is off, rather than compiled in and skipped at runtime, or the overhead you were gating is still
being paid.

This is what makes optional features free for projects that do not use them, and it is why
`VITE_PHYSX_FIXED_TIMESTEP` and the rest of the
[compile-time switches](Compile-Time-Switches.md) exist in the form they do. Document every new switch on
that page in the same change.

## Review checklist

Confirm all of these before opening a pull request:

| Check | |
|---|---|
| No recursion | ☐ |
| No added virtual calls, existing engine virtuals cached where possible | ☐ |
| No new Kismet exposure | ☐ |
| Copyright-clean code | ☐ |
| ABI and bitmask integrity preserved | ☐ |
| Strict Clang compliance | ☐ |
| No undefined behaviour | ☐ |
| No performance regressions | ☐ |
| No unnecessary binary size increases | ☐ |
| No shader or RHI ABI mismatches | ☐ |
| Overhead or behaviour changes gated in `CoreDefines.h`, defaulting off | ☐ |
| New compile-time switches documented | ☐ |

## Marking your changes

Vite's engine modifications are marked inline so they survive upstream merges and are findable later. The
existing convention in the tree uses trailing comments on modified lines:

```c++
case MP_Anisotropy:
    CustomPinNames.Add({ MSM_CallistoBRDF, "Diffuse Fresnel" }); // AKCHANGES
    CustomPinNames.Add({ MSM_Toon, "Softness" }); //Fletch
```

Follow whatever marker the surrounding code uses. For a new region, a `// AKCHANGES START` /
`// AKCHANGES END` pair makes the extent of the change clear.

## See also

- [Contributing](Contributing.md)
- [Commit Conventions](Commit-Conventions.md)
- [Backporting](Backporting.md)
- [Compile-Time Switches](Compile-Time-Switches.md)
- [Performance Targets](Performance-Targets.md)
