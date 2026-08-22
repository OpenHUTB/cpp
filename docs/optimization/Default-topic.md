# 引擎优化入门

<tldr>
<p>
Unreal Engine Vite 是 Unreal Engine 4.27 的性能优先分支，基于 NvRTX 和 Plus 分支构建。
它保留了 PhysX，添加了 DDGI、RTXDI、曲面细分、SMAA 和 HBAO+，并以原始分辨率为目标，
在控制台级硬件上进行高帧速率渲染。
</p>

新手从[Vite简介](./GettingStarted/Introduction-to-Vite.md)开始，然后
[从源代码构建引擎](./GettingStarted/Build-From-Source.md)。

</tldr>

---

欢迎使用虚幻引擎 Vite 手册。该网站记录了引擎分支本身：如何构建它，相对于原版虚幻引擎 4.27 有何变化、添加了哪些渲染和物理系统，以及如何让项目在其上交付。

Vite针对的是活跃生产中的团队。它不是一个研究分支或技术演示。每个功能
此处记录的内容预计可以在真实硬件上的烘焙、打包和交付中保存下来，并且
手册自始至终都是基于这一假设编写的。

![](../img/optimization/StylizedRTDemo.png)

<div class="div" style="text-align: center">
<i>由 Dynamic DDGI 照亮的风格化场景，帧计数器读数为 811 FPS</i></div>
<style>
.div {
    height: 20px;
    line-height: 1px;
}
</style>


*完全由动态光线追踪全局光照的风格化演示：在 RTX 4080 Super 上以 1440p 原生测得 811 FPS。虚幻引擎 5.7 中 Lumen 上的同一场景测得 324 FPS。*

## Vite 的目的是什么

虚幻引擎 5.7 和 5.8 的目标是在 PlayStation 5 上使用 Lumen、Nanite、虚拟阴影贴图、TSR 和 Chaos 技术，以 720p 到 1080p 的动态内部分辨率实现大约 60 FPS 的帧率。这套技术栈通过虚拟化和时间重建来提升画面保真度，但代价是处理、流式传输和内存开销，以及噪点、重影和模糊等问题，此外，渲染器本身的基础成本也很高，即使项目尚未构建任何内容，成本依然很高。

随着任天堂 Switch 2 的上市、Steam Deck 等掌机的普及以及硬件成本的上涨，这种权衡越来越难以满足游戏运行所需的硬件需求。Vite则采取了截然相反的策略：在保持高视觉保真度的同时，严格控制帧时间并实现高原生分辨率。


具体来说，UE4 的实际游戏帧率比 UE5.7 的预期功能集提升了 2.5 倍，并且有测量数据支持，而不是断言——请参阅 [UE4 与 UE5 的成本分析](./EngineOverview/UE4-Versus-UE5-Cost-Analysis.md) 以及[项目和演示](./ProjectsAndDemos.md)。

[![Vite ray-traced GI and ray-traced reflections](https://img.youtube.com/vi/2vfG3W-Gy5E/maxresdefault.jpg)](https://youtu.be/2vfG3W-Gy5E)

## 从哪里开始

选择与您现在正在做的事情相匹配的切入点。

| 我想要&nbsp;&hellip; | 从这里开始 |
|---|---|
| 了解 Vite 是什么以及它是否适合我的项目 | [Vite 简介](./GettingStarted/Introduction-to-Vite.md) |
| 让引擎在我的机器上运行 | [入门](./GettingStarted/Getting-Started.md) |
| 从源代码编译引擎 | [从源代码构建](./GettingStarted/Build-From-Source.md) |
| 修复编译或设置错误 | [构建故障排除](./GettingStarted/Build-Troubleshooting.md) |
| 将现有的 UE5 项目移至 Vite 上 | [从虚幻引擎 5 迁移](./GettingStarted/Migrating-From-UE5.md) |
| 在没有 Lumen 的情况下照亮场景 | [全局光照](./Rendering/Global-Illumination.md) |
| 在我的游戏代码中用 PhysX 替换 Chaos | [PhysX](./Physics/PhysX.md) |
| 查找控制台变量 | [控制台变量](./Reference/Console-Variables.md) |
| 为 fork 做出贡献 | [贡献](./Contributing/Contributing.md) |

## Vite 所围绕的功能

* 动态 DDGI

    光线追踪辐照度探测体积。结构上无噪声，完全动态，在类似场景下帧速率约为硬件流明的两倍。另请参阅[动态 DDGI](./Rendering/DDGI-Dynamic.md)。

* 静态 DDGI

    采用相同的探针表示方法，烘焙时间近乎瞬时。相比传统烘焙光照，反射保真度更高，对运动物体的覆盖范围更广。即使在完全不支持光线追踪的 GPU 上也能正常运行。另请参阅[静态 DDGI](./Rendering/DDGI-Static.md)。

* 优化 RT 反射

    在 PS5 级别的 GPU 上，可实现 4K 原生 60 帧。启用 DDGI 后，反射光线会采样探测器辐照度进行二次反射，而不是返回黑色。另请参阅[光线追踪反射](./Rendering/RT-Reflections.md)。


* UE4 时代的 SSGI

    当 UE5 将 SSGI 集成到 Lumen 中时，其质量和性能有所下降，并且无法再与独立的全局光照解决方案并行运行。在 Vite 中，它可以与 DDGI 完美兼容。另请参阅[SSGI](./Rendering/SSGI.md)。

* PhysX 3.4

    针对新版 Clang 重建了库，新增了 Blast 支持，并支持跨平台 GPU 加速粒子效果。在相同工作负载下，速度明显优于 Chaos。另请参阅[PhysX](./Physics/PhysX.md)。


* Apex 破损和布料

    这两个功能在 UE5 中均已被弃用并移除，现有资源没有迁移方案。但在 Vite 中它们仍然可以正常工作。请参阅[破损和布料](./Physics/Destruction-And-Cloth.md)部分。


* RTXDI

    与 5.1 及更高版本中集成在 Lumen 中的 MegaLights 相比，[RTXDI](./Rendering/RTXDI.md) 是一种噪音更低的替代方案。

* 细分曲面（Tessellation）

    基于距离和位移的几何细节：更平滑的表面、更清晰的轮廓和运行时高频细节，且无需 Nanite 的额外开销。另请参阅[细分曲面](./Rendering/Tessellation.md)。


## 性能目标

以下所有目标均包含光线追踪。完整详情和测量条件另请参阅[性能目标](./EngineOverview/Performance-Targets.md)。

| PS5 级目标 | 它包括什么 |
|---|---|
| **风格化的 4K 120 FPS** | RT DDGI，如风格化演示中所示。专为竞技性多人游戏而设计。 |
| **性能、高端、4K 60 FPS** | DDGI、RT 反射和曲面细分，如 Unreal Tournament Vite 演示中所示。 |
| **保真、高端、4K 30 FPS** | 相同的堆栈适用于具有高几何密度的大型开放世界。 |
| **保真、全 RT、 1440p 30 FPS** | 在 DDGI 和 RT 反射之上添加 RTAO 和 RT 阴影。 |

## 手册部分

* 入门

    安装、工具链设置、从源代码构建、创建您的第一个项目以及从虚幻引擎 5 迁移现有项目。请参阅[入门指南](./GettingStarted/Getting-Started.md)。


* 引擎概述

    该分支版本为何基于 NvRTX 4.27，其设计目标是什么，以及 UE4 和 UE5 之间可衡量的成本差异源自何处。请参阅[引擎概述](./EngineOverview/Engine-Overview.md)。

* 渲染

    全局光照、光线追踪、着色模型、抗锯齿、放大、曲面细分、环境光遮蔽和色彩管理。这是手册中篇幅最长的部分，也是 Vite 与原版 4.27 版本差异最大的地方。另请参阅[渲染](./Rendering/Rendering.md)部分。


* 物理

    PhysX 3.4 作为发布版本的物理引擎后端：支持快速路径、固定时间步长、Apex 破损、Apex 布料、爆炸以及大规模实例化刚体。另请参阅[物理](./Physics/Physics.md)部分。

* 性能与优化

    分析工具、引擎默认设置、Vite 的开箱即用更改、着色器编译和 PSO 行为、可扩展性，以及如何精简引擎以加快迭代速度。请参阅[性能与优化](./Performance/Performance.md)部分。

* 平台

    每个目标平台支持哪些功能、有哪些渲染器路径可用，以及 Vite 针对哪些硬件进行了优化。请参阅[平台](./Platforms/Platforms.md)部分。

* 插件

    引擎自带的插件，以及新插件的审核流程。请参阅[插件](./Plugins/Plugins.md)部分。


* 工具和自动化

    ViteSetup.bat 助手负责安装引擎构建、打包和分发脚本以及缓存管理。请参阅[工具和自动化](./Tools/Tools.md)部分。


* 项目和演示

    提供可下载的示例项目和基准测试场景，以及它们所用的硬件参数。请参阅[项目和演示](./ProjectsAndDemos.md)。

* 贡献

    编码规范、提交和拉取请求约定，以及将 UE5 更改引入分支的向后移植工作流程。请参阅[贡献](./Contributing/Contributing.md)部分。

* 参考

    控制台变量参考、编译时开关、术语表和常见问题解答。请参阅[参考文档](./Reference/Reference.md)。



## 社区和项目链接

| 资源 | 链接 |
|---|---|
| 引擎仓库 | [ViteStudio-Tech](https://github.com/ViteStudio-Tech) |
| 社区 Discord | [discord.gg/n9zQrYFhMb](https://discord.gg/n9zQrYFhMb) |
| 工作计划（Trello） | [UE Vite PhysX Studio 分支](https://trello.com/b/JKyBFS5X/ue-vite-physx-vite-studio-fork) |
| 公共资产 | [谷歌云盘](https://drive.google.com/drive/folders/16FOkb5u6GSqHiWeAm50NaxZ19QFBwZeI?usp=sharing) |
| 支持发展 | [ko-fi.com/vitestudio](https://ko-fi.com/vitestudio) |
| 媒体更新 | [@theredpix](https://x.com/theredpix) |

> Vite 由一个小型引擎程序员团队开源开发。如果您想贡献代码，请阅读[贡献指南](./Contributing/Contributing.md)并在 Discord 上申请 Forker 角色。
