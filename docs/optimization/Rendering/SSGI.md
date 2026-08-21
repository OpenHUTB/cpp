# SSGI

<tldr>
<p>
UE4-era screen-space global illumination, enabled with <code>r.SSGI.Enable 1</code>. Recommended
<i>alongside</i> <a href="./DDGI-Dynamic.md">DDGI</a> rather than instead of it: DDGI supplies world-scale
bounce, SSGI supplies high-frequency contact detail. No ray tracing hardware required.
</p>
</tldr>

Screen-space global illumination gathers indirect light by sampling the depth and colour buffers. It only
knows about what is on screen, which is simultaneously its great limitation and the reason it complements a
world-space solution so well.

## Why this matters in Vite specifically

SSGI experienced both quality and performance regressions in UE5 as a result of being integrated into
Lumen, and it is no longer possible to activate it alongside a separate GI solution there.

Vite retains the UE4-era implementation, which is both faster and composable. Running SSGI in tandem with
DDGI is the recommended default configuration, not an exotic combination.

## What each technique contributes

| | DDGI | SSGI |
|---|---|---|
| Spatial resolution | Probe grid | Per-pixel |
| Knows about off-screen geometry | Yes | No |
| Knows about occluded geometry | Yes | No |
| Captures contact-scale detail | No | Yes |
| Requires ray tracing hardware | Yes (dynamic mode) | No |
| Stable under camera motion | Yes | Screen-space artefacts at frame edges |

The two fail in opposite directions, which is exactly what you want from a pair of techniques. DDGI has
complete world knowledge at coarse resolution; SSGI has pixel resolution over an incomplete view. Together
they cover both scales.

Concretely, SSGI is what gives you the darkening where a chair leg meets the floor, bounce inside a narrow
gap between two objects, and the colour bleed from a nearby red wall onto a character's shoulder &mdash; all
features smaller than typical probe spacing.

## Enabling

```c++
IConsoleManager::Get().FindConsoleVariable(TEXT("r.SSGI.Enable"))->Set(1);
```

```ini
; Config/DefaultEngine.ini
[/Script/Engine.RendererSettings]
r.SSGI.Enable=1
```

SSGI quality is controlled through the standard UE4 SSGI console variables and the post process volume. The
quality setting trades ray count and resolution against cost in the usual way.

## Limitations to design around

Everything screen-space shares the same failure modes, and it is worth knowing them so you do not chase
them as bugs:

**Off-screen light is missing.** A brightly lit wall just outside the frustum contributes nothing. As the
camera turns and it comes into view, its contribution appears. This is why SSGI alone is not a viable GI
solution and why the pairing matters.

**Occluded geometry is missing.** SSGI can only see the depth buffer's front surface. Light that should
bounce from behind an object is not represented.

**Frame edges.** Contributions fade out towards the screen border because the sampling kernel runs out of
buffer. This is usually acceptable but becomes visible with fast camera motion.

Because DDGI is providing the world-scale answer, none of these produce a *wrong* image in the combined
setup &mdash; they produce a locally less detailed one, which is a much better failure mode.

## Cost

SSGI is cheap relative to any world-space technique, and it scales with resolution and quality setting
rather than with scene complexity. On the [Performance Targets](../EngineOverview/Performance-Targets.md) that include it, it
is not the dominant cost; DDGI and reflections are.

If you are hunting frame time, measure before you disable it. `stat gpu` will tell you what the pass
actually costs in your scene, and it is frequently less than people assume.

## See also

- [Global Illumination](./Global-Illumination.md)
- [Dynamic DDGI](./DDGI-Dynamic.md)
- [Static DDGI](./DDGI-Static.md)
- [Ambient Occlusion](./Ambient-Occlusion.md)
- [Console Variable Reference](../Reference/Console-Variables.md)
