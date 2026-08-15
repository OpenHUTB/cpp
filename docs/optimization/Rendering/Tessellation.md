# Tessellation and Displacement

<tldr>
<p>
Hardware tessellation with true world displacement, removed in Unreal Engine 5, is fully present in Vite.
Set the material's <b>Tessellation Mode</b> and drive the <b>World Displacement</b> and
<b>Tessellation Multiplier</b> pins.
</p>
</tldr>

Tessellation subdivides geometry on the GPU and displaces the resulting vertices along a displacement map,
producing real geometric detail: silhouettes change, surfaces self-occlude and self-shadow correctly, and
ray-traced effects intersect the displaced surface rather than the flat one.

## Why this page exists

Unreal Engine 5 removed hardware tessellation. Nanite is offered as the replacement, but Nanite solves a
different problem: it renders extremely dense authored meshes efficiently. It does not let you take a
moderate-density mesh and add procedural or texture-driven detail at runtime.

The workflows that break without tessellation:

- **Terrain and landscape displacement** driven by a heightmap, where the detail is procedural and
  view-dependent rather than baked into a mesh.
- **Runtime displacement** &mdash; snow accumulation, footprints, deformation, water surfaces &mdash; where
  the displacement changes during play.
- **Texture-driven detail on tiling surfaces**, where a single material adds real depth to brick, cobble or
  bark across many meshes without authoring unique high-poly geometry for each.
- **Memory-constrained detail**, where a heightmap is dramatically cheaper than the equivalent dense mesh.

Vite keeps the entire 4.27 tessellation pipeline intact, which means these workflows still work.

## Enabling tessellation on a material

<procedure title="Set up a displaced material" id="setup-tessellation">
    <step>
        Open the material and set <b>Tessellation Mode</b> in the details panel:
        <b>Flat Tessellation</b> subdivides without smoothing the base surface; <b>PN Triangles</b>
        smooths the base surface as it subdivides. PN Triangles is usually what you want on organic
        shapes; Flat is correct when the base silhouette should be preserved exactly.
    </step>
    <step>
        Connect a scalar to <b>Tessellation Multiplier</b>. This controls how much subdivision the surface
        receives. Drive it from distance so that far geometry is not subdivided.
    </step>
    <step>
        Connect a vector to <b>World Displacement</b>. The usual form is your surface normal multiplied by
        a heightmap sample multiplied by a displacement scale.
    </step>
    <step>
        Enable <b>Adaptive Tessellation</b> on the material if you want the engine to scale subdivision by
        screen-space triangle size.
    </step>
</procedure>

## Global controls

| CVar | Default | Effect |
|---|---|---|
| `r.TessellationAdaptivePixelsPerTriangle` | `48.0` | Global tessellation factor multiplier. Target screen-space triangle size in pixels when adaptive tessellation is enabled. |

Lowering the value produces smaller triangles and more subdivision; raising it produces fewer. It is a
useful global scalability lever: expose it through your scalability groups so that lower quality presets
back off tessellation across the whole project without touching individual materials.

## Cost and control

Tessellation is not free, and the failure mode is severe: an unbounded tessellation multiplier on a large
surface can generate millions of triangles and collapse frame rate.

**Always bound the multiplier by distance.** The standard pattern multiplies the tessellation factor by a
distance falloff so that only nearby surfaces are subdivided:

- Sample `Camera Position` and the object or pixel world position.
- Compute distance, remap it through a `Divide` and `Saturate` into a 0&ndash;1 falloff.
- Multiply the falloff into the tessellation multiplier.

**Watch triangle density directly.** The wireframe view mode and the `Shader Complexity` view mode both
reveal runaway subdivision immediately. Add `stat rhi` to check triangle counts.

**Displacement cracks** appear at UV seams and mesh boundaries where adjacent vertices displace by
different amounts. Fix them by making the heightmap continuous across the seam, or by masking displacement
to zero at boundaries.

## Interaction with ray tracing

Displaced geometry participates in ray tracing correctly, which is a meaningful advantage over the
parallax-mapping alternatives. A parallax-occlusion-mapped brick wall looks displaced from the primary view
but is flat to every ray-traced [reflection](RT-Reflections.md), [shadow](RT-Shadows-And-Ambient-Occlusion.md)
and [DDGI](DDGI-Dynamic.md) probe ray. A tessellated and displaced one is genuinely displaced to all of
them.

The cost is that ray tracing acceleration structures must be built for the displaced geometry, which
increases BLAS build cost for dynamically tessellated surfaces. For large static displaced surfaces this is
paid once; for surfaces whose displacement animates every frame it is paid continuously. Budget
accordingly.

## Alternatives

Tessellation is the right tool when you need real geometry. When you do not:

| Technique | When it is enough |
|---|---|
| Normal mapping | Lighting detail only; silhouette and self-occlusion do not matter |
| Parallax occlusion mapping | Convincing depth from the primary view, flat to rays, no geometry cost |
| Authored high-poly meshes | Static detail, known in advance, memory budget allows it |
| **Tessellation** | Silhouette matters, displacement is dynamic, or ray-traced effects must see the detail |

## See also

- [Rendering](Rendering.md)
- [Ray Tracing](Ray-Tracing.md)
- [UE4 versus UE5 Cost Analysis](UE4-Versus-UE5-Cost-Analysis.md)
- [Shading Models](Shading-Models.md)
