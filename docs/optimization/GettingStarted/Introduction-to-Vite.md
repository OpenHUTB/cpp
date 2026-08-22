# Vite 简介

<tldr>
<p>
Vite 是 Unreal Engine 4.27 面向生产的分支，基于 NVIDIA 的 NvRTX Caustics 分支。它保留了 PhysX 3.4 和与引擎无关的光线追踪管线，并用在高帧速率下达到本机分辨率的更轻的替代方案取代了 Epic 的 UE5 功能堆栈（Lumen、Nanite、VSM、TSR、Chaos）。
</p>
</tldr>

Unreal Engine Vite 专为专业游戏开发而构建，支持当前正在制作的游戏。其长期目标是不断发展的现代引擎，提供与专有的内部引擎竞争的 CPU 和渲染吞吐量，并针对当代控制台硬件提供持续的性能、稳定性和图形管线工作。


目标足够具体，可以证伪：**在每毫秒保真度和模拟规模上击败 Epic 的 UE5**，并在这两个方面与专有 AAA 引擎竞争。每毫秒的保真度是[性能目标](../EngineOverview/Performance-Targets.md)和[UE4 与 UE5 成本分析](../EngineOverview/UE4-Versus-UE5-Cost-Analysis.md)衡量的。模拟规模是[物理](../ProjectsAndDemos/Physics-Cube-Bench.md)和[角色](../ProjectsAndDemos/400-Characters-CMC-Bench.md)基准测试所测量的。 Vite 目前没有获胜的地方，本手册是这么说的。

## 设计论证

Epic 的虚幻引擎 5.7 和 5.8 使用 Lumen、Nanite、虚拟阴影贴图、时间超分辨率和 Chaos，在 PlayStation 5 上以 720p–1080p 的动态内部分辨率实现大约 60 FPS。这就是引擎上发布的游戏在实践中所展示的内容。


虚拟化方法——虚拟几何、阴影和纹理，加上重建分辨率——增加了处理、流媒体和内存开销。时间重建、去噪和随机采样会引入噪声、重影、不稳定性和模糊。相对于 UE4，Substrate、GPU 场景、RDG、较重的着色器模型和一般功能扩展会增加基本渲染器开销、着色器排列计数、字节码大小、PSO 计数、编译时间和缓存大小。除了混沌之外，CPU 成本还通过更繁重的场景维护、GPU 场景上传、流明更新、Nanite 流、VSM 失效、世界分区以及渲染线程和 RHI 工作负载而增加。


与此同时，硬件正在向另一个方向发展。 Nintendo Switch 2 的预计使用寿命为七到八年。功能远不如 PS5 的掌上电脑现已成为主流市场，包括 Valve 的 Steam Deck 和 Steam Machine。人工智能需求下，硬件成本不断上涨。在此背景下，UE5 的性能目标看起来与游戏实际运行的机器越来越不一致，而且渲染堆栈可以说更适合电影、虚拟制作和高端 PC，而不是跨大众市场硬件的可持续长期游戏开发。

Vite 采取相反的立场：优先考虑高视觉保真度，同时在控制台级硬件上以高原生分辨率保持严格的帧时间预算。

!!! 注意
    在 Vite 中运行的具有光线追踪全局光照、光线追踪反射和曲面细分的场景优于 UE 5.7 中完全没有光线追踪、Lumen、Nanite 或曲面细分的相同场景。这在 RTX 4080 Super 和 RDNA2 RX 6700（相当于 PS5）上保持 4K 原生分辨率，在 Steam Deck 硬件上保持原生分辨率。


## Vite 是由什么组成的

Vite 最初是 NvRTX 4.27 Caustics 的一个分支，它在 Epic 标准 4.27 分支上添加了 DX12、光线追踪和渲染改进，以及 DLSS、NVIDIA Reflex、改进的降噪器和全面的光线追踪支持，包括 DDGI-lit 光线追踪反射。

在该基础之上：

- Epic 的 UE 4.27 Plus 分支已完全合并。
- NVIDIA 的 NvRTX 5.0 分支已合并。
- 集成了 AMD 引擎分支的渲染功能。
- 发布分支中有 300 多个从 UE 5.0 到 5.8 时代的向后移植，其中 1,200 多个集成在内部暂存分支中。

集成工作由具有丰富虚幻引擎源代码经验的引擎程序员完成，他们使用适当的代码保护、托管着色器排列以及将每个精心挑选的 UE5 更改手动调整为 Vite 代码库，而不是盲目合并。

借助 PhysX 以及组合的 DDGI 和 SSGI 光照管线，Vite 与用于启动 *The Finals* 的定制虚幻引擎构建非常相似。

## Headline 特色

**[动态 DDGI](../Rendering/DDGI-Dynamic.md).** Lumen 的无噪声全局光照替代品。与软件流明相比，反射质量更高，漏光更少，与反射的硬件流明相当，并且通常约为帧速率的两倍。 DDGI 已在《地铁：离去》、《守望先锋 2》、《总决赛》、《控制》、《巫师 3》、《战锤 40,000：暗潮》、《DOOM：黑暗时代》、《夺宝奇兵：夺宝奇兵》、《007 曙光》、《羊蹄之魂》和《星球大战亡命之徒》（包括 Switch 2 版本）中应用。

**[静态 DDGI](../Rendering/DDGI-Static.md).** 烘焙模式具有近乎即时的烘焙时间、比传统烘焙照明更高的反弹保真度以及更好的移动对象覆盖范围，可在根本不支持光线追踪的 GPU 上使用。

**[PhysX 3.4](../Physics/PhysX.md).** 稳定、经过商业验证，并且在 Vite 中升级为在较新的 Clang 版本下构建，以获得有意义的编译器优化收益。内部压力测试显示，在物理限制场景中，Chaos 的运行速度比 PhysX 慢五倍以上。

**[RTXDI](../Rendering/RTXDI.md).** MegaLights 的噪音较小的替代品，以其独立形式而不是 UE 5.1 和更高版本的 NvRTX 分支中的 Lumen 集成版本。

**[细分曲面（Tessellation）](../Rendering/Tessellation.md).** 距离和位移驱动的几何细节，无需 Nanite 的开销。

**[完整的光线追踪套件](../Rendering/Ray-Tracing.md).** 反射、环境光遮挡、阴影、天光、半透明、焦散、直接光照、每像素光线追踪 GI 和路径追踪——《黑神话：悟空》所搭载的渲染堆栈。

有关完整列表，请参阅[发行说明](../EngineOverview/Release-Notes.md)。

## UE4 不是已弃用的代码库吗？


这是一个公平的问题，答案是虚幻引擎 4 继续为最近的 AAA 版本提供支持：《最终幻想 VII 重生》（2024 年 4.26 版）、《星辰之刃》（2024 年 4.26 版）、《往日不再重制版》（2025 年 4.11 版）、《三角洲特种部队》（2026 年 4.22 版）、《真人快打 1》（2023 年 4.27 版）、《马里奥与路易吉：兄弟情谊 (4.26, 2024)、桃子公主：演出时间！ （2024 年 4.26 日）、《皮克敏 4》（2023 年 4.26 日）、Square Enix 的《勇者斗恶龙 VII 重制版》（2026 年 4.27 日）以及即将推出的《最终幻想 VII：启示录》（2027 年 4.27 日）。它们都搭载了 PhysX。

这些作品保留在 UE4 上，以保留特定功能并满足保真度和性能目标。 UE4 还继续通过 4.27 Plus 分支接收来自主要工作室的更新，并且仍然是任天堂平台的优先事项。

Vite 的计划是继续升级该代码库：优化核心系统、现代化渲染核心、改进 UI 并更新工具链，而不是将 4.27 视为冻结的工件。请参阅 [为什么选择 NvRTX 4.27](../EngineOverview/Why-NvRTX-427.md) 了解基本版本选择背后的技术原因。

## 另请参阅

- [性能目标](../EngineOverview/Performance-Targets.md)
- [为什么选择 NvRTX 4.27](../EngineOverview/Why-NvRTX-427.md)
- [UE4 与 UE5 成本分析](../EngineOverview/UE4-Versus-UE5-Cost-Analysis.md)
- [入门](Getting-Started.md)
