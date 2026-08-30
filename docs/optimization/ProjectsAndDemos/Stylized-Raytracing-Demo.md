# 风格化光线追踪演示

应用于非写实艺术风格的光线追踪技术——这一案例反驳了“光线追踪仅服务于照片级写实效果”的既有观念。
![](../../img/optimization/StylizedRTDemo.png)
*风格化的户外场景，采用动态DDGI照明，动态 DDGI，无烘焙光照。在 RTX 4080 Super 显卡上，1440p 原生分辨率下，帧率显示为 811 FPS。*

## 下载

[风格化光线追踪演示](https://drive.google.com/file/d/1M0H60ESNuvUltF9eePO-CHrlUuBzvFHh/view)

## 为何采用风格化光线追踪？

光线追踪通常被视为实现照片级真实感的技术，这导致许多风格化项目将其拒之门外。这其实是个误区。光线追踪的核心优势在于能够实现正确的光线传输，而风格化渲染同样能从中获益——甚至可以说获益更多，因为风格化美术往往采用强烈且刻意设计的布光，在这种情况下，屏幕空间技术产生的伪影会显得格外刺眼。

具体而言：

| 光线追踪特性 | 为风格化作品带来的效果 |
|---|---|
| [反射](../Rendering/RT-Reflections.md) | 呈现屏幕外几何体的反射（这是屏幕空间反射无法实现的） |
| [阴影](../Rendering/RT-Shadows-And-Ambient-Occlusion.md) | 接触精准的阴影，无需调整阴影贴图分辨率或偏差值 |
| [DDGI](../Rendering/DDGI-Dynamic.md) | 色彩溢出（Color Bleed）效果，能强化而非破坏有限的调色板风格 |
| [环境光遮蔽](../Rendering/Ambient-Occlusion.md) | 扎实的接触感，且不会产生屏幕空间环境光遮蔽（SSAO）常见的晕轮伪影 |

Vite 很好地支持将光线追踪光照与[卡通（Toon）着色模型](../Rendering/Shading-Models.md)相结合——“Toon”正是该分支版本（fork）新增的自定义着色模型之一。


## 观察重点

在演示中切换光线追踪反射的开启与关闭状态。差异最显著之处恰好是屏幕空间反射（screen-space reflections, SSR）失效的场景：例如画面之外的物体、处于掠射角（grazing angles）的表面，以及位于摄像机后方的物体所产生的反射。

接着观察风格化材质如何响应光线追踪光照，而非预烘焙的近似光照。这种光照效果表现稳定，美术设计在摄像机移动时可完全信赖其一致性。


## 可用性说明

**警告：** 默认的 Vite 构建版本支持反射、阴影、环境光遮蔽（AO）和天光（sky light）。而半透明效果、焦散（caustics）、RTXDI、路径追踪以及逐像素光线追踪全局光照（GI）则通过 `VITE_RT_PSO_DEBLOAT` 宏在**编译时被剔除**（该宏默认值为 `1`）。

如果你发现属于后一组功能的控制台变量似乎不起作用，原因就在于此。请参阅[光线追踪](../Rendering/Ray-Tracing.md)与[编译时开关](../Performance/Compile-Time-Switches.md)相关章节。


## 另请参阅

- [光线追踪](../Rendering/Ray-Tracing.md)
- [着色模型](../Rendering/Shading-Models.md)
- [动态 DDGI](../Rendering/DDGI-Dynamic.md)
- [编译时开关](../Performance/Compile-Time-Switches.md)
