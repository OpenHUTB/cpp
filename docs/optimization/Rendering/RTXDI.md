# RTXDI


适用于包含大量光源场景的光线追踪直接照明。这是独立的 RTXDI（RTX 直接光照，RTX Direct Illumination），而非 UE 5.1 及更高版本中集成的 Lumen 版本。它是 MegaLights 的一个噪声更低的替代方案。

**默认构建版本中已编译移除。**需要使用 `VITE_RT_PSO_DEBLOAT=0` 重新构建，`r.RayTracing.SampledDirectLighting` 才能生效。

RTX 直接照明解决了多光源问题：即使数百或数千个光源可能共同作用于一个像素，也能正确地对其进行着色，而无需评估所有光源。

## 它的作用

传统的直接光照会评估每个像素的所有相关光源，因此成本会随着光源数量的增加而增加。一旦场景中有数百个光源，这种方法就变得难以承受，这也是为什么引擎历来会采取激进的剔除、限制投射阴影的光源或烘焙等措施的原因。

RTXDI 使用基于储层的时空重要性重采样。每个像素维护一个包含候选光源样本的小型储层，这些样本会根据相邻像素和帧进行优化。最终结果相当于对所有光源进行采样，而实际评估的光源却只有少数几个，因此成本几乎与光源数量无关。

实际效果是，光源数量不再是需要控制的预算。您可以根据场景实际拥有的光源数量来照明，而不是根据渲染器能够处理的数量。


## 独立实现方式的区别

这一点很重要，但很容易被忽略。

UE 5.1 及更高版本的 NvRTX 分支将 RTXDI **集成到了** Lumen 中。Vite 提供的是独立实现，它独立于任何全局光照解决方案，处理场景的直接光照。

其结果是可组合性更强。RTXDI 处理直接光照；[DDGI](./DDGI-Dynamic.md) 处理间接反射；它们彼此无需了解。同时启用两者仍然比独立的硬件 Lumen 性能更好，同时生成的图像噪点比 MegaLights 更少。


## 启用

**警告：** 当 `VITE_RT_PSO_DEBLOAT` 为 `1`（默认值）时，RTXDI 的着色器排列组合会被编译掉。在这种情况下，`ShouldRenderRayTracingSampledLighting()` 会无条件返回 `false`：控制台变量会成功设置，但不会渲染任何内容。

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
