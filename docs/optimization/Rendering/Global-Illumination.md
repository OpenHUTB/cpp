# Global Illumination

<tldr>
<p>
Vite's recommended GI setup is <b>Dynamic DDGI plus SSGI</b>. DDGI resolves world-scale bounce without
noise; SSGI fills in the high-frequency contact detail that probe volumes are too coarse to capture.
Enable with <code>r.GlobalIllumination.ExperimentalPlugin 1</code> and <code>r.SSGI.Enable 1</code>.
</p>
</tldr>

Global illumination is the single biggest reason Vite exists as a separate fork. This page explains the
options and how to choose between them; the individual techniques have their own pages.

## The options

| Solution                          | Cost                 | Requires DXR | Dynamic lighting | Best for                                                          |
|-----------------------------------|----------------------|---|---|-------------------------------------------------------------------|
| [Dynamic DDGI](DDGI-Dynamic.md)   | Low                  | Yes | Fully dynamic | Almost everything                                                 |
| [Static DDGI](DDGI-Static.md)     | Near zero at runtime | No | Baked | Low-end and no-DXR hardware                                       |
| [SSGI](SSGI.md)                   | Low                  | No | Fully dynamic | High Frequency Detail, made to run alongside DDGI                 |
| [Per-pixel RT GI](Ray-Tracing.md) | High                 | Yes | Fully dynamic | GPUs non trivially faster than PS5, Reference                     |
| Path-Tracing                      | Ultra                | Yes | Fully dynamic | RTX 5080 and above, Ground Truth Reference                        |
| Baked lightmaps                   | Zero at runtime      | No | Static only | Fully static scenes, Always use CPU LightMass for maximum quality |

<note>
"Vite includes several secondary Indirect Bounce solutions, such as Distance Fields Bounce and IBL capture (Vite specific addition). 
These are rarely considered over the solutions listed above, except for very specific target platforms or specialized setups."
</note>

## Why DDGI rather than Lumen

Dynamic Diffuse Global Illumination stores irradiance in a grid of probes and filters it using spherical
harmonics. Because the representation is smooth by construction, the result is noise-free without a
denoiser &mdash; which is the root cause of most of its advantages over Lumen.

Against Software Lumen, DDGI provides higher quality bounce and less light leaking. Against Hardware Lumen,
it is comparable for bounce quality while typically running around twice as fast (End Scene FPS not just the isolated GI cost).
In one representative test scene at 1440p native on an RTX 4080 Super, DDGI measured 811 FPS against Lumen 5.7's 324 FPS. 
On AMD hardware the technique holds up well: the same class of test scene runs at 245 FPS at 1080p native on an
RX 6600.

DDGI is also not experimental technology. Implementations ship in Metro Exodus, Overwatch 2, The Finals,
Control, The Witcher 3, Warhammer 40,000: Darktide, DOOM: The Dark Ages, Indiana Jones and the Great Circle,
007 First Light, Ghost of Yotei and Star Wars Outlaws including its Switch 2 version. AAA engines including
Anvil and Snowdrop use DDGI probes as part of their ray-traced GI pipelines. The technique was designed to
scale across a wide hardware range, starting from Xbox One S GPU for Static Mode and GTX 1060 class GPUs for Dynamic RT mode.

<img src="../../img/optimization/StylizedRTDemo.png" alt="Stylized scene lit by Dynamic DDGI with the frame counter reading 811 FPS" border-effect="line"/>

*811 FPS, RTX 4080 Super, 1440p native, Dynamic DDGI. The same scene on Lumen in 5.7 measures 324 FPS.*

<img src="../../img/optimization/DDGIEmissiveSurfaces.png" alt="Interior lit by emissive surfaces contributing to DDGI" border-effect="line"/>

*Emissive materials contribute to DDGI directly, so a scene can be lit from emissive geometry without
placing light actors for it.*

## Why pair DDGI with SSGI

Probe volumes have a spatial resolution. Detail smaller than the probe spacing &mdash; the darkening where a
chair leg meets the floor, bounce inside a narrow gap, contact shading under a desk &mdash; is not
represented, because there is no probe there to represent it.

Screen-space GI operates at pixel resolution and captures exactly that. The two techniques have shortcomings in opposite
directions: SSGI has no information about anything off-screen or occluded, while DDGI has complete world
knowledge at coarse resolution. Running both gives you world-scale bounce from DDGI and high-frequency
contact detail from SSGI. This is an officially recommended setup by NVIDIA from their Unreal Engine DDGI 
presentations. Vite's SSGI is configured to work alongside DDGI from the get-go.

This is not possible in UE5. SSGI regressed in both quality and performance when it was folded into Lumen,
and can no longer be enabled alongside a separate GI solution. In Vite it is the UE4-era implementation and
composes cleanly.

## Enabling the recommended setup

```c++
IConsoleManager::Get().FindConsoleVariable(TEXT("r.GlobalIllumination.ExperimentalPlugin"))->Set(1);
IConsoleManager::Get().FindConsoleVariable(TEXT("r.SSGI.Enable"))->Set(1);
```

Or in configuration, which is preferable for shipping projects because it participates in scalability:

```ini
; Config/DefaultEngine.ini
[/Script/Engine.RendererSettings]
r.GlobalIllumination.ExperimentalPlugin=1
r.SSGI.Enable=1
```

You then need to place DDGI volumes in the level. See [Dynamic DDGI](DDGI-Dynamic.md) for volume setup,
probe density and the settings that matter.

## Choosing for your hardware floor

<procedure title="Pick a GI configuration" id="pick-gi">
    <step>
        If your minimum spec has DXR support, use Dynamic DDGI plus SSGI. This is the default
        recommendation and covers GTX 1060 6&nbsp;GB and above.
    </step>
    <step>
        If your minimum spec has no DXR support at all, use
        <a href="DDGI-Static.md">Static DDGI</a>. It bakes almost instantly, gives better bounce fidelity
        than traditional baked lighting, and handles moving objects better because the probe volumes
        cover the space rather than the surfaces.
    </step>
    <step>
        If you need to support both, ship Static DDGI as a scalability fallback. The volumes and authoring
        are shared between the two modes, so this is a scalability setting rather than a second lighting
        pass through the level.
    </step>
    <step>
        Only reach for per-pixel ray-traced GI if you are producing reference imagery or
        previsualisation. It is far more expensive than DDGI and its advantage does not survive a
        frame-time budget. It is also compiled out of a default build and requires rebuilding with
        <code>VITE_RT_PSO_DEBLOAT=0</code> &mdash; see
        <a href="Compile-Time-Switches.md">Compile-Time Switches</a>.
    </step>
</procedure>

## See also

- [Dynamic DDGI](DDGI-Dynamic.md)
- [Static DDGI](DDGI-Static.md)
- [SSGI](SSGI.md)
- [Ray Tracing](Ray-Tracing.md)
- [Performance Targets](Performance-Targets.md)
