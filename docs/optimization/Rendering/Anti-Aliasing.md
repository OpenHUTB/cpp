# Anti-Aliasing

<tldr>
<p>
Vite adds <b>SMAA</b> as a fourth antialiasing method alongside None, FXAA, TemporalAA and MSAA. SMAA is
the recommended for hihest raw Image Quality and specially competitive titles.
Vite improves upon <b>UE4's TAA</b> Image quality with both improved stabalization and improved Colour
Reproduction of Materials, specially Glossy ones and textures sharpness.
</p>
</tldr>

Antialiasing is where Vite’s rendering philosophy is most apparent. Unlike Epic’s Unreal Engine 5, Vite renders at native resolution by default and targets high frame rates. Its antialiasing solution therefore prioritizes temporal stability while preserving the image-quality benefits of a full native 4K output, 
whose higher pixel density already reduces aliasing compared with lower internal resolutions. Compared with UE5, Vite effectively uses supersampling when the two engines are evaluated at the same output resolution but different internal rendering resolutions.

<note>
Epic’s UE 5.8 defaults to an internal resolution with only one-quarter of the target pixel count—a 50% scale on each axis. 
This behavior appears across several rendering paths: even when TSR is disabled, the engine may "insist" to fall back to another non-native scaling method, 
such as TAAU or basic spatial upscaling. Vite has no such behavior, it always defaults to the Screen Full Native Resolution.
When comparing Vite with UE5, always verify that both engines are rendering at the same native internal 
resolution. Use stat unit to confirm the active resolution and performance characteristics.
</note>

## Available methods

The **Anti-Aliasing Method** setting under **Project Settings > Engine > Rendering > Default Settings**
offers:

| Method | Enum | `r.DefaultFeature.AntiAliasing` | Notes                                                                                                |
|---|---|---|------------------------------------------------------------------------------------------------------|
| None | `AAM_None` | 0 | Aliased. Useful for comparison and for debugging AA artefacts.                                       |
| FXAA | `AAM_FXAA` | 1 | Cheap post-process filter. Blurs texture detail along with edges. Vite implements a higher IQ option |
| TemporalAA | `AAM_TemporalAA` | 2 | Vite improves over UE4 TAA with higher IQ and stability. Better AA Handling with a cost on Image Quality |
| MSAA | `AAM_MSAA` | 3 | Forward shading only. Sample count via `r.MSAACount`.                                                |
| **SMAA** | `AAM_SMAA` | **4** | Vite's SMAA implementation faster and higher quality than other stock Morphological solutions        |

## SMAA

Subpixel Morphological Anti-Aliasing analyses the image for edge patterns and blends across them
geometrically. It is a spatial technique: it looks at one frame and resolves the aliasing in that frame.

That single property is why it fits Vite. Temporal techniques accumulate samples across frames, which is
what produces ghosting behind moving objects, smearing on fast camera motion, and the general softness that
makes TAA output look like it is rendered behind a thin layer of vaseline. SMAA has no history buffer, so it
has none of those failure modes. What it gives up is subpixel detail reconstruction &mdash; it cannot
recover information that was never rendered &mdash; which matters much less when you are rendering at native
4K in the first place.

Vite's bespoke SMAA implementation is 32% faster than CMAA2 in Unreal Engine, that's why CMAA2 was deemed redundant.
Other morphological solutions were evaluated; none of them provided any objective argument to be implemented in Vite 
the logical choice was to simply keep improving the base SMAA shader beyond its original form. 

### Enabling SMAA

```ini
; Config/DefaultEngine.ini
[/Script/Engine.RendererSettings]
r.DefaultFeature.AntiAliasing=4
```

Or per-camera through the post process volume's **Anti-Aliasing Method** override, or at runtime:

```
r.DefaultFeature.AntiAliasing 4
```

### Quality

| CVar | Default | Values |
|---|---|---|
| `r.Vite.SMAA.Mode` | `1` | `0` = Low, `1` = High |

Marked `ECVF_Scalability`, so it can be driven from scalability groups. High quality is the default and is
what the [performance targets](Performance-Targets.md) assume; Low exists for the lowest scalability bucket
where the difference in edge quality is a reasonable trade for the frame time.

### Where SMAA runs in the pipeline

Vite applies SMAA **before tonemapping**, unlike FXAA which runs after. This is deliberate: resolving edges
in linear HDR space avoids the tonemapper amplifying partially-resolved edge pixels into visible fringing on
high-contrast boundaries, which is a common complaint about post-tonemap SMAA implementations.

The practical consequence is that post-process materials in the **After Tonemapping** blend location see an
already-anti-aliased image, and that SMAA composes correctly with HDR output.

### Debugging

In non-Shipping builds:

| CVar | Values |
|---|---|
| `r.AntiAliasing.SMAA.Debug` | `0` = off, `1` = show detected edges, `2` = show blend weights |

Edge visualisation is the fastest way to diagnose SMAA that appears to be doing nothing (usually the method
is not actually selected) or is over-blurring (usually excessive high-frequency content in the source image,
often from an aggressive sharpening or noise post-process).

## Aliasing that anti-aliasing will not fix

Some aliasing is authored in rather than introduced by rasterisation, and no regular AA method resolves it:

- **Specular aliasing** from high-frequency normal maps on smooth materials. Fix with proper mip generation
  and normal-to-roughness conversion, not with AA.
- **Texture aliasing** from missing or badly generated mips. Fix in the texture import settings.
- **Alpha-test shimmer** on foliage. Consider dithered opacity, or reduce the alpha-tested surface area.

Diagnose these by switching to `AAM_None` and looking at where the aliasing appears. Aliasing that moves
with the surface rather than sitting on silhouette edges is a content problem.

## See also

- [Rendering](Rendering.md)
- [Upscalers and Frame Generation](Upscalers.md)
- [Performance Targets](Performance-Targets.md)
- [Colour Management](Color-Management.md)
