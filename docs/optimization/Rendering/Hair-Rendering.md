# 毛发渲染

!!! note "概要"
    Vite 内置两套基于发丝的毛发系统：AMD **TressFX 5.0** 与 Epic 的 **Groom**（HairStrands）。TressFX 是 Vite 新增的，Groom 是 4.27 自带的。两者默认都关闭。

Vite 内置了两款基于毛发的渲染系统：AMD **TressFX 5.0** 和 Epic 的 Groom（HairStrands）。TressFX 是 Vite 添加的；Groom 是 4.27 版本的自带系统。两者默认均处于禁用状态。


毛发是最难渲染出逼真效果的表面之一。基于卡片的毛发渲染成本低廉且效果可预测，但在运动或掠射角度下始终无法准确呈现。基于毛发的渲染系统能够模拟并渲染每一根毛发，效果显著提升，但成本也更高。


## 可用系统

| 系统 | 插件 | 来源 | 备注 |
|---|---|---|---|
| TressFX 5.0 | `Engine/Plugins/Runtime/TressFX` | AMD | 由 Vite 添加。包含模拟、渲染和编辑器工具。 |
| Groom (HairStrands) | `Engine/Plugins/Runtime/HairStrands` | Epic | 原生版本 4.27。基于 Alembic 引擎，兼容 `Hair` 着色模型。 |
| Alembic Hair Importer | `Engine/Plugins/Importers/AlembicHairImporter` | Epic | 毛发资源导入路径 |

默认情况下，两种发型系统均未启用。请从**编辑(Edit) > 插件(Plugins)** 中启用其中一个；在同一个项目中启用两个发型系统意义不大。

!!! note
    自 7 月主要版本起，TressFX 也是一个编译时特性：要先用 `VITE_TRESSFX=1` 构建引擎，再启用插件。TressFX 的着色器不会包含在移动平台与 ES 3.1 平台上。详见[编译时开关](../Performance/Compile-Time-Switches.md)。

## TressFX 5.0

TressFX 是 AMD 的开源毛发系统。Vite 集成包含三个模块：TressFXCore 用于运行时模拟和渲染，`TressFXImportTranslator` 用于资源导入，`TressFXEditor` 用于创作工具。

它将模拟（毛发响应头部运动、风力和碰撞）和渲染（与顺序无关的透明度、自阴影以及毛发所需的各向异性镜面反射）作为一个系统来处理。


### 控制台变量

| CVar | 默认 | 用途 |
|---|---|---|
| `r.TressFX.StrandsMode` | `0` | 调试可视化。`0` 关闭，`1` 模拟发束，`2` 渲染发束颜色受模拟影响，`3` 毛发 UV，`4` 毛发根部 UV，`5` 毛发种子，`6` 维度  |
| `r.TressFX.Interoplation.FrustumCulling` | `1` | 发束插值期间的视锥体剔除。实验性功能。 |
| `r.TressFX.MorphTargetMeshVisualization` | `0` | 精确的透射率通道，改善小尺寸 TressFX 的渲染效果 |

!!! note
    `r.TressFX.Interoplation.FrustumCulling` 的拼写与源码里一致，`Interpolation` 里的字母也照原样颠倒。请原样使用这个字符串。

    引擎里这两个变量的帮助文本与实际作用对不上（写的是缓冲区交换和一个透射通道）。上面的说明以 `TressFXManager.cpp` 的代码为准。

调试模式是诊断头发问题的最快方法。特别是模式 `2`——渲染发丝时会根据模拟效果着色——可以立即显示引导发丝分布是否合理，或者是否存在大片渲染发丝区域由过少的引导发丝驱动的情况。


### 成本

发丝渲染的成本很容易被低估，因为成本分散在模拟（计算）、渲染（大量透明叠加绘制）和阴影三个方面。一个拥有完整 TressFX 头发的角色可能会消耗 [4K60 帧预算](../EngineOverview/Performance-Targets.md)中相当一部分的资源。


实用指南：

- 仅对主角使用发丝渲染。群众角色和背景角色应使用卡式渲染。
- 积极使用 LOD（细节层次）。发丝数量应随距离大幅减少，在中距离切换到卡式渲染是正常的。
- 使用实际的[抗锯齿](./Anti-Aliasing.md)配置进行测试。对于细小的几何体，抗锯齿方法的影响尤为显著，使用 TAA 抗锯齿效果尚可的头发，在 SMAA 抗锯齿下可能会出现闪烁。

## Groom

Groom 是 Epic 的毛发系统，在 4.27 版本中已内置。它通过 Alembic Hair Importer 从 Maya、Houdini 或 Blender 导入 Alembic 毛发缓存，并使用毛发着色模型及其 **Scatter**（金属引脚）和 **Backlit**（自定义数据 0）参数进行渲染，同时法线引脚上还需启用 **Tangent** 属性。


Groom 的文档更完善，并且拥有更多第三方工具支持，因为它基于 Unreal 引擎。如果您之前使用过面向 AMD 的渲染管线，或者需要 TressFX 的特定模拟行为，那么 TressFX 是更好的选择。


## 与光线追踪的交互

默认情况下，毛发和光线追踪的交互效果不佳。毛发几何体会在加速结构中生成大量微小图元，而光线追踪的[阴影](./RT-Shadows-And-Ambient-Occlusion.md)和[反射](./RT-Reflections.md)在毛发上既耗费资源又会产生噪声。

建议将毛发从光线追踪中排除（通过图元的**在光线追踪中可见（Visible in Ray Tracing）** 标志），并允许其接收光栅阴影。视觉效果上的差异通常很小，但性能差异却不容忽视。


## 另请参阅

- [着色模型](./Shading-Models.md)
- [抗锯齿](./Anti-Aliasing.md)
- [已捆绑的插件](../Plugins/Bundled-Plugins.md)
- [性能目标](../EngineOverview/Performance-Targets.md)
