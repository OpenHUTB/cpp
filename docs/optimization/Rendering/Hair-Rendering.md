# Hair Rendering

<tldr>
<p>
Two strand-based hair systems ship with Vite: AMD <b>TressFX 5.0</b> and Epic's <b>Groom</b>
(HairStrands). TressFX is the one Vite adds; Groom is the stock 4.27 system. Both are disabled by default.
</p>
</tldr>

Hair is one of the hardest surfaces to render convincingly. Card-based hair is cheap and predictable but
never quite reads correctly under motion or from grazing angles. Strand-based hair simulates and renders
individual fibres, which looks dramatically better and costs dramatically more.

## Available systems

| System | Plugin | Origin | Notes |
|---|---|---|---|
| TressFX 5.0 | `Engine/Plugins/Runtime/TressFX` | AMD | Added by Vite. Simulation, rendering and editor tooling. |
| Groom (HairStrands) | `Engine/Plugins/Runtime/HairStrands` | Epic | Stock 4.27. Alembic-based, works with the `Hair` shading model. |
| Alembic Hair Importer | `Engine/Plugins/Importers/AlembicHairImporter` | Epic | Import path for Groom assets |

Neither hair system is enabled by default. Enable one from **Edit > Plugins**; there is little reason to
enable both in the same project.

## TressFX 5.0

TressFX is AMD's open hair and fur system. The Vite integration includes three modules: `TressFXCore` for
runtime simulation and rendering, `TressFXImportTranslator` for asset import, and `TressFXEditor` for
authoring tools.

It handles simulation (strands respond to head movement, wind and collision) and rendering (order-
independent transparency, self-shadowing and the anisotropic specular that hair requires) as one system.

### Console variables

| CVar | Default | Purpose |
|---|---|---|
| `r.TressFX.StrandsMode` | `0` | Debug visualisation. `0` off, `1` simulation strands, `2` render strands coloured by simulation influence, `3` hair UV, `4` hair root UV, `5` hair seed, `6` dimensions |
| `r.TressFX.Interoplation.FrustumCulling` | `1` | Frustum culling during strand interpolation. Experimental. |
| `r.TressFX.MorphTargetMeshVisualization` | `0` | Accurate transmittance pass, improves rendering of small-scale TressFX |

<note>
<code>r.TressFX.Interoplation.FrustumCulling</code> is spelled as shown in the source, including the
transposed letters in "Interpolation". Use the literal string.
</note>

The debug modes are the fastest way to diagnose hair problems. Mode `2` in particular &mdash; render strands
coloured by simulation influence &mdash; immediately shows whether your guide strand distribution is
reasonable or whether large regions of render strands are being driven by too few guides.

### Cost

Strand hair is expensive in a way that is easy to underestimate, because the cost is split across
simulation (compute), rendering (heavy overdraw with transparency) and shadowing. A single character with
full TressFX hair can consume a meaningful fraction of a
[4K60 frame budget](Performance-Targets.md).

Practical guidance:

- Budget strand hair for hero characters only. Crowd and background characters should use cards.
- Use LODs aggressively. Strand count should drop hard with distance, and switching to cards at mid-range
  is normal.
- Test with your actual [anti-aliasing](Anti-Aliasing.md) configuration. Thin geometry is exactly the case
  where AA method matters most, and hair that looks acceptable with TAA may shimmer with SMAA.

## Groom

Groom is Epic's strand system, present in stock 4.27. It imports Alembic groom caches from Maya, Houdini or
Blender via the Alembic Hair Importer, and renders through the `Hair` shading model with its **Scatter**
(Metallic pin) and **Backlit** (Custom Data 0) parameters, plus **Tangent** on the Normal pin.

Groom is the better-documented path and has more third-party tooling around it, since it is standard Unreal.
TressFX is the better choice if you are coming from an AMD-oriented pipeline or need its specific
simulation behaviour.

## Interaction with ray tracing

Hair and ray tracing interact poorly by default. Strand geometry generates enormous numbers of tiny
primitives in the acceleration structure, and ray-traced [shadows](RT-Shadows-And-Ambient-Occlusion.md) and
[reflections](RT-Reflections.md) against hair are both expensive and noisy.

Consider excluding hair from ray tracing (via the primitive's **Visible in Ray Tracing** flag) and letting
it receive raster shadows instead. The visual difference is usually small; the performance difference is
not.

## See also

- [Shading Models](Shading-Models.md)
- [Anti-Aliasing](Anti-Aliasing.md)
- [Bundled Plugins](Bundled-Plugins.md)
- [Performance Targets](Performance-Targets.md)
