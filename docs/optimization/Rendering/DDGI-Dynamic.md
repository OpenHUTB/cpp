# Dynamic DDGI

<tldr>
<p>
Real-time ray-traced irradiance probe volumes. Enable with
<code>r.GlobalIllumination.ExperimentalPlugin 1</code>, then place DDGI volumes in the level.
Noise-free by construction, fully dynamic, and roughly 2x the frame rate of hardware Lumen in
comparable scenes.
</p>
</tldr>

Dynamic Diffuse Global Illumination is Vite's primary global illumination solution and the feature the fork
is best known for. This page covers how it works, how to set it up, and how to tune it.

<img src="DDGIDirectOnly.png" alt="Attic interior with direct lighting only, everything outside the sunbeams reading black" border-effect="line"/>

*Direct lighting only. Everything the sun does not reach directly is black, because nothing is carrying
light around the room.*

<img src="DDGIDirectPlusGI.png" alt="The same attic interior with Dynamic DDGI enabled, indirect bounce filling the room" border-effect="line"/>

*The same frame with Dynamic DDGI enabled. The ceiling, the boxes on the left and the space under the roof
line are all lit entirely by bounce &mdash; and there is no noise to denoise, because probe irradiance is
smooth by construction.*

## How it works

DDGI places a three-dimensional grid of probes across a volume. Each frame, a budget of rays is traced from
each probe into the scene; the resulting radiance is accumulated into per-probe irradiance and depth
textures, and stored using spherical harmonics.

Two properties follow from that representation, and they explain almost everything about DDGI's behaviour:

**It is noise-free.** Because irradiance is accumulated and filtered into a smooth basis over many frames
rather than sampled per-pixel per-frame, the output has no stochastic noise and needs no denoiser. That is
why DDGI does not exhibit the boiling, ghosting and temporal instability associated with denoised
ray-traced GI.

**It is coarse.** Spatial resolution is bounded by probe spacing. Lighting detail finer than the probe grid
does not exist in the representation. This is the reason [SSGI](SSGI.md) is recommended alongside it.

The per-probe depth information is what prevents light leaking through thin geometry &mdash; probes know how
far away the nearest surface is in each direction and can reject contributions that would have to pass
through a wall. This is why DDGI leaks less than software Lumen.

<img src="DDGIProbeVisualisation.png" alt="Editor viewport with DDGI probe spheres visualised throughout an interior" border-effect="line"/>

*Probe visualisation in the editor. Each sphere is one probe displaying its stored irradiance, which makes
probe spacing and any misplaced probes immediately visible.*

## Vite's integration

Vite's DDGI is not the stock 4.27 launcher plugin. It inherits NvRTX's engine-side integration, which
reaches into the ray tracing pipeline rather than sitting alongside it. The practical differences:

- **Probe-based ray-traced reflections.** Reflection rays can sample probe irradiance for their secondary
  bounce instead of returning black or falling back to a cubemap. This substantially improves RT reflection
  quality and is one of the larger visual wins in the fork. See
  [Ray-Traced Reflections](RT-Reflections.md).
- Continued optimisation work on the DDGI update path as part of each release.
- Composition with the rest of the RT effect suite, so DDGI, RTXDI and RT reflections can all be active.

> Do not install the launcher 4.27 DDGI plugin into a Vite project. The engine already ships DDGI and the
> two conflict. The standalone
> [DDGI 1.1.5 plugin](https://github.com/GapingPixel/UE4-RTXGI-1.1.5-Latest-Official) is only for team
> members working in a stock launcher 4.27 install alongside a Vite project.
>
{style="warning"}

## Setting up

<procedure title="Set up Dynamic DDGI in a level" id="setup-ddgi">
    <step>
        Confirm ray tracing is enabled for the project. DDGI needs DXR support.
    </step>
    <step>
        Enable the plugin path with <code>r.GlobalIllumination.ExperimentalPlugin 1</code>, either from the
        console, from code, or in <code>DefaultEngine.ini</code>.
    </step>
    <step>
        Place a DDGI volume actor so that it encloses the playable space you want lit. Volumes cover
        volume, not surfaces &mdash; think about where the player and dynamic objects can go, not about
        where the walls are.
    </step>
    <step>
        Set probe counts per axis. Start coarse and increase only where you can see the difference; probe
        count drives both memory and per-frame ray cost.
    </step>
    <step>
        Enable <a href="SSGI.md">SSGI</a> with <code>r.SSGI.Enable 1</code> to recover the contact detail
        the probe grid cannot represent.
    </step>
    <step>
        Verify in motion, not in a still frame. DDGI's advantages are temporal &mdash; a screenshot will not
        show you the stability difference against a denoised technique.
    </step>
</procedure>

<img src="ProjectSettingsRHI.png" alt="Project Settings showing Default RHI set to DirectX 12 with DirectX 11 and 12 SM5 checked" border-effect="line"/>

*Step one, and the step people skip. **Project Settings &rarr; Platforms &rarr; Windows &rarr; Targeted RHIs**
must be DirectX 12; ray tracing in 4.27 is DX12-only.*

<img src="DDGIEnablePlugin.png" alt="Plugins dialog with the NVIDIA RTX Global Illumination plugin enabled under Built-In Rendering" border-effect="line"/>

*The GI plugin path under **Built-In &rarr; Rendering**. Vite ships this engine-side, so this is the
in-engine plugin, not the launcher 4.27 plugin &mdash; do not install that one alongside it.*

<img src="DDGIVolumeEditor.png" alt="A DDGI volume actor placed in a level in the editor viewport" border-effect="line"/>

*A DDGI volume in the level. Volumes cover space, not surfaces &mdash; size them around where the camera and
dynamic objects can actually go.*

### Volume settings

<img src="DDGISettingsVolume.png" alt="DDGI volume settings panel" border-effect="line"/>

<img src="DDGISettingsProbes.png" alt="DDGI probe settings panel showing counts per axis and probe spacing controls" border-effect="line"/>

<img src="DDGISettingsLighting.png" alt="DDGI lighting settings panel" border-effect="line"/>

*Volume, probe and lighting settings on the DDGI volume actor. Probe counts per axis are the control that
drives both memory and per-frame ray cost.*

## Tuning

The settings that matter most, roughly in order of impact:

**Probe density.** The dominant cost and quality control. Denser grids resolve smaller lighting features and
cost proportionally more in both rays traced and memory. Interior spaces with lots of small rooms need more
probes than open exteriors.

<img src="DDGIProbeDensity.png" alt="Comparison of probe density showing how lighting detail changes with probe spacing" border-effect="line"/>

*Probe density against resolved lighting detail. Detail finer than the probe spacing does not exist in the
representation at any quality setting &mdash; that is what [SSGI](SSGI.md) is for.*

**Volume placement and count.** Several tightly-fitted volumes usually beat one large loose one. A volume
that spans a whole level at low density wastes probes on solid geometry and starves the spaces that matter.

**Rays per probe.** Trades convergence speed against per-frame cost. Lower counts converge more slowly,
which shows up as lag when lighting changes rapidly &mdash; a door opening onto a bright exterior, for
instance.

**Hysteresis / update rate.** How quickly probes accept new information. Faster response reduces lag but
increases temporal variation.

**Normal and view bias.** The standard controls for trading light leaking against contact shadow darkening.
If you see light bleeding through thin walls, increase bias; if contact areas look detached, reduce it.

## Reference material

NVIDIA's original documentation and talks remain the best deep reference for the technique itself.

- [RTXGI plugin README](https://github.com/GapingPixel/UE5-PhysX-Vite/tree/ue5Vite-release/Engine/Plugins/Runtime/Nvidia/RTXGI)
- [Dynamic Diffuse Global Illumination, GDC presentation](https://developer.download.nvidia.com/video/gputechconf/gtc/2019/presentation/s9900-irradiance-fields-rtx-diffuse-global-illumination-for-local-and-cloud-graphics.pdf)

Ray-Traced Irradiance Fields:

[![Ray-Traced Irradiance Fields](https://img.youtube.com/vi/KufJBCTdn_o/0.jpg)](https://www.youtube.com/watch?v=KufJBCTdn_o)

Sample scene setup, and combined usage with SSGI:

[![DLSS and RTXGI Unreal Engine 4 Plugin: Settings Deep Dive](https://img.youtube.com/vi/ZefvmV1pdP8/0.jpg)](https://www.youtube.com/watch?v=ZefvmV1pdP8&t=1210s)

Plugin settings deep dive:

[![NVIDIA RTXGI Unreal Engine 4 Plugin: Settings Deep Dive](https://img.youtube.com/vi/U57_a3lGKOo/0.jpg)](https://www.youtube.com/watch?v=U57_a3lGKOo)

Full official playlist:

[![Getting Started with NVIDIA RTXGI Unreal Engine 4 Plugin](https://img.youtube.com/vi/bxEVMnyXxqw/0.jpg)](https://www.youtube.com/playlist?list=PL4FII4B-zM0f5h75klcOfiO1v_atlp8Ky)

There is also an internal
[DDGI reference document](https://docs.google.com/document/d/1kdZGRV6bRNjNvec1OzzEJd64NtLBDZ8hzQvFVzB2GfI/edit?tab=t.0)
maintained by the team.

## Sample scenes

- [Tech Demo Project](Tech-Demo-Project.md) &mdash; includes NVIDIA's official DDGI Cornell Box sample and a
  high-end DDGI plus SSGI cave scene.
- [Stylized Raytracing Demo](Stylized-Raytracing-Demo.md) &mdash; the 4K120 stylised target.
- [Abandoned Apartment](Abandone-Apartment.md) and [Attic Scene](Attic-Scene.md) &mdash; NVIDIA's RTGI
  showcase scenes.

## See also

- [Global Illumination](Global-Illumination.md)
- [Static DDGI](DDGI-Static.md)
- [SSGI](SSGI.md)
- [Ray-Traced Reflections](RT-Reflections.md)
- [Performance Targets](Performance-Targets.md)
