# Attic Scene

<tldr>
<p>
Interior lighting scene with strong directional light through a limited opening. A good case for observing
how global illumination handles high-contrast interiors.
</p>
</tldr>

<img src="AtticScene.png" alt="Attic interior with strong directional sunlight entering through a small window" border-effect="line"/>

*Direct sun and deep shadow in the same frame, with a large volume of dim indirect light between them.*

## Download

[Attic Scene](https://drive.google.com/file/d/12CEdigm95nuu7GhRd_KjYIGeCi9_QvVT/view?usp=sharing)

<img src="AtticRTGIV2.jpg" alt="The attic scene rendered with the enhanced ray-traced GI path, volumetric shafts through the window" border-effect="line"/>

*The same scene under NVIDIA's enhanced RTGI path, which the NvRTX Caustics branch contributes to Vite.*

## What it exercises

An attic is a specific and demanding lighting situation: a strong directional source entering through a
small opening, a large volume of dim indirect light, and a lot of geometric clutter casting into it.

Three things get tested harder here than in an average scene:

- **Dynamic range.** Direct sun and deep shadow in the same frame stresses the
  [tonemapper and exposure setup](Color-Management.md). Get this wrong and the scene either blows out at
  the window or crushes everything else to black.
- **Indirect bounce falloff.** How light attenuates as it bounces further from the opening is what sells
  the depth of the space. This is where probe density and GI method choice show.
- **Contact detail.** Clutter against floors and walls needs
  [ambient occlusion](Ambient-Occlusion.md) and ideally [SSGI](SSGI.md) to read as grounded.

## What to look at

Compare [static and dynamic DDGI](DDGI-Static.md) here. A static bake is entirely adequate for a scene
where nothing moves, and it is the cheaper option; the value of dynamic DDGI only appears once the sun
angle or an occluder changes. Moving the directional light makes the distinction obvious.

Then look at the [anti-aliasing](Anti-Aliasing.md) treatment. Fine clutter geometry against a bright
window is a hard case, and it is where SMAA's behaviour differs most visibly from TAA's.

## See also

- [Global Illumination](Global-Illumination.md)
- [Static DDGI](DDGI-Static.md)
- [Color Management](Color-Management.md)
- [Anti-Aliasing](Anti-Aliasing.md)
- [Abandoned Apartment](Abandone-Apartment.md)
