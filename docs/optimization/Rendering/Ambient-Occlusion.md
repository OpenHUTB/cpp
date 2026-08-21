# Ambient Occlusion

<tldr>
<p>
Three options: Vite's <b>optimised SSAO</b> (on by default, free), <b>HBAO+</b> (higher quality,
post-process volume controlled) and <b>RTAO</b> (ray traced, most accurate, most expensive).
They compose &mdash; HBAO+ multiplies over the SSAO buffer.
</p>
</tldr>

Ambient occlusion darkens creases, contact points and enclosed areas that ambient light should not fully
reach. It is one of the cheapest ways to make a scene read as grounded, and one of the most common places to
overspend.

## Vite optimised SSAO

Vite rewrites the memory access pattern of the stock UE4.27 screen-space AO pass. It produces the same
result as the stock path at lower cost, and is enabled by default.

| Control | Default | Notes |
|---|---|---|
| `VITE_O_SSAO` (compile-time) | `1` | Shipping builds are locked to this value |
| `r.Vite.SSAO` (runtime) | `VITE_O_SSAO` | Development builds only. `0` = stock UE path, `1` = optimised path |

The runtime CVar exists so you can A/B the optimised path against stock during development, which is how the
optimisation was validated. In Shipping the CVar does not exist at all &mdash; the path is compiled in.
See [Compile-Time Switches](../Performance/Compile-Time-Switches.md).

Standard UE4.27 SSAO settings all still apply and are found under **Rendering Features > Ambient Occlusion**
in a post process volume: Intensity, Radius, Quality, Power, Bias, Fade Out Distance and the rest.

## HBAO+

Horizon-Based Ambient Occlusion is NVIDIA's higher-quality AO technique, inherited through the NvRTX
branch. It samples the depth buffer along horizon directions rather than in a screen-space sphere, which
produces more accurate occlusion in creases and less of SSAO's characteristic haloing around object
silhouettes.

HBAO+ **multiplies over** the screen-space AO buffer rather than replacing it, so both run when it is
enabled.

### Enabling

| CVar | Default | Effect |
|---|---|---|
| `r.HBAO.Enable` | `0` | Master enable |
| `r.HBAO.HighPrecisionDepth` | `0` | `0` = FP16 internal depth, `1` = FP32. Use FP32 to avoid self-occlusion banding on distant objects. |
| `r.HBAO.GBufferNormals` | `1` | `0` = reconstruct normals from depth, `1` = fetch GBuffer normals |

<note>
The CVar help text describes HBAO+ as DX11-only. Vite implements it on both D3D11 and D3D12; the help
string is inherited from the original NVIDIA integration and has not been updated.
</note>

### Post process volume settings

Once enabled, HBAO+ is tuned per post process volume under the **HBAO+** category. These are Blueprint
read/write, so they can be driven at runtime.

| Setting | Default | Range | Purpose |
|---|---|---|---|
| Power Exponent | `2.0` | 0&ndash;4 | Darkening curve applied to the AO result. Higher is more contrasty. |
| Radius | `2.0` | 0.1&ndash;2 | World-space sampling radius in metres |
| Bias | `0.1` | 0&ndash;0.2 | Rejects samples below this angle. Raise to remove self-occlusion on flat surfaces. |
| SmallScale AO | `1.0` | 0&ndash;1 | Weight of fine-detail occlusion |
| Blur Radius | 2 pixels | Disabled / 2px / 4px | Noise reduction blur |
| Blur Sharpness | `16.0` | 0&ndash;32 | Edge preservation during blur. Higher preserves more edges. |
| Max Depth | `9500.0` | 0&ndash;400+ | View depth beyond which HBAO+ stops being computed |
| Depth Sharpness | `50.0` | 0&ndash;100 | Depth discontinuity sensitivity |
| Clamp Foreground AO | off | &mdash; | Limits AO on near geometry |
| Foreground AO Distance | `100.0` | 0&ndash;1000 | Distance used by the foreground clamp |
| Clamp Background AO | off | &mdash; | Limits AO on distant geometry |
| Background AO Distance | `1000.0` | 0&ndash;10000 | Distance used by the background clamp |

**Radius** is the setting that matters most and is most often wrong. It is in metres, and the correct value
depends entirely on scene scale. An interior with human-scale props wants a small radius; a large exterior
wants a larger one. Too large produces soft grey wash across everything; too small produces occlusion only
in the tightest creases.

**Max Depth** is a straightforward performance lever. Distant geometry contributes very little visible AO,
so lowering this value reduces cost with minimal visual change. Check the result against a fly-through
before committing to an aggressive value.

## Ray-traced ambient occlusion

RTAO is covered in detail in [RT Shadows and Ambient Occlusion](../Rendering/RT-Shadows-And-Ambient-Occlusion.md). In
brief: it traces actual occlusion rays, so it handles off-screen occluders correctly and does not suffer
from screen-space AO's fundamental limitation of only knowing about what is visible.

It is also the most expensive of the three, and requires DXR.

## Choosing

| Target | Recommended configuration |
|---|---|
| Stylised 4K120 | Vite SSAO only |
| Performance High End 4K60 | Vite SSAO, HBAO+ if there is headroom |
| Fidelity High End 4K30 | Vite SSAO + HBAO+ |
| Fidelity Full RT 1440p30 | RTAO |

The general rule is that AO should be the cheapest thing in your frame that produces its result. If
[DDGI](../Rendering/DDGI-Dynamic.md) is already resolving the indirect lighting in a space correctly, heavy AO on top of
it is double-darkening &mdash; you are subtracting light that the GI solution never added.

<warning>
Watch for AO stacking. Enabling material AO, SSAO, HBAO+ and RTAO simultaneously produces a scene that is
much darker in contact areas than it should be. Each layer multiplies. Compare against a
<a href="../Rendering/Path-Tracing.md">path-traced</a> reference if you are unsure whether your contact shadows are
physically plausible or just dark.
</warning>

## See also

- [RT Shadows and Ambient Occlusion](../Rendering/RT-Shadows-And-Ambient-Occlusion.md)
- [Global Illumination](../Rendering/Global-Illumination.md)
- [SSGI](../Rendering/SSGI.md)
- [Compile-Time Switches](../Performance/Compile-Time-Switches.md)
- [Performance Targets](../EngineOverview/Performance-Targets.md)
