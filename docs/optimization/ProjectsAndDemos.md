# 项目和演示

可下载的项目展示了 Vite 的各项功能并提供了性能数据。其中一些项目是[性能目标](./EngineOverview/Performance-Targets.md)的基准测试。

这些项目分为两类：展示引擎视觉效果的**演示项目**，以及评估其成本的**基准测试项目**。如果您正在考虑 Vite 是否适合您的项目，基准测试项目将更有参考价值。


## 演示项目

| 项目 | 演示 |
|---|---|
| [技术演示项目](./ProjectsAndDemos/Tech-Demo-Project.md) | DDGI、SSGI、Apex Destruction 和 Apex Cloth 在一个包中 |
| [风格化光线追踪演示](./ProjectsAndDemos/Stylized-Raytracing-Demo.md) | 光线追踪应用于非真实感艺术指导 |
| [Callisto BRDF 演示](./ProjectsAndDemos/Callisto-BRDF-Demos.md) | 自定义皮肤和角色着色模型 |
| [废弃的公寓](./ProjectsAndDemos/Abandone-Apartment.md) | 室内照明场景 |
| [阁楼场景](./ProjectsAndDemos/Attic-Scene.md) | 室内照明场景 |
| [古惑狼：时空扭曲](./ProjectsAndDemos/Crash-Bandicoot.md) | 基于 UE4 构建的完整游戏关卡 |

## 基准测试项目

| 项目 | 测量 |
|---|---|
| [400 个角色 CMC 工作台](./ProjectsAndDemos/400-Characters-CMC-Bench.md) | 角色移动组件的大规模成本 |
| [物理立方台](./ProjectsAndDemos/Physics-Cube-Bench.md) | PhysX 刚体吞吐量 |
| [PhysX 测试](./ProjectsAndDemos/PhysXTest.md) | 一般 PhysX 行为和稳定性 |
| [PhysX 实例子系统](./ProjectsAndDemos/PhysX-Instanced-Subsystem.md) | 实例刚体的数量是单个参与者无法达到的 |

## 使用基准

基准测试比帧速率数字所显示的信息更丰富。在`stat unit`可见的情况下运行它们，并注意哪个线程是限制——这会告诉您类似的工作负载将花费您多少费用，以及适用哪些 Vite 优化。请参阅[性能分析](Profiling.md)。

这些项目的数据反映了它们所用的硬件配置。Vite 的[性能基准](Coding-Guidelines.md)是 ARM 架构的 1GHz 左右的 CPU，远低于开发桌面电脑的性能。在你机器上运行流畅的基准测试结果，可能并不适合你的目标平台。

## 另请参阅

- [性能目标](./EngineOverview/Performance-Targets.md)
- [性能分析](./Performance/Profiling.md) 
- [UE4 与 UE5 成本分析](./EngineOverview/UE4-Versus-UE5-Cost-Analysis.md)
