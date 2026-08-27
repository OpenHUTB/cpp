# 编译时开关

[Engine/Source/Runtime/Core/Public/Misc/CoreDefines.h](https://github.com/OpenHUTB/engine/blob/hutb/Engine/Source/Runtime/Core/Public/Misc/CoreDefines.h) 中的五个 `VITE_*` 宏控制的功能是在编译时决定的，而不是通过控制台变量。`VITE_RT_PSO_DEBLOAT` **默认值为 1，会完全编译掉一些光线追踪效果**——在确定某个功能坏掉之前，先看看那一节。

有些 Vite 功能不能用控制台变量。要么它们会改变着色器的排列集，而这些是在编译着色器时决定的，要么它们会修改关键的运行时代码路径，在这些路径上运行时判断分支的开销比功能本身节省的还要大。


## 开关

| 宏 | 默认 | 启用时的效果 |
|---|---|---|
| `VITE_RT_PSO_DEBLOAT` | `1` | 确定性的固定步长 PhysX 渲染插值 |
| `VITE_O_SSAO` | `1` | Vite 优化过的 SSAO 内存访问路径 |
| `VITE_PHYSX_FIXED_TIMESTEP` | `0` | 确定性的固定步长PhysX，带渲染插值 |
| `VITE_DLSS_PATCH` | `0` | 在放大时实现输出分辨率的半透明效果和体积雾 |
| `VITE_NVRTX_TRANSLUCENCY_DEPTH` | `0` | 把 NvRTX 分支里的半透明深度纹理分离出来 |

每个都用 `#ifndef` 守护定义，所以可以在不修改头文件的情况下从构建系统中重写它们。


## 重写一个开关

### 更改编译时开关

1. 打开你项目的 `Source/<Project>.Target.cs` 文件，或者如果你要在整个引擎中修改，就打开引擎的 target 文件。

2. 将定义添加到 `GlobalDefinitions` 中：

```C#
public MyGameTarget(TargetInfo Target) : base(Target)
{
    Type = TargetType.Game;
    DefaultBuildSettings = BuildSettingsVersion.V2;
    ExtraModuleNames.Add("MyGame");

GlobalDefinitions.Add("VITE_RT_PSO_DEBLOAT=0");

}
```

3. 重新生成项目文件。

4. 如果更改了 `VITE_RT_PSO_DEBLOAT` 或影响着色器排列的任何开关，则重建引擎，并擦除着色器缓存。请参见[缓存管理](../Tools/Cache-Management.md)。

在目标文件中首选 `GlobalDefinitions`，而不是编辑 [CoreDefines.h](https://github.com/OpenHUTB/engine/blob/hutb/Engine/Source/Runtime/Core/Public/Misc/CoreDefines.h)。编辑标头会更改每个目标的值，并创建与引擎存储库的差异，您将必须执行每次合并。


## VITE_RT_PSO_DEBLOAT

这是让人惊讶的开关，所以细节最丰富。

光线追踪流水线状态对象的代价高昂，容易被忽视。每一个可能使用的光线生成着色器置换都必须编译、打包并绑定到光线追踪流水线中，即使其所属的效果在运行时被控制台变量禁用。将 CVar 设置为 0 并不会阻止其着色器存在。结果是着色器编译时间过长，打包构建体积庞大，PSO 数量高，以及运行时光线追踪流水线创建变慢。

`VITE_RT_PSO_DEBLOAT` 通过从 `ShouldCompilePermutation` 返回 `false`，将置换设置简化为 Vite 实际发布的效果。


### 默认值下会编译掉什么

当 `VITE_RT_PSO_DEBLOAT=1` 时，这些效果**无论控制台变量如何都无法使用**：


| 效果 | 通常由以下控制 |
|---|---|
| 每像素光线追踪全局光照 | `r.RayTracing.GlobalIllumination` |
| RTXDI 采样直接光照 | `r.RayTracing.SampledDirectLighting` |
| 路径追踪 | `r.PathTracing`，路径追踪视图模式 |
| 光线追踪半透明 | `r.RayTracing.Translucency` |
| 网格焦散 | NvRTX 焦散控制台变量（Console Variables, CVars） |
| 水体焦散 | NvRTX 水体焦散 CVars |
| 单层水面光线追踪反射 | `r.Water.SingleLayer.RTR` |
| 光线追踪反射捕捉和反射探针 | `r.RayTracing.Reflections.RayTraceEnvironmentCaptures` |

设置控制台变量看起来会成功，但效果不会生效。

### 仍然有效的功能

| 效果 | 说明 |
|---|---|
| 光线追踪反射 | 被迫使用排序后的延迟算法。旧的非延迟路径已经被编译出去。 |
| 光线追踪阴影 | 不受影响 |
| 光线追踪环境遮蔽 | 不受影响 |
| 光线追踪天空光 | 不受影响 |
| DDGI | 不受影响。DDGI 是一个插件，不会经过这些排列组合。 |

剩下的配置正好是 Vite 推荐的设置：用 [DDGI](../Rendering/DDGI-Dynamic.md) 处理间接光照，通过延迟路径实现 [RT 反射](../Rendering/RT-Reflections.md)，以及 [RT 阴影和环境光遮蔽](../Rendering/RT-Shadows-And-Ambient-Occlusion.md)。查看[全局光照](../Rendering/Global-Illumination.md)了解为什么 DDGI 比每像素光线追踪全局光照更受推荐——精简开关会将这个推荐编码进构建中。


### 关闭它

如果你需要用于[参考图像](../Rendering/Path-Tracing.md)的路径追踪、多光源场景的 RTXDI，或者光线追踪的半透明和焦散效果，请设置 `VITE_RT_PSO_DEBLOAT=0` 并重新构建。


**警告：** 关闭这个选项会显著增加着色器编译时间、打包后的构建大小和光线追踪 PSO 数量。考虑保持一个单独的目标配置——内部参考渲染构建关闭减肥，发布构建开启减肥——而不是全局关闭它。


## VITE_O_SSAO

启用 Vite 为屏幕空间环境光遮蔽通道重写的内存访问模式。视觉效果与原版相同，但成本更低。

在这些开关中比较少见的是，它还有一个运行时 CVar —— 但仅在开发版本中有：

```
r.Vite.SSAO 0   // 库存 UE 的路径
r.Vite.SSAO 1   // 优化路径
```

发布版本的构建锁定在编译时的值；该 CVar 不存在。运行时开关存在是为了在开发过程中可以对两条路径进行 A/B 比较，这也是优化验证的方法。没有理由关闭编译时开关。参见[环境光遮蔽](../Rendering/Ambient-Occlusion.md)。


## VITE_PHYSX_FIXED_TIMESTEP

默认关闭。启用确定性的固定步长 PhysX 模拟，并配合渲染插值，还有 `p.VitePhysXFixedTimestep.*` 控制台变量。

这个编译时开关存在的原因是该功能会给物理场景、子步骤任务和动画物理混合添加分支和双缓冲变换存储。没有确定性需求的项目不需要为此付出代价。完整文档在[固定步长](../Physics/Fixed-Timestep.md)中。


## VITE_DLSS_PATCH

默认关闭。添加了 `TranslucencyAfterDOFUpscaledRT` 和 `TranslucencyAfterDOFModulateUpscaledRT` 网格通道，使选中的半透明物体在放大后以输出分辨率渲染，同时添加了 `r.VolumetricFog.UseUpScaledSizeVolumetricFog`，使雾网格可以从输出分辨率计算。

仅与启用超分辨率的项目相关。请参见[超分辨率和帧生成](../Rendering/Upscalers.md)。


## VITE_NVRTX_TRANSLUCENCY_DEPTH

默认关闭。此功能会分配并写入一个独立的半透明深度纹理，以及一个独立的半透明颜色纹理，继承自 NvRTX 分支。某些需要半透明深度的 NvRTX 特效需要此功能；启用后会占用额外的渲染目标。


## 检查构建版本包含的内容

编译时开关在运行时是不可见的，这使得“这个功能是否编译进去了？”成为调试时一个实际存在的问题。

最可靠的检查方法是设置相关的 CVar 并查看效果是否出现。如果在包含数百个光源的场景中，`r.RayTracing.SampledDirectLighting 1` 没有产生任何变化，则 `VITE_RT_PSO_DEBLOAT` 已启用。

对于带有运行时 CVar 的开关，检查 CVar 是否存在是一种直接的测试方法：在正式发布版本中，由于 `r.Vite.SSAO` 已被编译掉，因此找不到它。


## 另请参阅

- [着色器编译和 PSO](Shader-Compilation-And-PSO.md)
- [光线追踪](../Rendering/Ray-Tracing.md)
- [固定时间步长](../Physics/Fixed-Timestep.md)
- [缓存管理](../Tools/Cache-Management.md)
- [从源代码构建](../GettingStarted/Build-From-Source.md)
