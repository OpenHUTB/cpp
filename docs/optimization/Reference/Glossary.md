# Glossary

<tldr>
<p>
Terminology used throughout this documentation. Vite-specific terms are marked; the rest are standard
Unreal or graphics vocabulary that these docs assume.
</p>
</tldr>

## Vite and fork terminology

<deflist>
<def title="Vite">
This engine: a fork of Unreal Engine 4.27 based on NVIDIA's NvRTX branches, focused on performance and
rendering technology that 4.27 does not ship with upstream. Engine association identifier
<code>UE_ViteFork</code>.
</def>
<def title="NvRTX">
NVIDIA's public Unreal Engine branches carrying their ray tracing technology ahead of upstream. Vite draws
from <b>NvRTX 4.27 Caustics</b> and <b>NvRTX 5.0</b>.
</def>
<def title="VITE_* switch">
A compile-time preprocessor switch controlling whether a feature is built into the engine at all. Defined
in <code>Engine\Source\Runtime\Core\Public\Misc\CoreDefines.h</code>. Changing one requires an engine
rebuild. See <a href="../Performance/Compile-Time-Switches.md">Compile-Time Switches</a>.
</def>
<def title="Compiled out">
A feature whose shader permutations and code paths are excluded from the build by a compile-time switch.
Its console variables still exist and set successfully; nothing renders. The most common cause of
"this setting does nothing" in Vite.
</def>
<def title="Debloat">
The process of removing unused platform binaries, templates, samples and plugins from the engine tree to
reduce disk footprint. See <a href="../Performance/Debloat-Guide.md">Debloat Guide</a>.
</def>
<def title="Installed build">
A precompiled, redistributable engine that works like a launcher install. Cannot compile engine C++. See
<a href="../Tools/Installed-Builds.md">Installed Builds</a>.
</def>
</deflist>

## Rendering

<deflist>
<def title="DDGI">
Dynamic Diffuse Global Illumination. NVIDIA's probe-based GI technique, provided by the RTXGI plugin.
Probes store irradiance and depth; surfaces sample the surrounding probes for indirect light. Vite's
recommended dynamic GI. See <a href="../Rendering/DDGI-Dynamic.md">Dynamic DDGI</a>.
</def>
<def title="SSGI">
Screen Space Global Illumination. Derives indirect light from the rendered frame. Cheap and detailed but
limited to what is on screen. Usually layered under DDGI rather than used alone. See
<a href="../Rendering/SSGI.md">SSGI</a>.
</def>
<def title="RTXDI">
RTX Direct Illumination. Ray-traced direct lighting for scenes with very many lights, using reservoir
resampling. Vite has the standalone version, not the Lumen-integrated one from UE 5.1+. Compiled out by
default. See <a href="../Rendering/RTXDI.md">RTXDI</a>.
</def>
<def title="ReSTIR / reservoir resampling">
The sampling technique behind RTXDI. Each pixel maintains a reservoir of light samples that is refined
spatially and temporally, so a small number of rays approximates a much larger light set.
</def>
<def title="Path tracing">
Full light transport simulation. Reference quality, not a runtime target. Compiled out by default. See
<a href="../Rendering/Path-Tracing.md">Path Tracing</a>.
</def>
<def title="DXR">
DirectX Raytracing. The API behind every ray-traced feature in Vite, and the reason they all require
DirectX 12.
</def>
<def title="PSO">
Pipeline State Object. A compiled bundle of shader and fixed-function state. Ray tracing PSOs are large
and slow to build, which is what <code>VITE_RT_PSO_DEBLOAT</code> exists to reduce. See
<a href="../Performance/Shader-Compilation-And-PSO.md">Shader Compilation and PSO</a>.
</def>
<def title="Shader permutation">
One compiled variant of a shader for a specific combination of features. Permutation count multiplies:
each shading model, each optional feature and each quality level widens the set. The dominant factor in
shader compile time and package size.
</def>
<def title="Caustics">
Light focused by refraction or reflection through transparent or specular surfaces. Vite has both mesh
caustics and water caustics from the NvRTX Caustics branch. Compiled out by default. See
<a href="../Rendering/RT-Translucency-And-Caustics.md">RT Translucency and Caustics</a>.
</def>
<def title="Callisto BRDF">
A Vite-specific shading model for characters and skin, exposing retroreflection, diffuse Fresnel and
terminator smoothing by repurposing standard material pins. See
<a href="../Rendering/Shading-Models.md">Shading Models</a>.
</def>
<def title="Shadow terminator">
The boundary between lit and unlit regions on a curved surface. Its hardness is one of the most visible
tells of real-time rendering; Callisto BRDF exposes direct control over it.
</def>
<def title="SMAA">
Subpixel Morphological Anti-Aliasing. Vite's recommended anti-aliasing method. Spatial rather than
temporal, so it does not introduce the ghosting and smearing TAA does. See
<a href="Anti-Aliasing.md">Anti-Aliasing</a>.
</def>
<def title="TAA">
Temporal Anti-Aliasing. Unreal's default. Accumulates samples across frames; effective but introduces
ghosting, smearing and softness on motion.
</def>
<def title="HBAO+">
NVIDIA's Horizon-Based Ambient Occlusion. Higher quality than standard SSAO, multiplying over the
screen-space AO buffer. Works on DX11 and D3D12 despite help text to the contrary. See
<a href="../Rendering/Ambient-Occlusion.md">Ambient Occlusion</a>.
</def>
<def title="Tessellation">
Hardware subdivision of triangles on the GPU, driving displacement. Removed in UE5 in favour of Nanite;
retained in Vite. See <a href="../Rendering/Tessellation.md">Tessellation</a>.
</def>
<def title="TressFX">
AMD's strand-based hair rendering and simulation system. Version 5.0 is bundled. An alternative to
Unreal's Groom system. See <a href="../Rendering/Hair-Rendering.md">Hair Rendering</a>.
</def>
<def title="NRD / ReLAX">
NVIDIA Real-Time Denoiser. Cleans up the noise inherent in low-sample-count ray tracing. Enabled by
default.
</def>
</deflist>

## Upscaling

<deflist>
<def title="DLSS">
NVIDIA Deep Learning Super Sampling. Renders at lower resolution and reconstructs, using an AI model.
NVIDIA hardware only. Vite bundles 4.5. See <a href="../Rendering/Upscalers.md">Upscalers</a>.
</def>
<def title="DLAA">
Deep Learning Anti-Aliasing. DLSS's reconstruction applied at native resolution &mdash; quality rather than
performance.
</def>
<def title="Ray Reconstruction">
DLSS component that replaces hand-tuned ray tracing denoisers with a learned one.
</def>
<def title="FSR">
AMD FidelityFX Super Resolution. Cross-vendor upscaling. Vite bundles FSR 4.1.1, which is DX12-only.
</def>
<def title="XeSS">
Intel Xe Super Sampling. Cross-vendor, with a faster path on Intel Arc hardware. Vite bundles 3.0.5.
</def>
<def title="NIS">
NVIDIA Image Scaling. Spatial upscaling with no temporal component. The cheapest and lowest-quality
option, but it works everywhere.
</def>
<def title="Streamline">
NVIDIA's plugin layer providing DLSS Frame Generation and Reflex.
</def>
<def title="Frame generation">
Synthesising intermediate frames between rendered ones. Increases displayed frame rate without reducing
input latency, and slightly increases it.
</def>
<def title="Reflex">
NVIDIA's input latency reduction technology, which matters more once frame generation is in play.
</def>
</deflist>

## Physics

<deflist>
<def title="PhysX">
NVIDIA's physics engine, Unreal's default through 4.27. Retained by Vite rather than migrating to Chaos.
See <a href="../Physics/PhysX.md">PhysX</a>.
</def>
<def title="Chaos">
Epic's physics engine, which replaced PhysX in UE5. Vite does not use it.
</def>
<def title="Apex Destruction">
PhysX's destructible mesh system. Removed in UE5; retained in Vite. See
<a href="../Physics/Destruction-And-Cloth.md">Destruction and Cloth</a>.
</def>
<def title="Apex Cloth">
PhysX's cloth simulation. Removed in UE5; retained in Vite.
</def>
<def title="Blast">
NVIDIA's newer destruction library, more flexible than Apex Destruction. Bundled with separate runtime and
authoring plugins.
</def>
<def title="Fixed timestep">
Advancing physics by a constant delta regardless of frame time, giving frame-rate-independent determinism.
Optional in Vite behind <code>VITE_PHYSX_FIXED_TIMESTEP</code>. See
<a href="../Physics/Fixed-Timestep.md">Fixed Timestep</a>.
</def>
<def title="Substepping">
Subdividing a frame's physics update into smaller steps for stability. Distinct from fixed timestep, which
also fixes the step size across frames.
</def>
<def title="CCD">
Continuous Collision Detection. Sweeps a body's motion between frames rather than testing only its
endpoints, preventing fast objects from tunnelling through thin geometry. More expensive than discrete
detection.
</def>
<def title="CMC">
Character Movement Component. Unreal's standard character locomotion component, and one of the most
expensive per-instance costs in a crowd scene. See
<a href="../ProjectsAndDemos/400-Characters-CMC-Bench.md">400 Characters CMC Bench</a>.
</def>
</deflist>

## Engine and build

<deflist>
<def title="DDC">
Derived Data Cache. Where compiled shaders, cooked textures and other derived artefacts are stored. See
<a href="../Tools/Cache-Management.md">Cache Management</a>.
</def>
<def title="UAT / UBT">
Unreal Automation Tool and Unreal Build Tool. UAT drives high-level operations such as installed builds
and packaging; UBT compiles.
</def>
<def title="BuildGraph">
UAT's XML-driven build orchestration system, used to produce installed builds.
</def>
<def title="ISM / HISM">
Instanced Static Mesh and Hierarchical Instanced Static Mesh components. Render many copies of one mesh in
a small number of draw calls. The rendering side of the
<a href="../Physics/Instanced-Physics.md">PhysX Instanced Subsystem</a>.
</def>
<def title="ABI">
Application Binary Interface. In Vite's context, the binary layout of structures shared between CPU and
GPU. Changing one breaks PSO caching, serialization and ray tracing stability, and is grounds for
immediate rejection of a contribution. See <a href="../Contributing/Coding-Guidelines.md">Coding Guidelines</a>.
</def>
<def title="ACL">
Animation Compression Library. Replaces Unreal's built-in animation compression with a substantially
better size-to-quality curve.
</def>
<def title="Motion matching">
Selecting animation frames at runtime by matching current pose and trajectory against a database, rather
than following a state machine. Provided by the Motion Symphony plugin.
</def>
</deflist>

## See also

- [Console Variables](./Console-Variables.md)
- [Compile-Time Switches](../Performance/Compile-Time-Switches.md)
- [FAQ](./FAQ.md)
