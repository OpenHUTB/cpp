# Static DDGI

<tldr>
<p>
Baked probe volumes with virtually instantaneous bake times. Better bounce fidelity than traditional baked
lighting, better coverage of moving objects, and no runtime ray tracing cost &mdash; so it runs on GPUs with
no DXR support at all.
</p>
</tldr>

Static DDGI uses the same probe volumes and the same Irradiance calculations and Octahedral representations as
[Dynamic DDGI](./DDGI-Dynamic.md), but resolves irradiance once at bake time instead of continuously at
runtime. It is the low-end and no Ray-Tracing Hardware GPU support (GTX 900/ Radeon 5000 series or older) path.

<note>
Dynamic RT DDGI works from 2016's GTX 1060 6GB (10 years old), 
the minimum requirement is Pascal Architecture with atleast 6GB of VRAM.
</note>

## What you get

**Near-instant bakes.** This is the headline difference from LightMass. Baking probe irradiance is a
fundamentally cheaper problem than solving lightmap UVs and per-texel radiosity, and the iteration loop
changes character entirely when a bake takes seconds rather than hours.

**Better bounce fidelity than traditional baked lighting.** The Actual Geometrical Scene traversal representation 
captures directional irradiance rather than a flat lightmap values, so surfaces respond correctly to their orientation
even where lighting was baked.

**Better coverage of moving objects.** This is the structural advantage over lightmaps. Lightmaps store
lighting on *surfaces*, so a moving object has to fall back to indirect lighting samples or volumetric
lightmaps. DDGI probe volumes store lighting in *space*, so anything that moves through the volume &mdash;
characters, vehicles, physics debris &mdash; samples the same representation static geometry does, and looks
consistent with it.

<note>
**No DXR requirement at runtime.** Once baked, the Probe texture data is serialized there is no RT acceleration. 
This is what makes Static DDGI viable on ultra-low-end GPUs and on hardware with no ray tracing support at all.
</note>

## When to use it

| Situation | Recommendation                               |
|---|----------------------------------------------|
| Minimum spec has no DXR support | Static RT DDGI                               |
| Minimum spec is very low-end but DXR capable | Dynamic RT DDGI at lower Ray Budget          |
| Lighting is fully static (no time of day, no destructible lights) | Static DDGI is sufficient                    |
| Lighting changes at runtime | [Dynamic RT DDGI](./DDGI-Dynamic.md)           |
| Shipping across a wide hardware range | Both, switched automatically by HW detection |

That last row is the common case and the reason Static DDGI is worth setting up even on a project targeting
dynamic lighting. Because the two modes share volumes and authoring, supporting both is a scalability
setting rather than a second lighting pass through every level.

## Setting up

<procedure title="Bake Static DDGI" id="bake-static-ddgi">
    <step>
        Place and size DDGI volumes exactly as you would for the dynamic mode. Volume placement and probe
        density authoring is shared.
    </step>
    <step>
        Switch the volume to its static mode.
    </step>
    <step>
        Finalise your lighting. Anything that changes after the bake will not be reflected in the probes.
    </step>
    <step>
        Bake. Iterate freely &mdash; the bake is fast enough that you can treat it as part of the
        lighting loop rather than an overnight job.
    </step>
    <step>
        Verify with moving objects in the scene, since that is where Static DDGI most visibly beats
        lightmaps.
    </step>
</procedure>

## Limitations

Static DDGI is baked, so everything that implies applies: no time-of-day, no lights that move or change
intensity contributing bounce, no bounce from destructible geometry after it is destroyed. Direct lighting
can still be fully dynamic; it is only the indirect bounce that is frozen.

Probe spacing still bounds spatial resolution, so the same reasoning about contact detail applies. Combining
with [SSGI](./SSGI.md) is worthwhile here too, and SSGI has no ray tracing requirement, so it remains
available on the same hardware.

Note that Vite disables lightmap UV generation by default on import &mdash; see
[Engine Default Changes](../Performance/Engine-Defaults.md). If you intend to use traditional lightmaps alongside or instead
of Static DDGI, you will need to re-enable it.

<note>
It's feasible to combine traditional LightMass with Static or Dynamic DDGI, the GI does blend nicely 
and in general there won't be artifacts or crush out calculations from the GI related math.
</note>



## See also

- [Dynamic DDGI](./DDGI-Dynamic.md)
- [Global Illumination](./Global-Illumination.md)
- [SSGI](./SSGI.md)
- [Scalability](../Performance/Engine-Defaults.md)
- [Platform Support](../Platforms/Platforms.md)
