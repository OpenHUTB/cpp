# 系统要求


Windows 10 或 11 x64。建议使用支持光线追踪的 GPU，但并非必需——[静态 DDGI](../Rendering/DDGI-Static.md) 与光栅路径可以在完全不支持 DXR 的硬件上运行。动态 DDGI 则设计为可下探到 GTX 1060 6 GB 级 GPU。


虚幻引擎有两组不同的要求：一组是运行在引擎上构建的内容需要什么，另一组是构建引擎本身需要什么。两者差别很大，后者重得多。

## 在引擎上运行内容

虚幻引擎的设计初衷就是在广泛的硬件区间内伸缩，这也是选择 DDGI 而非 Lumen 的首要目的。

| | 最低要求                         | 推荐 |
|---|---------------------------------|---|
| 操作系统 | Windows 10 x64                  | Windows 11 x64 |
| GPU (无光线追踪) | 任何支持 DirectX 11 的 GPU      | &mdash; |
| GPU (动态 DDGI) | GTX 1060 6&nbsp;GB 或任何 RDNA2 | RTX 2060 /RX 6600 或更好 |
| GPU (完整的 RT 套件) | RTX 2060 / RX 6600              | RTX 3070 / RX 6700 XT 或更好 |
| VRAM | 6&nbsp;GB                       | 8&nbsp;GB 或更多 |

动态 DDGI 适用于 GTX 1060 6 GB 级及以上的 GPU。低于这个级别，或使用任何不支持光线追踪的 GPU 时，请改用[静态 DDGI](../Rendering/DDGI-Static.md)：它把辐照度预计算进探针体积，运行时没有光线追踪开销。

### 测试硬件

这些是虚幻引擎主动做基准测试与调优的配置。下列测试硬件由该分支的主要开发者自持。



| 供应商 | 硬件                        | 为什么这很重要                               |
|---|---------------------------------|----------------------------------------------|
| AMD | RX 6700 (RDNA2)                 | 与 PlayStation 5 最接近的桌面游戏       |
| AMD | Steam Deck LCD Van Gogh (RDNA2) | 掌机类                               |
| AMD | RX 9600XT (RDNA4)               | 中级 GPU                                 |
| NVIDIA | RTX 2060 (Turing)               | HW RT 和 DLSS 的下限                 |
| NVIDIA | RTX 3060 (Ampere)               | 匹配 Steam 最常见的 GPU                |
| NVIDIA | RTX 4080 Super (Ada Lovelace)   | 4K 原生捕获的上限参考 |

测试在 4K 原生显示器或电视（从 6700/RTX 4060 起步）上最有意义，因为引擎的核心主张就是保持原生分辨率，而不是从较低的内部分辨率超分上来。

如果您想展示硬件中的结果（如演示中的结果），Discord 上的 `#showcase` 频道是协调测试的地方。

## 从源代码构建引擎

构建受 CPU、RAM 和磁盘限制。将这些视为实际指导而不是硬性限制。

| | 实用最低限度           | 舒适                         |
|---|-----------------------------|-------------------------------------|
| CPU | 6核/12线程        | 16核/32线程或更多       |
| 内存 | 24&nbsp;GB                  | 32&nbsp;GB                          |
| 硬盘 | 具有 50GB 空间的 SATA SSD | NVMe SSD 157GB 空间（全尺寸） |
| 操作系统 | Windows 10 x64              | Windows 11 x64                      |

包含依赖项、中间体和构建编辑器的虚幻引擎 4.27 源代码树非常大 -预算远超过 100 GB，如果您在其旁边生成已安装的构建，则预算会更多，因为这会将第二个副本写入“LocalBuilds\Engine\Windows\”。机械硬盘可以工作，但每一步都会很痛苦；建议使用SSD。

编译时间几乎与核心数量成线性关系。 Ryzen 9 9950X3D 的完整构建大约需要 14 分钟。您可以通过排除不针对的平台并删除可选内容来回收大量的时间和磁盘空间 -请参阅 [精简指南](../Performance/Debloat-Guide.md) 和 [从源代码构建](Build-From-Source.md) 中描述的设置预设。

您还需要特定的编译器和 SDK。这不是可选的，在[工具链要求](Toolchain-Requirements.md) 中单独介绍。

## 参见

- [工具链要求](Toolchain-Requirements.md)
- [从源代码构建](Build-From-Source.md)
- [平台支持](../Platforms/Platforms.md)
- [性能目标](../EngineOverview/Performance-Targets.md)
