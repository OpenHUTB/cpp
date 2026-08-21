# Colour Management

<tldr>
<p>
Vite uses the stock Unreal Engine 4.27 tonemapping and colour pipeline: ACES film tonemapper, post process
volume grading, LUTs and HDR output. This page covers the controls and the settings that most often cause
problems.
</p>
</tldr>

Colour management is the last stage of the frame and the one that determines whether all the work done by
the [lighting](./Global-Illumination.md) and [ray tracing](./Ray-Tracing.md) systems actually reaches the
display looking correct.

## The pipeline

The renderer works in linear HDR throughout. At the end of the frame:

1. **[SMAA](./Anti-Aliasing.md)** resolves edges, in linear space, before tonemapping.
2. **Colour grading** applies the post process volume's exposure, white balance, saturation, contrast,
   gain, gamma and offset controls, plus any colour grading LUT.
3. **Tonemapping** maps the HDR range into the display range using the ACES filmic curve.
4. **Output encoding** applies the transfer function for the target display, sRGB for SDR or PQ/ST-2084 for
   HDR.

## Tonemapper controls

| CVar | Default | Purpose |
|---|---|---|
| `r.TonemapperFilm` | `1` | Use the ACES film tone mapper. `0` reverts to the legacy curve. |
| `r.Tonemapper.Quality` | `5` | `0` basic only, `1` + film contrast, `2` + vignette, `3` + film shadow tint, `4` + grain, `5` + grain jitter (full quality) |
| `r.Tonemapper.Sharpen` | `0` | Sharpening in the tonemapper, clamped at 10. `0.5` half strength, `1` full strength. |
| `r.Tonemapper.GrainQuantization` | `1` | `1` adds a high-frequency pixel pattern to fight 8-bit colour quantisation. `0` is slightly faster. |
| `r.TonemapperGamma` | `0.0` | `0` uses the default sRGB or Rec709 transform; any other value forces a fixed gamma. |
| `r.Gamma` | `1.0` | Gamma applied on output |

`r.Tonemapper.Quality` is a genuine scalability lever. Each step down removes a feature from the tonemapper
shader, producing a cheaper permutation. If your project does not use vignette, film shadow tint or grain,
dropping the quality level costs nothing visually and saves both frame time and shader permutations.

`r.Tonemapper.Sharpen` deserves care. A small amount of tonemapper sharpening can compensate for perceived
softness, but it operates after [anti-aliasing](./Anti-Aliasing.md) and will re-introduce aliasing on edges
SMAA just resolved. If the image looks soft, first confirm that you are actually rendering at native
resolution and that no upscaler is active.

## Colour grading

All grading is done through post process volumes under **Color Grading**. The controls are organised into
Temperature, Global, Shadows, Midtones and Highlights, each offering saturation, contrast, gamma, gain and
offset.

Grading in Unreal is applied before tonemapping, in linear space. This is why grading values that look
correct in a 2D image editor do not transfer directly &mdash; you are grading scene-referred linear data,
not display-referred pixels.

**LUTs.** A colour grading LUT can be assigned in the post process volume with a blend weight. Author LUTs
against a neutral capture of your scene rather than against an already-graded one, or the grades compound.

## HDR output

| CVar | Purpose |
|---|---|
| `r.AllowHDR` | Enables HDR output support for the project. Usually set per-project or per-platform in `DefaultEngine.ini`. |
| `r.HDR.Display.OutputDevice` | Selects the output transfer function |
| `r.HDR.Display.ColorGamut` | Selects the output colour gamut |

```ini
; Config/DefaultEngine.ini
[/Script/Engine.RendererSettings]
r.AllowHDR=1
```

HDR output interacts with SMAA correctly because [SMAA runs before tonemapping](./Anti-Aliasing.md) in Vite.
Anti-aliasing implementations that run after the tonemapper generally have to be reworked for HDR output;
this one does not.

Slate and UI are composited with knowledge of the output device via `r.TonemapperGamma`, but UI authored
against an SDR reference will look wrong in HDR. Budget time to check your UI in HDR specifically rather
than assuming it transfers.

## Common problems

<deflist>
<def title="The scene looks washed out or milky">
Usually excessive ambient or fog rather than a grading problem. Check whether
<a href="./Ambient-Occlusion.md">AO</a> is being applied and whether your
<a href="./Global-Illumination.md">GI</a> intensity is too high. Compare against a
<a href="./Path-Tracing.md">path-traced</a> reference before reaching for contrast in the grade.
</def>
<def title="Colours shift between the editor viewport and packaged builds">
Almost always a different output device or a post process volume that is not unbound. Confirm
<code>r.HDR.Display.OutputDevice</code> matches in both, and check for editor-only post process volumes.
</def>
<def title="Banding in gradients and skies">
Check <code>r.Tonemapper.GrainQuantization</code> is <code>1</code>. If banding persists on an HDR display,
the source gradient itself may be quantised &mdash; check the sky texture or gradient material precision.
</def>
<def title="The image is sharp in the editor and soft in game">
Confirm no upscaler is active. See <a href="./Upscalers.md">Upscalers and Frame Generation</a>. Vite renders
at native resolution by default; an upscaler enabled in a game settings menu is the usual cause.
</def>
</deflist>

## See also

- [Anti-Aliasing](./Anti-Aliasing.md)
- [Path Tracing](./Path-Tracing.md)
- [Upscalers and Frame Generation](./Upscalers.md)
- [Rendering](./Rendering.md)
