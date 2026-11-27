# Vite 简介

!!! note "概要"
    Vite 是 Unreal Engine 4.27 面向生产的分支，基于 NVIDIA 的 NvRTX Caustics 分支。它保留 PhysX 3.4 和与引擎无关的光线追踪管线，用更轻的方案取代 Epic 的 UE5 功能堆栈（Lumen、Nanite、VSM、TSR、Chaos），目标是在高帧率下达到原生分辨率。

Unreal Engine Vite 面向专业游戏开发，服务正在制作中的游戏。它的长期目标是成为持续演进的现代引擎：在 CPU 与渲染吞吐上能同专有内部引擎竞争，并针对当代主机硬件持续投入性能、稳定性与图形管线。


目标足够具体，可以证伪：**在每毫秒保真度与模拟规模上击败 Epic 的 UE5**，并在这两项上与专有 AAA 引擎竞争。每毫秒保真度由[性能目标](../EngineOverview/Performance-Targets.md)和[UE4 与 UE5 成本分析](../EngineOverview/UE4-Versus-UE5-Cost-Analysis.md)衡量，模拟规模由[物理](../ProjectsAndDemos/Physics-Cube-Bench.md)和[角色](../ProjectsAndDemos/400-Characters-CMC-Bench.md)基准衡量。Vite 赢不了的地方，本手册也会直说。

## 设计论证

Epic 的虚幻引擎 5.7 与 5.8 使用 Lumen、Nanite、虚拟阴影贴图（VSM）、时间超分辨率（TSR）和 Chaos，在 PlayStation 5 上以 720p–1080p 的动态内部分辨率跑到约 60 FPS。引擎上已发行的游戏，实际表现就是如此。


虚拟化路线——虚拟几何、虚拟阴影与虚拟纹理，再加上重建分辨率——增加了处理、流送与内存开销。时间重建、去噪和随机采样会引入噪声、重影、抖动与模糊。相对 UE4，Substrate、GPU Scene、RDG、更重的着色模型以及一般性的功能膨胀，会抬高基础渲染开销、着色器排列数、字节码体积、PSO 数量、编译时间与缓存体积。除 Chaos 之外，CPU 成本还因更重的场景维护、GPU Scene 上传、Lumen 更新、Nanite 流送、VSM 失效、世界分区以及渲染线程与 RHI 负载而上升。


与此同时，硬件正朝另一个方向走。Nintendo Switch 2 的预计生命周期为七到八年。性能远不如 PS5 的掌机已经进入主流，包括 Valve 的 Steam Deck 与 Steam Machine。AI 需求推高硬件成本。在这种背景下，UE5 的性能目标与游戏实际要跑的机器越来越脱节，整套渲染堆栈也更适合影视、虚拟制作和高端 PC，而不是面向大众市场硬件的长期游戏开发。

Vite 采取相反的立场：优先考虑高视觉保真度，同时在控制台级硬件上以高原生分辨率保持严格的帧时间预算。

!!! note
    在 Vite 中运行的带光线追踪全局光照、光线追踪反射与曲面细分的场景，优于 UE 5.7 里完全没有光线追踪、Lumen、Nanite 或曲面细分的同一场景。这在 RTX 4080 Super 和 RDNA2 RX 6700（相当于 PS5）上能保持 4K 原生分辨率，在 Steam Deck 硬件上也能保持原生分辨率。


## Vite 是由什么组成的

Vite 最初从 NvRTX 4.27 Caustics 分支而来。NvRTX 在 Epic 标准 4.27 上加入了 DX12、光线追踪与渲染改进，以及 DLSS、NVIDIA Reflex、改进的降噪器和完整的光线追踪支持，包括由 DDGI 照明的光线追踪反射。

在该基础之上：

- Epic 的 UE 4.27 Plus 分支已完全合并。
- NVIDIA 的 NvRTX 5.0 分支已合并。
- 集成了 AMD 引擎分支的渲染功能。
- 发布分支中有 300 多个从 UE 5.0 到 5.8 时代的向后移植，其中 1,200 多个集成在内部暂存分支中。

集成工作由熟悉虚幻引擎源码的引擎程序员完成：他们使用适当的代码保护与托管着色器排列，把每个精选的 UE5 改动手工适配进 Vite 代码库，而不是盲目合并。

凭借 PhysX 加上 DDGI 与 SSGI 组合的光照管线，Vite 与用来推出《总决赛》（*The Finals*）的定制虚幻引擎构建非常相似。

## 核心特性

**[动态 DDGI](../Rendering/DDGI-Dynamic.md)。** Lumen 的无噪全局光照替代方案。与软件 Lumen 相比反射质量更高、漏光更少，与硬件 Lumen 的反射相当，帧率通常约为两倍。DDGI 已用于《地铁：离去》、《守望先锋 2》、《总决赛》、《控制》、《巫师 3》、《战锤 40,000：暗潮》、《DOOM：黑暗时代》、《夺宝奇兵：古老之圈》、《007 曙光》、《羊蹄之魂》和《星球大战：亡命之徒》（含 Switch 2 版本）。

**[静态 DDGI](../Rendering/DDGI-Static.md)。** 预计算模式的预计算时间近乎即时，弹射保真度高于传统预计算光照，对移动对象的覆盖也更好，并且能在完全不支持光线追踪的 GPU 上使用。

**[PhysX 3.4](../Physics/PhysX.md).** 稳定、经过商业验证，并且在 Vite 中升级为在较新的 Clang 版本下构建，以获得有意义的编译器优化收益。内部压力测试显示，在物理限制场景中，Chaos 的运行速度比 PhysX 慢五倍以上。

**[RTXDI](../Rendering/RTXDI.md)。** MegaLights 的低噪替代方案，采用独立形式，而非 UE 5.1 及更高版本 NvRTX 分支里的 Lumen 集成版本。

**[曲面细分（Tessellation）](../Rendering/Tessellation.md)。** 基于距离与位移的几何细节，无需承担 Nanite 的开销。

**[完整的光线追踪套件](../Rendering/Ray-Tracing.md)。** 反射、环境光遮蔽、阴影、天光、半透明、焦散、直接光照、逐像素光线追踪 GI 和路径追踪——《黑神话：悟空》所用的渲染堆栈。

有关完整列表，请参阅[发行说明](../EngineOverview/Release-Notes.md)。

## UE4 不是已弃用的代码库吗？


这是个合理的问题。答案是：虚幻引擎 4 仍在为近年 AAA 作品提供支持——《最终幻想VII：重生》（4.26，2024）、《剑星》（4.26，2024）、《往日不再：重制版》（4.11，2025）、《三角洲行动》（4.22，2026）、《真人快打 1》（4.27，2023）、《马力欧与路易吉：兄弟齐航》（4.26，2024）、《碧琪公主：表演时刻！》（4.26，2024）、《皮克敏 4》（4.26，2023）、Square Enix 的《勇者斗恶龙VII：重制版》（4.27，2026），以及即将推出的《最终幻想VII：启示录》（4.27，2027）。它们全都随游戏附带 PhysX。

这些作品留在 UE4 上，是为了保住特定功能并达成保真度与性能目标。UE4 也继续通过 4.27 Plus 分支收到来自主要工作室的更新，并且仍是任天堂平台上的优先项。

Vite 的计划是继续演进这套代码库：优化核心系统、现代化渲染核心、改进 UI 并更新工具链，而不是把 4.27 当成冻结的产物。基础版本选择背后的技术原因见 [为何选择 NvRTX 4.27](../EngineOverview/Why-NvRTX-427.md)。

## 另请参阅

- [性能目标](../EngineOverview/Performance-Targets.md)
- [为什么选择 NvRTX 4.27](../EngineOverview/Why-NvRTX-427.md)
- [UE4 与 UE5 成本分析](../EngineOverview/UE4-Versus-UE5-Cost-Analysis.md)
- [入门](Getting-Started.md)
