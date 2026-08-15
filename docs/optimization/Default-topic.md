# Unreal Engine Vite Manual

<tldr>
<p>
Unreal Engine Vite is a performance-first fork of Unreal Engine 4.27 built from the NvRTX and Plus branches.
It keeps PhysX, adds DDGI, RTXDI, tessellation, SMAA and HBAO+, and targets native-resolution,
high-frame-rate rendering on console-class hardware.
</p>
<p>
New here? Start with <a href="./GettingStarted/Introduction-to-Vite.md">Introduction to Vite</a>, then
<a href="./GettingStarted/Build-From-Source.md">Build the Engine from Source</a>.
</p>
</tldr>

Welcome to the Unreal Engine Vite manual. This site documents the engine fork itself: how to build it,
what it changes relative to stock Unreal Engine 4.27, which rendering and physics systems it adds, and
how to get a project shipping on it.

Vite is aimed at teams in active production. It is not a research branch or a Tech demonstration. Every feature
documented here is expected to survive cooking, packaging and shipping on real hardware, and the
manual is written with that assumption throughout.

<img src="../img/optimization/StylizedRTDemo.png" alt="Stylized scene lit by Dynamic DDGI with the frame counter reading 811 FPS" border-effect="line"/>

*The stylized demo, lit entirely by dynamic ray-traced global illumination: 811 FPS at 1440p native on an
RTX 4080 Super. The same scene on Lumen in Unreal Engine 5.7 measures 324 FPS.*

## What Vite is for

Unreal Engine 5.7 and 5.8 target roughly 60 FPS at dynamic internal resolutions of 720p&ndash;1080p on
PlayStation 5 using Lumen, Nanite, Virtual Shadow Maps, TSR and Chaos. That stack buys its fidelity with
virtualisation and temporal reconstruction, and pays for it in processing, streaming and memory overhead,
in noise, ghosting and blur, and in a renderer whose base cost is high before a project has built anything.

With the Nintendo Switch 2 shipping, handhelds like the Steam Deck established, and hardware costs rising,
that trade increasingly does not match the machines games have to run on. Vite takes the opposite position:
hold high visual fidelity while keeping strict frame-time budgets and high native resolutions.

The concrete claim is a base performance uplift of up to 2.5x real game FPS against UE 5.7's intended
feature set, and it is backed by measurements rather than assertion &mdash; see
[UE4 versus UE5 Cost Analysis](./EngineOverview/UE4-Versus-UE5-Cost-Analysis.md) and
[Projects and Demos](./ProjectsAndDemos.md).

[![Vite ray-traced GI and ray-traced reflections](https://img.youtube.com/vi/2vfG3W-Gy5E/maxresdefault.jpg)](https://youtu.be/2vfG3W-Gy5E)

## Where to start

Pick the entry point that matches what you are doing right now.

| I want to&nbsp;&hellip; | Start here |
|---|---|
| Understand what Vite is and whether it fits my project | [Introduction to Vite](./GettingStarted/Introduction-to-Vite.md) |
| Get the engine running on my machine | [Getting Started](./GettingStarted/Getting-Started.md) |
| Compile the engine from source | [Building from Source](./GettingStarted/Build-From-Source.md) |
| Fix a compile or setup error | [Build Troubleshooting](./GettingStarted/Build-Troubleshooting.md) |
| Move an existing UE5 project onto Vite | [Migrating from Unreal Engine 5](./GettingStarted/Migrating-From-UE5.md) |
| Light a scene without Lumen | [Global Illumination](./Rendering/Global-Illumination.md) |
| Replace Chaos with PhysX in my gameplay code | [PhysX](./Physics/PhysX.md) |
| Find a console variable | [Console Variables](./Reference/Console-Variables.md) |
| Contribute a change to the fork | [Contributing](./Contributing/Contributing.md) |

## The features Vite is built around

<deflist>
<def title="Dynamic DDGI">
Ray-traced irradiance probe volumes. Noise-free by construction, fully dynamic, and roughly twice the frame
rate of hardware Lumen in comparable scenes. See <a href="./Rendering/DDGI-Dynamic.md">Dynamic DDGI</a>.
</def>
<def title="Static DDGI">
The same probe representation with near-instant bake times. Higher bounce fidelity than traditional baked
lighting and better coverage of moving objects. Viable on GPUs with no ray tracing support at all. See
<a href="../Rendering/DDGI-Static.md">Static DDGI</a>.
</def>
<def title="Optimised RT reflections">
Capable of 4K native 60 FPS on a PS5-class GPU. With DDGI active, reflection rays sample probe irradiance
for their secondary bounce rather than returning black. See
<a href="./Rendering/RT-Reflections.md">Ray-Traced Reflections</a>.
</def>
<def title="UE4-era SSGI">
SSGI regressed in quality and performance when UE5 folded it into Lumen, and can no longer run alongside a
separate GI solution. In Vite it composes cleanly with DDGI. See <a href="./Rendering/SSGI.md">SSGI</a>.
</def>
<def title="PhysX 3.4">
Libraries rebuilt for newer Clang versions, Blast support added, and GPU-accelerated particles that run
across vendors. Measurably faster than Chaos on the same workload. See <a href="./Physics/PhysX.md">PhysX</a>.
</def>
<def title="Apex Destruction and Cloth">
Both were deprecated and then removed in UE5 with no migration path for existing assets. In Vite they keep
working. See <a href="./Physics/Destruction-And-Cloth.md">Destruction and Cloth</a>.
</def>
<def title="RTXDI">
A less noisy alternative to MegaLights, in its standalone form rather than the Lumen-integrated version
found in 5.1 and later. See <a href="./Rendering/RTXDI.md">RTXDI</a>.
</def>
<def title="Tessellation">
Distance- and displacement-driven geometric detail: smoother surfaces, better silhouettes and
high-frequency detail at runtime without Nanite's overhead. See <a href="./Rendering/Tessellation.md">Tessellation</a>.
</def>
</deflist>

## Performance targets

Every target below includes ray tracing. Full detail and measurement conditions in
[Performance Targets](./EngineOverview/Performance-Targets.md).

| PS5-class target | What it includes |
|---|---|
| **Stylized, 4K 120 FPS** | RT DDGI, as shown in the Stylized demo. Intended for competitive multiplayer. |
| **Performance, high end, 4K 60 FPS** | DDGI, RT reflections and tessellation, as shown in the Unreal Tournament Vite demo. |
| **Fidelity, high end, 4K 30 FPS** | The same stack scaled for large open worlds with high geometric density. |
| **Fidelity, full RT, 1440p 30 FPS** | Adds RTAO and RT shadows on top of DDGI and RT reflections. |

## Manual sections

<deflist>
<def title="Getting Started">
Installation, toolchain setup, building from source, creating your first project, and migrating an
existing project from Unreal Engine 5. See <a href="./GettingStarted/Getting-Started.md">Getting Started</a>.
</def>
<def title="Engine Overview">
Why the fork is based on NvRTX 4.27, what performance targets it is designed around, and where the
measurable cost differences between UE4 and UE5 come from. See
<a href="./EngineOverview/Engine-Overview.md">Engine Overview</a>.
</def>
<def title="Rendering">
Global illumination, ray tracing, shading models, anti-aliasing, upscaling, tessellation, ambient
occlusion and colour management. The largest section of the manual, and where most of Vite's divergence
from stock 4.27 lives. See <a href="./Rendering/Rendering.md">Rendering</a>.
</def>
<def title="Physics">
PhysX 3.4 as the shipping physics backend: the fast path, fixed timestep, Apex Destruction, Apex Cloth,
Blast, and large-scale instanced rigid bodies. See <a href="./Physics/Physics.md">Physics</a>.
</def>
<def title="Performance and Optimization">
Profiling tools, the engine defaults Vite changes out of the box, shader compilation and PSO behaviour,
scalability, and how to strip the engine down for faster iteration. See
<a href="./Performance/Performance.md">Performance and Optimization</a>.
</def>
<def title="Platforms">
What each target platform supports, which renderer paths are available, and the hardware Vite is tuned
against. See <a href="./Platforms/Platforms.md">Platforms</a>.
</def>
<def title="Plugins">
Plugins bundled with the engine, and the vetting process for proposing new ones. See
<a href="./Plugins/Plugins.md">Plugins</a>.
</def>
<def title="Tools and Automation">
The <code>ViteSetup.bat</code> assistant, installed engine builds, packaging and distribution scripts, and
cache management. See <a href="./Tools/Tools.md">Tools and Automation</a>.
</def>
<def title="Projects and Demos">
Downloadable sample projects and benchmark scenes, with the hardware numbers they were captured on. See
<a href="./ProjectsAndDemos.md">Projects and Demos</a>.
</def>
<def title="Contributing">
Coding guidelines, commit and pull request conventions, and the backporting workflow used to bring UE5
changes into the fork. See <a href="./Contributing/Contributing.md">Contributing</a>.
</def>
<def title="Reference">
Console variable reference, compile-time switches, glossary and FAQ. See
<a href="./Reference/Reference.md">Reference</a>.
</def>
</deflist>

## Community and project links

| Resource | Link |
|---|---|
| Engine repository | [ViteStudio-Tech](https://github.com/ViteStudio-Tech) |
| Community Discord | [discord.gg/n9zQrYFhMb](https://discord.gg/n9zQrYFhMb) |
| Work plan (Trello) | [UE Vite PhysX Studio Fork](https://trello.com/b/JKyBFS5X/ue-vite-physx-vite-studio-fork) |
| Public asset drive | [Google Drive](https://drive.google.com/drive/folders/16FOkb5u6GSqHiWeAm50NaxZ19QFBwZeI?usp=sharing) |
| Support development | [ko-fi.com/vitestudio](https://ko-fi.com/vitestudio) |
| Media updates | [@theredpix](https://x.com/theredpix) |

> Vite is developed in the open by a small team of engine programmers. If you want to contribute,
> read [Contributing](./Contributing/Contributing.md) and request the Forker role on Discord.
>
{style="note"}
