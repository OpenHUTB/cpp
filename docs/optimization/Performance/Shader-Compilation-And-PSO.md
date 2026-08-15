# Shader Compilation and PSO

<tldr>
<p>
Shader permutation count drives compile time, package size and runtime hitching. Vite's main lever is
<code>VITE_RT_PSO_DEBLOAT</code>, which removes ray tracing permutations for effects it does not ship.
Your main levers are shading model discipline and PSO caching.
</p>
</tldr>

A shader permutation is one compiled variant of a shader for one specific combination of features. Unreal
generates permutations combinatorially, so each independent option roughly doubles the count.

This matters in four places: how long a full build takes, how large the packaged game is, how long the
first-run shader compile takes on a player's machine, and whether the game hitches when it encounters a
material for the first time.

## Ray tracing permutations

Ray tracing is the worst case, because ray tracing pipeline state objects must include every ray generation
shader that could be dispatched. A console variable set to `0` does not remove its shaders &mdash; they are
still compiled, packaged and bound into the pipeline.

`VITE_RT_PSO_DEBLOAT`, which defaults to `1`, addresses this by returning `false` from
`ShouldCompilePermutation` for ray tracing effects outside Vite's recommended configuration: per-pixel
ray-traced GI, RTXDI, path tracing, ray-traced translucency, mesh and water caustics, ray-traced reflection
captures, and the non-deferred reflection path.

This is a large reduction. It is also the reason those effects do not work in a default build. Full detail
is in [Compile-Time Switches](Compile-Time-Switches.md).

## Material permutations

The controls you have over material shader count:

**Shading models.** Every [shading model](Shading-Models.md) in use adds permutations. A project using
Default Lit and Callisto BRDF compiles far fewer shaders than one using six models. This is a real budget,
not a theoretical one.

**Material usage flags.** Each **Used With** flag on a material adds a vertex factory permutation: used
with skeletal mesh, instanced static meshes, particle sprites, splines and so on. Unreal sets these
automatically when it encounters a new usage, which means a material can silently accumulate flags it no
longer needs. Audit them.

**Static switches.** Static switch parameters double the permutation count each. Two switches is four
variants; ten is 1024. Use dynamic branches or separate materials when the count gets away from you.

**Quality levels and feature levels.** Each shader platform and quality level the project targets
multiplies everything above.

**Tessellation.** Enabling [tessellation](Tessellation.md) on a material adds hull and domain shader
permutations.

## Compile times

Reference figures from the Vite repository, measured on a Ryzen 9 9950X3D:

| Configuration | Full engine build |
|---|---|
| Full repository | ~15 minutes |
| Without Vite plugins | ~12 minutes |

Houdini is the single largest contributor among the added plugins. See the
[Debloat Guide](Debloat-Guide.md) for how to strip plugins you do not need.

To speed up shader compilation specifically:

- Increase worker count in `BuildConfiguration.xml`, if you have the cores and RAM. Each worker needs
  memory, so oversubscribing causes swapping and makes things worse.
- Use a shared Derived Data Cache across the team. This is the single largest win for a team of more than
  one person: shaders compiled once by anyone are available to everyone.
- Avoid touching global shader headers. A change to a widely-included `.ush` file rebuilds everything.

## PSO caching

A pipeline state object bundles shaders and render state into the object the GPU driver actually needs.
Creating one at the moment a material is first drawn is what causes shader compilation hitches in shipping
games.

PSO caching solves this by recording which PSOs a build actually uses and precompiling them at startup or
during loading.

<procedure title="Set up PSO caching" id="pso-caching">
    <step>Enable PSO caching in <b>Project Settings &gt; Packaging</b>.</step>
    <step>
        Package a build with logging enabled and play through it, covering every level, every material and
        every effect the shipping game contains. Coverage is the whole point: a PSO not encountered during
        recording will still hitch.
    </step>
    <step>Collect the recorded PSO cache files from the build.</step>
    <step>Use the shader pipeline cache tools to consolidate them into a single cache.</step>
    <step>Include the cache in the shipping build and verify the hitches are gone.</step>
</procedure>

This is genuinely tedious work and it is usually left too late. Schedule it before content lock, not after.

## Cache problems

Stale shader caches produce some of the most confusing failures in Unreal: shaders that do not match the
source, materials that render incorrectly, editor crashes on load, and changes to compile-time switches that
appear to have no effect.

If you changed `VITE_RT_PSO_DEBLOAT` or any other switch affecting shader permutations, wipe the cache. See
[Cache Management](Cache-Management.md) for `WipeShaderCache.bat`, which clears the engine-level derived
data cache, intermediate shaders and shader debug info.

## Diagnosing

| Symptom | Likely cause |
|---|---|
| Hitch the first time an effect or material appears | Missing PSO cache entry |
| Very long first launch after a build | Global shader recompile, often from a header change |
| Shaders recompiling every launch | DDC not persisting, or a non-deterministic input to the shader hash |
| Console variable has no visible effect | Feature compiled out. See [Compile-Time Switches](Compile-Time-Switches.md). |
| Compile times growing over time | Permutation creep from accumulated material usage flags and static switches |

`r.ShaderDevelopmentMode 1` and `r.DumpShaderDebugInfo 1` produce diagnostic output when you need to
understand what is actually being compiled.

## See also

- [Compile-Time Switches](Compile-Time-Switches.md)
- [Shading Models](Shading-Models.md)
- [Cache Management](Cache-Management.md)
- [Build Troubleshooting](Build-Troubleshooting.md)
- [Debloat Guide](Debloat-Guide.md)
