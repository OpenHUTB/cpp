# RTX 直接光照（RTX Direct Illumination, RTXDI）


Ray-traced direct lighting for scenes with very many lights. This is the <b>standalone</b> RTXDI, not the
Lumen-integrated version in UE 5.1+. A less noisy alternative to MegaLights.


<b>Compiled out in a default build.</b> Requires rebuilding with
<code>VITE_RT_PSO_DEBLOAT=0</code> before <code>r.RayTracing.SampledDirectLighting</code> does anything.
</p>
</tldr>

RTX Direct Illumination solves the many-lights problem: shading a pixel correctly when hundreds or thousands
of lights could contribute to it, without evaluating all of them.

## What it does

Conventional direct lighting evaluates every relevant light per pixel, so cost scales with light count. Once
a scene has hundreds of lights, that becomes untenable, which is why engines historically cull aggressively,
limit shadow-casting lights, or bake.

RTXDI uses reservoir-based spatiotemporal importance resampling. Each pixel maintains a small reservoir of
candidate light samples, refined across neighbouring pixels and across frames. The result approximates
sampling all lights while evaluating only a few, and the cost becomes roughly independent of light count.

The practical effect is that light count stops being a budget you manage. You can light a scene with the
number of emitters it physically has rather than the number the renderer can afford.

## The standalone distinction

This matters and is easy to miss.

UE 5.1 and later NvRTX branches integrate RTXDI *into Lumen*. Vite ships the standalone implementation,
which operates on the scene's direct lighting independently of any global illumination solution.

The consequence is composability. RTXDI handles direct lighting; [DDGI](./DDGI-Dynamic.md) handles indirect
bounce; they do not need to know about each other. Enabling both still results in better performance than
standalone hardware Lumen, while producing a less noisy image than MegaLights.

## Enabling

<warning>
RTXDI's shader permutations are compiled out when <code>VITE_RT_PSO_DEBLOAT</code> is <code>1</code>, which
is the default. In such a build <code>ShouldRenderRayTracingSampledLighting()</code> returns
<code>false</code> unconditionally: the console variable will set successfully and nothing will render.
</warning>

<procedure title="Enable RTXDI" id="enable-rtxdi">
    <step>
        Add <code>GlobalDefinitions.Add("VITE_RT_PSO_DEBLOAT=0");</code> to your target file and rebuild
        the engine. See <a href="../Performance/Compile-Time-Switches.md">Compile-Time Switches</a>.
    </step>
    <step>
        Set the console variable:
        <code-block lang="ini">
; Config/DefaultEngine.ini
[/Script/Engine.RendererSettings]
r.RayTracing.SampledDirectLighting=1
        </code-block>
    </step>
    <step>Confirm the target hardware supports DXR.</step>
</procedure>

Because turning the debloat switch off restores the full ray tracing permutation set &mdash; not just
RTXDI's &mdash; weigh the shader compile time and package size cost against what RTXDI actually buys your
scene. The next section is the test.

## When it earns its cost

RTXDI is a specialised tool. It is worth enabling when your scene has enough lights that conventional direct
lighting becomes the bottleneck, and not otherwise.

| Scene | Verdict |
|---|---|
| Neon-lit city street, hundreds of emissive signs and practicals | Strong fit |
| Interior with many small practical lights | Strong fit |
| Destructible or dynamic environments where light count varies unpredictably | Strong fit |
| Outdoor daylight scene with a sun and a few fill lights | Not worth it |
| Stylised scene with a deliberately small light rig | Not worth it |

For a scene with a handful of lights, conventional direct lighting is cheaper and produces an identical
result. RTXDI has a fixed setup cost that only pays off once light count is high.

## Interaction with other systems

**DDGI.** Complementary and designed to run together. RTXDI resolves what direct light reaches a surface;
DDGI resolves what bounced light reaches it.

**RT shadows.** RTXDI performs its own visibility testing as part of sampling, so the relationship with
`r.RayTracing.Shadows` needs measuring in your specific scene rather than assuming. Enabling both is not
automatically the right answer.

**Denoising.** RTXDI output is denoised. It is described as less noisy than MegaLights rather than noise-free
&mdash; unlike [DDGI](./DDGI-Dynamic.md), which is noise-free by construction. Under fast motion in a scene
with many small bright lights, expect to spend some time on denoiser settings.

## See also

- [Ray Tracing](./Ray-Tracing.md)
- [RT Shadows and Ambient Occlusion](./RT-Shadows-And-Ambient-Occlusion.md)
- [Dynamic DDGI](./DDGI-Dynamic.md)
- [Compile-Time Switches](../Performance/Compile-Time-Switches.md)
- [Performance Targets](../EngineOverview/Performance-Targets.md)
