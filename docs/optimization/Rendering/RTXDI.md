# RTXDI


适用于包含大量光源场景的光线追踪直接照明。这是独立的 RTXDI（RTX 直接光照，RTX Direct Illumination），而非 UE 5.1 及更高版本中集成的 Lumen 版本。它是 MegaLights 的一个噪声更低的替代方案。

**默认构建版本中已编译移除。** 需要使用 `VITE_RT_PSO_DEBLOAT=0` 重新构建，`r.RayTracing.SampledDirectLighting` 才能生效。

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

### 启用 RTXDI

1. 将 `GlobalDefinitions.Add("VITE_RT_PSO_DEBLOAT=0");` 添加到目标文件中，然后重新构建引擎。请参阅[编译时开关](../Performance/Compile-Time-Switches.md)。

2. 设置控制台变量：
```json
; Config/DefaultEngine.ini
[/Script/Engine.RendererSettings]
r.RayTracing.SampledDirectLighting=1
```
3. 确认目标硬件支持 DXR。

由于关闭精简开关会恢复完整的光线追踪排列集（而不仅仅是 RTXDI 的排列集），因此请权衡着色器编译时间和软件包大小的成本与 RTXDI 实际为场景带来的性能提升。下一节是测试。


## 当它收回成本时

RTXDI 是一个专用工具。只有当场景中存在足够多的光源，以至于传统的直接光照成为瓶颈时，才值得启用它；否则则不建议启用。

| 场景 | 意见 |
|---|---|
| 霓虹闪烁的城市街道，数百个发光标志和实用灯具 | 非常合适 |
| 室内场景，包含许多小型实用灯具 | 非常合适 |
| 可破坏或动态环境，灯光数量变化无常 | 非常合适 |
| 户外日光场景，包含太阳和少量补光灯 | 不值得 |
| 风格化场景，特意使用小型灯光装置 | 不值得 |

对于只有少量光源的场景，传统的直接照明更经济，且效果相同。RTXDI 的设置成本固定，只有在光源数量较多时才能体现其优势。

## 与其他系统的交互

**DDGI：** 两者互补，旨在协同运行。RTXDI 负责解析到达表面的直接光；DDGI 负责解析到达表面的反射光。

**RT 阴影：** RTXDI 会在采样过程中进行自身的可见性测试，因此其与 `r.RayTracing.Shadows` 的关系需要根据具体场景进行测量，而非想当然。同时启用两者并非总是最佳方案。

**降噪：** RTXDI 的输出经过降噪处理。与 [DDGI](./DDGI-Dynamic.md) 不同，RTXDI 的输出被描述为比 MegaLights 的噪声更小，而非完全无噪。DDGI 本身就是无噪的。在包含许多小型明亮光源的快速运动场景中，您可能需要花一些时间来调整降噪设置。

## 另请参阅

- [光线追踪](./Ray-Tracing.md)
- [RT 阴影和环境光遮蔽](./RT-Shadows-And-Ambient-Occlusion.md)
- [动态 DDGI](./DDGI-Dynamic.md)
- [编译时开关](../Performance/Compile-Time-Switches.md)
- [性能目标](../EngineOverview/Performance-Targets.md)
