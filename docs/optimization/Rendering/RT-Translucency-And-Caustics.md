# Ray-Traced Translucency and Caustics

<tldr>
<p>
Hybrid translucency lets rasterised and ray-traced translucency coexist. <code>r.RayTracing.Translucency 3</code>
plus absorption is the recommended high-quality setup. Mesh and water caustics are separate systems
enabled with <code>r.RayTracing.MeshCaustics.Enable</code> and <code>r.RayTracing.WaterCaustics.Type</code>.
</p>
</tldr>

These features come from NVIDIA's NvRTX 4.27 Caustics branch, which is Vite's base. They are the most
parameter-heavy part of the renderer, so this page is structured as a reference rather than a narrative.

<warning>
<b>Everything on this page is compiled out in a default build.</b> Ray-traced translucency, mesh caustics
and water caustics all have their shader permutations removed when <code>VITE_RT_PSO_DEBLOAT</code> is
<code>1</code>, which is the default. Their console variables will set successfully and render nothing.
<p>
Rebuild with <code>VITE_RT_PSO_DEBLOAT=0</code> to use them. See
<a href="Compile-Time-Switches.md">Compile-Time Switches</a>.
</p>
</warning>

These are the most expensive effects in the ray tracing suite, and none of them fit inside Vite's
[performance targets](Performance-Targets.md) at 4K. They are documented here because the NvRTX lineage
provides them and some projects will want them for cinematics, marketing captures or specific set pieces.

> All code and art assets of the RTX features inherited from the NvRTX Caustics branch are covered by the
> [GameWorks license](https://developer.nvidia.com/gameworks-source-sdk-eula).
>
{style="note"}

## Translucency modes

Stock UE4 ray-traced translucency forces *all* translucency through the ray tracing pipeline, which makes
unsupported primitive types such as Cascade particles disappear, and produces refraction behaviour that
interacts unintuitively with content authored for rasterisation. Hybrid translucency solves this.

| `r.RayTracing.Translucency` | Mode |
|---|---|
| `0` | Stock UE4 ray-traced translucency off |
| `1` | Stock UE4 ray-traced translucency on |
| `2` | Hybrid Translucency 1 &mdash; ray-traced translucent reflection only |
| `3` | Hybrid Translucency 2 &mdash; ray-traced translucent reflection and refraction |

Hybrid modes require the **Hybrid Translucency** checkbox in project settings, because they permute the base
translucency shaders. The corresponding shader support switch is
`r.RayTracing.HybridTranslucencySupport`.

### Mode 2 &mdash; reflection only

Traces several layers of translucency to an off-screen surface, then composites them as part of normal
raster translucency. It loses order-independent transparency and refraction relative to full ray-traced
translucency, but it is no worse than raster in those areas while delivering ray-traced reflections and
shading.

| Console variable | Purpose |
|---|---|
| `r.RayTracing.Translucency.HybridLayers` | How many overlapping ray-traced translucency levels are tracked |
| `r.RayTracing.Translucency.HybridDepthThreshold` | World-space separation at which geometry counts as a different translucency layer. Too small and hybrid translucency will not appear, or z-fighting artefacts occur; too large and stacked layers incorrectly merge. |
| `r.RayTracing.Translucency.HalfRes` | 0 full, 1 half vertically (interleaved), 2 half checkerboard (4-tap), 3 half checkerboard (2-tap vertical) |

### Mode 3 &mdash; reflection and refraction

Mixes raster and *fully* ray-traced translucency, including reflection, refraction and OIT support. Cascade
particles can coexist with ray-traced translucency without losing its advantages. Order-independent
transparency is automatic in this mode.

This is the recommended mode for translucent meshes, used together with absorption.

Recommended settings for best visual quality:

```
r.RayTracing.Translucency.Refraction 1
r.RayTracing.Translucency.HybridLayers 5
r.RayTracing.Translucency.MaxRefractionRays 5
```

In 4.27, when RT refraction is enabled under mode 3, per-material IOR is bound to the PBR-based refraction
index input; the material's Refraction Mode input must be set to **Index Of Refraction**.

Mode 3 also lets you choose per-mesh whether a translucent static mesh participates in ray tracing at all.
Meshes excluded from ray tracing render through rasterisation, which lets you mix ray-traced and rasterised
translucency in the same scene and can improve performance by reducing ray tracing pipeline workload.

Additional notes for mode 3:

- `r.RayTracing.Translucency.HybridDepthThreshold` does not apply. Use
  `r.RayTracing.Translucency.PrimaryRayBias` instead to bias depth when determining layer ordering.
- Cascade and Niagara emitters spawning opaque meshes may disappear when fully occluded by ray-traced
  translucent meshes, because some particle types cannot generate the BVH data hardware ray tracing
  requires. This is a known stock UE4 behaviour that hybrid mode 2 does not change.
- Half-resolution refraction is available via `r.RayTracing.Translucency.HalfRes`: 0 full, 1 half
  checkerboard with weighted colour reconstruction, 2 half checkerboard interframe, 3 half checkerboard with
  average colour reconstruction. These reconstruction techniques are intended for use under TAA and need
  careful tuning under DLSS, which can amplify pixel-level reconstruction artefacts.

## Translucent absorption

With absorption enabled, thicker objects made of the same material appear less transparent, which is the
physically correct behaviour and a large fidelity win on glass and liquids. Absorption is a per-material
option, so objects with and without it render together.

<procedure title="Enable translucent absorption" id="enable-absorption">
    <step>Ensure ray-traced translucency is enabled.</step>
    <step>Set <code>r.RayTracing.Translucency.EnableAbsorption 1</code>, or check <b>Enable Absorption</b> in the post process volume.</step>
    <step>In the material editor, check <b>Ray Traced Translucency Absorption</b> on each material that should use it.</step>
</procedure>

Throughput culling reduces cost by discarding low-contribution ray paths:

| Console variable | Purpose |
|---|---|
| `r.RayTracing.Translucency.MinRefractionThroughput` | Higher values cull more refraction rays. May introduce artefacts. |
| `r.RayTracing.Translucency.MinReflectionThroughput` | Higher values cull more reflection rays. May introduce artefacts. |
| `r.RayTracing.PrimaryRays.AbsorptionForceShadingOnOpaqueObjects` | Set to 1 to eliminate double-shading artefacts on transparent reflections where actors occlude background. |

## Depth of field from primary rays

Rasterised depth of field handles translucency poorly, particularly where Cascade particles intersect
translucent glass. Ray tracing makes accurate cinematic DOF achievable.

<procedure title="Enable ray-traced depth of field" id="enable-rt-dof">
    <step>Use a cine camera and apply depth of field as normal.</step>
    <step>
        Collect every translucent material in the camera frustum, including translucent particles. For each
        root material, check <b>Output Translucency Depth</b> and ensure <b>Translucency Depth Opacity
        Threshold</b> is lower than the material instance's opacity parameter.
    </step>
    <step>Set <code>r.RayTracing.PrimaryRays.IncludeDOF 1</code>.</step>
</procedure>

Enhanced translucency mode 3, RT translucent reflection and refraction, and translucent absorption are all
strongly recommended alongside RT DOF. DLSS is supported automatically.

## Mesh caustics

Renders interactive caustics for translucent and metallic objects. Supports all four UE4 light types,
multiple light sources, reflective and refractive caustics, dispersion and soft caustics.

<img src="../../img/optimization/MeshCaustics.jpg" alt="POV-Ray Glasses scene with ray-traced refractive caustics cast onto a tiled surface" border-effect="line"/>

*The POV-Ray Glasses scene. Every bright pattern on the tile is a refractive caustic, traced rather than
authored.*

<img src="../../img/optimization/CausticsDispersion.jpg" alt="Prisms splitting a white beam into a spectrum through ray-traced dispersion" border-effect="line"/>

*Dispersion. Requires **Ray Traced Caustics Dispersion Amount** above 0 in the material root node together
with `r.RayTracing.MeshCaustics.EnableDispersion 1`.*

<procedure title="Enable mesh caustics" id="enable-mesh-caustics">
    <step>Enable ray tracing.</step>
    <step>Check <b>Enabled</b> under <b>Ray Tracing Mesh Caustics</b> in the post process volume, or set <code>r.RayTracing.MeshCaustics.Enable 1</code>.</step>
    <step>In light properties, check <b>Cast Mesh Caustics</b>.</step>
    <step>
        For metallic materials, check <b>Cast Ray Traced Reflection Caustics</b>. For translucent objects,
        check <b>Cast Ray Traced Reflection Caustics</b> for reflective caustics and <b>Cast Ray Traced
        Refraction Caustics</b> for refractive caustics.
    </step>
</procedure>

### Feature settings

Where a console variable is set to `-1`, the post process volume controls the actual value.

| Console variable | Purpose |
|---|---|
| `r.RayTracing.MeshCaustics.EnableTranslucentReflection` | Reflective caustics for transparent objects |
| `r.RayTracing.MeshCaustics.TranslucentReflectionMode` | 0 refractive only, 1 refractive plus reflective first bounce, 2 reflective for arbitrary bounces |
| `r.RayTracing.MeshCaustics.EnableDispersion` | Enables dispersion. Requires **Ray Traced Caustics Dispersion Amount** greater than 0 in the material root node. |
| `r.RayTracing.MeshCaustics.DispersionSamples` | Colour samples used for dispersion |
| `r.RayTracing.MeshCaustics.SoftCausticsSample` | Sample count for soft caustics. Requires **Mesh Caustics Softness** greater than 0 in light settings. |
| `r.RayTracing.MeshCaustics.EnableAdvancedSoftCaustics` | Higher quality soft caustics algorithm |

To expose dispersion amount per material instance, check **Ray Traced Caustics Use CustomData 0 As
Dispersion Amount** and connect a scalar to the Custom Data 0 channel, which makes the parameter tweakable
at runtime.

### Performance tuning

The key to performance is limiting photon count. Set the view mode to
**Ray Tracing Debug &rarr; Mesh Caustics Debug Data**, then set **Debug Light Data Type** to **Photon
Count**. Around 100k photons produces decent results in typical cases. If the count is too high, increase
**Adaptive Photon Size** and decrease **Adaptive Variance Gain** in the post process volume, and raise
**Final Cull Threshold** and **Mid Cull Threshold** until caustics begin to disappear.

| Console variable | Purpose |
|---|---|
| `r.RayTracing.MeshCaustics.FinalCullColorThreshold` | Culls low-contribution rays |
| `r.RayTracing.MeshCaustics.MidCullColorThreshold` | Culls low-contribution rays |
| `r.RayTracing.MeshCaustics.BufferScale` | -1 post process volume, 0 full, 1 half, 2 quarter resolution |
| `r.RayTracing.MeshCaustics.AdaptivePhotonSize` | Target screen-space photon size. Smaller is more detailed and more expensive. |
| `r.RayTracing.MeshCaustics.AdaptiveVarianceGain` | Higher values suppress flickering |
| `r.RayTracing.MeshCaustics.EnableTemporalFilter` | Temporal filtering to reduce flicker |
| `r.RayTracing.MeshCaustics.TemporalStrength` | Higher is more stable but can introduce lag or ghosting |
| `r.RayTracing.MeshCaustics.MaxTraceDepth` | Limits bounce count; raises performance for translucent objects |

## Water caustics

Renders interactive caustics for water areas from ponds to open sea. Supports all four light types, multiple
lights, reflective and refractive caustics, dispersion, soft caustics and cascaded caustics maps.

<img src="../../img/optimization/WaterCaustics.jpg" alt="Swimming pool floor lit by ray-traced water caustics with a character disturbing the surface" border-effect="line"/>

*The Swimming Pool scene under a single directional light. The caustics respond to surface disturbance
because they are traced against the water mesh each frame.*

<procedure title="Enable water caustics" id="enable-water-caustics">
    <step>Enable ray tracing.</step>
    <step>Select a water caustics type in the post process volume, or set <code>r.RayTracing.WaterCaustics.Type</code> to 1 or 2.</step>
    <step>In light properties, check <b>Cast Water Caustics</b>.</step>
    <step>
        On the water surface static mesh actor, check <b>Evaluate Ray Tracing Water Caustics</b> under the
        Ray Tracing tab. The water surface material must use a Translucent blend mode.
    </step>
</procedure>

### Choosing an algorithm

**Photon Difference Scattering (type 1)** is flexible and works with all light types UE4 supports. It needs
relatively high-resolution caustics maps to produce sharp patterns, so for large surfaces such as seas and
large lakes, enable cascaded caustics maps alongside it. Cascades keep caustics sharp near the camera while
rendering more cheaply at distance.

**Procedural Caustic Mesh (type 2)** produces very sharp caustics even from relatively low-resolution maps.
It does not work with cascaded caustics maps, because it does not need them. It is usually faster than PDS,
but it does not support area lights and can leave artefacts on the edges of caustics receivers.

### Settings

| Console variable | Purpose |
|---|---|
| `r.RayTracing.WaterCaustics.MaxReflectionRayDistance` | Set to 0 to disable reflective caustics |
| `r.RayTracing.WaterCaustics.MaxRefractionRayDistance` | Set to 0 to disable refractive caustics |
| `r.RayTracing.WaterCaustics.MapCascades` | Cascade count, up to 4 levels. Type 1 only. |
| `r.RayTracing.WaterCaustics.MapSizeX` / `MapSizeY` | Caustics map size, default 2048. 1024 is often enough for a small pond and saves substantial performance. |
| `r.RayTracing.WaterCaustics.NumDenoisePasses` | Default 2. Reduce to 0 for a sharper result. |
| `r.RayTracing.WaterCaustics.UseSceneLightDir` | Set to 0 to capture the caustics map from above the camera rather than along the light direction. Directional lights only. |
| `r.RayTracing.WaterCaustics.BufferScale` | 1 full, 2 half, 4 quarter resolution |
| `r.RayTracing.WaterCaustics.PhotonScale` | Initial photon size in PDS. Default 3. |
| `r.RayTracing.WaterCaustics.ShowPhoton` | Debug: draws photons as points |
| `r.RayTracing.WaterCaustics.RefractBackFaceCullingThreshold` | Set around -0.5 to ignore surface normals where cracks or seams appear |
| `r.RayTracing.WaterCaustics.ReflectBackFaceCullingThreshold` | As above, for reflective caustics |

Restricting water caustics to a single dynamic light saves a great deal of performance; light distance
culling and intensity fade are fully supported.

Caustics focus depends on mesh and normal strength: large waves or strong normal map values produce more
dramatic caustics. To enable dispersion, increase **Dispersion Intensity** in the post process volume and
tune **Dispersion Offset**.

## Sample content

NVIDIA provides project files and packaged demos for these features:

- [All project files](https://drive.google.com/drive/folders/1MfJ1rLqwx8acdscFfQtaYOR2Cdm1WPz9?usp=sharing)
- [All packaged demos](https://drive.google.com/drive/folders/1yHFOtmZWVDof8GbfZJMeazfb927ahTDn?usp=sharing)

Specific scenes: the POV-Ray Glasses scene for heavy refraction, a prism dispersion demonstration, the
Swimming Pool scene for water caustics, and the Office scene mixing particles, reflections, refractions,
mesh caustics and RTGI. See also [Abandoned Apartment](Abandone-Apartment.md) and
[Attic Scene](Attic-Scene.md) in this manual.

<img src="../../img/optimization/RTOfficeScene.jpg" alt="Office scene combining volumetric light, translucent spheres, ray-traced reflections and refractions" border-effect="line"/>

*The Office scene is the one that exercises the whole stack at once: particles, translucent reflection and
refraction, mesh caustics and ray-traced GI in a single frame.*

## See also

- [Ray Tracing](Ray-Tracing.md)
- [RT Reflections](RT-Reflections.md)
- [Upscalers and Frame Generation](Upscalers.md)
- [Console Variable Reference](Console-Variables.md)
