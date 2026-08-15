# Shading Models

<tldr>
<p>
Vite adds three shading models to the stock 4.27 set: <b>Callisto BRDF</b> for high-fidelity skin and
character surfaces, <b>Toon</b> for stylised non-photorealistic rendering, and <b>Lit Reactive</b>.
Each repurposes existing material pins, so read the pin mapping tables before authoring.
</p>
</tldr>

Shading models are selected from the **Shading Model** dropdown in the material's details panel, or driven
per-pixel through the **Shading Model** material expression. All stock Unreal Engine 4.27 shading models
remain available.

## Added shading models

| Shading model | Display name | Purpose |
|---|---|---|
| `MSM_CallistoBRDF` | Callisto BRDF | High-fidelity character and skin surfaces with single- and dual-lobe GGX specular |
| `MSM_Toon` | Toon | Stylised cel shading, inspired by Guilty Gear |
| `MSM_LitReactive` | Lit Reactive | Lit surface variant with anisotropy and tangent support |

## Callisto BRDF

Callisto BRDF provides single- and dual-lobe GGX specular with specular Fresnel falloff, aimed at
characters and skin where the default lit model's single specular lobe is too crude. The
[Callisto BRDF demo projects](Callisto-BRDF-Demos.md) show it applied to male and female character
surfaces.

### Pin mapping

The model repurposes several standard material pins. The material editor relabels them automatically once
the shading model is selected, but knowing the mapping helps when reading material graphs or writing
material functions.

| Standard pin | Callisto BRDF label | Purpose |
|---|---|---|
| Opacity | **Retroreflection** | Retroreflective response strength |
| Ambient Occlusion | **Retroreflection Falloff** | Falloff curve for the retroreflection term |
| Anisotropy | **Diffuse Fresnel** | Diffuse Fresnel term |
| Custom Data 0 | **Smooth Terminator** | Softens the light terminator across the shadow boundary |
| Custom Data 1 | **Diffuse Fresnel Falloff** | Falloff curve for the diffuse Fresnel term |

### Callisto Advanced Params

An additional custom output node, **Callisto Advanced Params**, exposes parameters that do not fit the
standard pin set:

| Input | Purpose |
|---|---|
| Specular Fresnel Falloff | Falloff curve for the specular Fresnel response |

Add the node to the material graph the same way as any other custom output. It compiles to the
`CallistoAdvancedParams` material function.

### Authoring notes

The smooth terminator control is the one most worth understanding. The hard light terminator produced by
standard GGX shading is a well-known artefact on curved organic surfaces, particularly faces lit from a
grazing angle, where the shading boundary appears unnaturally sharp. The smooth terminator input softens
that transition without flattening the overall shading.

The retroreflection and diffuse Fresnel controls together let you dial in surfaces that brighten toward
grazing angles &mdash; skin, fabric with fine fibres, dusty surfaces &mdash; without resorting to a
subsurface model.

## Toon

The Toon shading model provides cel shading with explicit artistic control over the shading ramp, specular
shape and shaded colour, inspired by Arc System Works' approach in Guilty Gear.

### Pin mapping

Toon repurposes more pins than any other model, because most of the standard PBR parameters have no
meaning under cel shading.

| Standard pin | Toon label | Purpose |
|---|---|---|
| Metallic | **Specular Brightness** | Intensity of the specular highlight |
| Specular | **Specular Size** | Size of the specular highlight |
| Roughness | **Shadow Bias** | Shifts the shading threshold between lit and shaded |
| Anisotropy | **Softness** | Softness of the lit/shaded transition |
| Subsurface Color | **Shaded Color** | Explicit colour used in the shaded region |

**Shaded Color** is the key control. Rather than deriving shadow colour by darkening the base colour, you
specify it directly, which is what gives cel-shaded art its characteristic non-physical colour shifts in
shadow &mdash; skin that goes purple rather than dark brown, for instance.

**Shadow Bias** and **Softness** together define the ramp. A bias near the middle with near-zero softness
gives a hard two-tone look; increasing softness moves toward a gradient.

## Lit Reactive

A lit surface variant supporting anisotropy, tangent input and Custom Data 0. It is treated as a basic
opaque surface for the purposes of the opacity and opacity-mask optimisation path, alongside Default Lit,
which means it avoids the masked and translucent-only attribute compilation that other lit models trigger.

## Shader permutations

Every shading model in use adds shader permutations, which cost compile time, PSO count and disk space.
This is a real budget rather than a theoretical concern &mdash; permutation growth was one of the specific
regressions Vite exists to avoid, as described in
[UE4 versus UE5 Cost Analysis](UE4-Versus-UE5-Cost-Analysis.md).

> Use the smallest set of shading models your art direction needs. A project that ships Default Lit,
> Callisto BRDF for characters and nothing else will compile faster, have fewer PSOs and load faster than
> one that uses six models across its material library.
>
{style="note"}

See [Shader Compilation and PSO](Shader-Compilation-And-PSO.md) for how Vite manages permutations, and note
that runtime PSOs are debloated in Shipping configuration.

## See also

- [Rendering](Rendering.md)
- [Callisto BRDF Demos](Callisto-BRDF-Demos.md)
- [Hair Rendering](Hair-Rendering.md)
- [Shader Compilation and PSO](Shader-Compilation-And-PSO.md)
- [Colour Management](Color-Management.md)
