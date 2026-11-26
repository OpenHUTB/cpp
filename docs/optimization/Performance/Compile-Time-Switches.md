# 编译时开关

!!! note "概要"

    `Engine/Source/Runtime/Core/Public/Misc/CoreDefines.h` 中有十四个 `VITE_*` 宏，它们控制的功能在编译期决定，而不是通过控制台变量。**`VITE_RT_PSO_DEBLOAT` 默认值为 `1`，会整体编译掉若干光线追踪效果**——在判定某个功能坏掉之前，先读那一节。

有些 Vite 功能没法做成控制台变量：要么它们会改变着色器排列集（这在编译着色器时就已确定），要么它们位于热路径上，运行期加一个分支的开销比功能本身省下来的还多。

## 开关

下表中的默认值取自 ueVite26 September Major 版本。"起于"列表示 `CoreDefines.h` 中首次包含该开关的主要版本。

### 渲染与光线追踪

| 宏 | 默认 | 启用时的效果 | 起于 |
|---|---|---|---|
| `VITE_RT_PSO_DEBLOAT` | `1` | 编译掉 Vite 主路径之外的路径追踪与较重的光线追踪排列 | 6 月 |
| `VITE_O_RT_REFLECTIONS` | `1` | 延迟 RT 反射优化；负载仍为 64 字节 | 8 月 |
| `VITE_O_SSAO` | `1` | Vite 优化的 SSAO 内存访问路径，不依赖纹理采样器 | 6 月 |
| `VITE_O_SSD_SAFE_SAMPLE_TRACK` | `1` | 屏幕空间降噪器把 `SampleTrackId` 保留在 VGPR 中（在 AMD 上安全）；置 `0` 恢复 UE 5.0/5.1 的 NVIDIA 占用率路径 | 8 月 |
| `VITE_SPECULAR_OCCLUSION` | `0` | 用 Jimenez 2016 的弯曲法线高光遮蔽替代 UE4 的 Karis 项 | 8 月 |
| `VITE_UE5_POSTPROCESSING` | `1` | UE 5.0 的局部曝光与后处理直方图路径 | 8 月 |
| `VITE_NVRTX_TRANSLUCENCY_DEPTH` | `0` | 来自 NvRTX 分支的独立半透明深度纹理 | 7 月 |
| `VITE_DLSS_PATCH` | `0` | 超分时让半透明与体积雾按输出分辨率渲染 | 7 月 |
| `VITE_FSR2_REACTIVE_MASK` | `0` | 引擎侧的 FSR 2 反应遮罩支持 | 7 月 |

### 着色模型与发丝

| 宏 | 默认 | 启用时的效果 | 起于 |
|---|---|---|---|
| `VITE_SHADINGMODEL_CALLISTO` | `0` | 编译进 Callisto BRDF 着色模型 | 7 月 |
| `VITE_SHADINGMODEL_TOON` | `0` | 编译进卡通（Toon）着色模型 | 7 月 |
| `VITE_TRESSFX` | `0` | 编译进 TressFX 发丝 | 7 月 |

着色模型开关是在 7 月主要版本中加入的：默认关闭这些额外模型后，参考场景从 235.56 FPS 提升到 264.44 FPS（+12.3%）。只开启项目实际用到的模型，参见[着色模型](../Rendering/Shading-Models.md)与[发丝渲染](../Rendering/Hair-Rendering.md)。

### 物理与编辑器

| 宏 | 默认 | 启用时的效果 | 起于 |
|---|---|---|---|
| `VITE_PHYSX_FIXED_TIMESTEP` | `0` | 带渲染插值的确定性固定步长 PhysX | 6 月 |
| `VITE_UE4_ICON_BADGES` | `1` | 在大纲图标上恢复 UE4 风格的 Mobility 与可生成标记；置 `0` 使用 UE5 的标签样式 | 8 月 |

每个宏都用 `#ifndef` 保护，因此可以从构建系统覆盖，不必修改头文件。

## 覆盖某个开关

1. 打开项目的 `Source/<Project>.Target.cs`；若要全引擎生效，则打开引擎的 target 文件。

2. 把定义加入 `GlobalDefinitions`：

```C#
public MyGameTarget(TargetInfo Target) : base(Target)
{
    Type = TargetType.Game;
    DefaultBuildSettings = BuildSettingsVersion.V2;
    ExtraModuleNames.Add("MyGame");

    GlobalDefinitions.Add("VITE_RT_PSO_DEBLOAT=0");
}
```

3. 重新生成工程文件。

4. 重新构建引擎；如果改动的是 `VITE_RT_PSO_DEBLOAT` 或任何影响着色器排列的开关，还要清空着色器缓存。参见[缓存管理](../Tools/Cache-Management.md)。

优先在 target 文件里使用 `GlobalDefinitions`，而不是直接编辑 `CoreDefines.h`。改头文件会对所有 target 生效，并且每次合并都要带着这份与引擎仓库的差异。

## VITE_RT_PSO_DEBLOAT

这是最容易让人意外的开关，所以展开得最细。

光线追踪管线状态对象（PSO）的代价很容易被低估。每一个**可能**被用到的光线生成着色器排列都必须编译、打包并绑定进光线追踪管线——即使它所属的效果在运行时已被控制台变量关闭。把 CVar 设为 `0` 并不会让着色器不存在。结果是：着色器编译时间长、打包体积大、PSO 数量高，运行时创建光线追踪管线也慢。

`VITE_RT_PSO_DEBLOAT` 对除此之外的所有效果在 `ShouldCompilePermutation` 中返回 `false`，把排列集压缩到 Vite 实际交付的那部分。

### 默认值下被编译掉的效果

当 `VITE_RT_PSO_DEBLOAT=1` 时，下列效果**无论控制台变量怎么设都不可用**：

| 效果 | 原本由谁控制 |
|---|---|
| 逐像素光线追踪全局光照 | `r.RayTracing.GlobalIllumination` |
| RTXDI 采样直接光照 | `r.RayTracing.SampledDirectLighting` |
| 路径追踪 | 路径追踪视图模式（`ShowFlag.PathTracing`） |
| 光线追踪半透明 | `r.RayTracing.Translucency` |
| 网格焦散 | NvRTX 焦散相关 CVar |
| 水体焦散 | NvRTX 水体焦散相关 CVar |
| 单层水面光线追踪反射 | `r.Water.SingleLayer.RTR` |
| 光线追踪反射捕捉与反射探针 | `r.RayTracing.Reflections.RayTraceEnvironmentCaptures` |

设置控制台变量看起来会成功，但效果不会渲染出来。

### 仍然可用的效果

| 效果 | 说明 |
|---|---|
| 光线追踪反射 | 强制走排序后的延迟算法；较旧的非延迟路径被编译掉。 |
| 光线追踪阴影 | 不受影响 |
| 光线追踪环境光遮蔽 | 不受影响 |
| 光线追踪天光 | 不受影响 |
| DDGI | 不受影响。DDGI 是插件，不经过这些排列。 |

剩下的这组效果，正好就是 Vite 推荐的配置：用 [DDGI](../Rendering/DDGI-Dynamic.md) 做间接光照，[RT 反射](../Rendering/RT-Reflections.md) 走延迟路径，再加 [RT 阴影与环境光遮蔽](../Rendering/RT-Shadows-And-Ambient-Occlusion.md)。为什么 DDGI 优于逐像素光线追踪全局光照，见[全局光照](../Rendering/Global-Illumination.md)——精简裁减开关把这个推荐直接编码进了构建。

### 关掉它

如果需要[参考图像](../Rendering/Path-Tracing.md)用的路径追踪、多光源场景的 RTXDI，或者光线追踪半透明与焦散，就把 `VITE_RT_PSO_DEBLOAT=0` 并重新构建。

!!! warning

    关闭该开关会显著增加着色器编译时间、打包体积与光线追踪 PSO 数量。建议维护两套 target 配置——内部参考渲染构建关闭精简裁减，交付构建保持开启——而不是全局关闭。

## VITE_O_SSAO

启用 Vite 为屏幕空间环境光遮蔽通道重写的内存访问模式：视觉效果与原版一致，成本更低。

在这些开关里比较特殊的是，它还有一个运行时 CVar——但只在开发版构建中存在：

```
r.Vite.SSAO 0   // 原版 UE 路径
r.Vite.SSAO 1   // 优化路径
```

Shipping 与 Test 构建锁定为编译期的值，该 CVar 根本不存在。保留运行时开关，是为了在开发期对两条路径做 A/B 对比——这次优化正是这样验证出来的。没有理由关掉这个编译期开关。参见[环境光遮蔽](../Rendering/Ambient-Occlusion.md)。

## VITE_PHYSX_FIXED_TIMESTEP

默认关闭。启用确定性固定步长 PhysX 模拟与渲染插值，并附带 `p.VitePhysXFixedTimestep.*` 控制台变量。

之所以设成编译期开关，是因为该功能会给物理场景、子步任务与动画物理混合增加分支和双缓冲的变换存储。没有确定性需求的项目不该为此买单。完整文档见[固定步长](../Physics/Fixed-Timestep.md)。

## VITE_DLSS_PATCH

默认关闭。增加 `TranslucencyAfterDOFUpscaledRT` 与 `TranslucencyAfterDOFModulateUpscaledRT` 两个网格 pass，使选定的半透明图元在超分之后按输出分辨率渲染；同时增加 `r.VolumetricFog.UseUpScaledSizeVolumetricFog`，让雾网格可以按输出分辨率计算。

只对启用了超分器的项目有意义。参见[超分辨率与帧生成](../Rendering/Upscalers.md)。

## VITE_NVRTX_TRANSLUCENCY_DEPTH

默认关闭。在独立半透明颜色之外，再分配并写入一张独立的半透明深度纹理，继承自 NvRTX 分支。某些需要半透明深度的 NvRTX 效果依赖它；开启后会多占用一张渲染目标。

## VITE_SPECULAR_OCCLUSION

默认关闭。把 UE4 的高光遮蔽项——Karis 的 `GetSpecularOcclusion(NoV, RoughnessSq, AO)`——替换为 Jimenez 等人 2016 年的锥-锥高光遮蔽，由 DFAO 或 SSAO 的弯曲法线驱动。它可以减少折痕内侧与物体下方反射中的漏光。

该宏只是基于图像的照明与光线追踪反射着色器上的编译期常量，既不是着色器排列，也不是全局着色器定义，所以不会增加 PSO；改动它只会重新编译那几个着色器。

## VITE_UE5_POSTPROCESSING

默认开启。编译进 UE 5.0 的局部曝光 pass，以及配套的后处理直方图与色调映射改动，让项目不必迁到 UE5 也能获得接近 UE5 的局部曝光控制。

## 着色模型与 TressFX 开关

`VITE_SHADINGMODEL_CALLISTO`、`VITE_SHADINGMODEL_TOON`、`VITE_FSR2_REACTIVE_MASK` 与 `VITE_TRESSFX` 默认关闭。每多一个着色模型，共享光照着色器就多一组分支，所有可能用到它的材质也多一批排列——无论项目是否真的渲染了这类材质，都要付这份成本。只开启美术方向需要的那些。TressFX 无论开关如何，都还会被排除在移动端与 ES 3.1 着色器平台之外。

## 检查构建里到底有什么

编译期开关在运行时不可见，所以调试时"这个功能到底编进去了没"是个真问题。

最可靠的检查方式是设置相关 CVar，看效果是否出现。如果在有数百个光源的场景里，`r.RayTracing.SampledDirectLighting 1` 毫无变化，那就是 `VITE_RT_PSO_DEBLOAT` 开着。

对于带运行时 CVar 的开关，直接检查 CVar 是否存在就是最直接的测试：在 Shipping 或 Test 构建里，`r.Vite.SSAO` 会因为被编译掉而根本找不到。

## 另请参阅

- [着色器编译与 PSO](Shader-Compilation-And-PSO.md)
- [光线追踪](../Rendering/Ray-Tracing.md)
- [固定步长](../Physics/Fixed-Timestep.md)
- [缓存管理](../Tools/Cache-Management.md)
- [从源码构建](../GettingStarted/Build-From-Source.md)
