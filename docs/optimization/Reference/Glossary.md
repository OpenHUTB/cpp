# 术语表

本文档中使用的术语。Vite 特有的术语已标记；其余术语为本文档默认使用的标准 Unreal 或图形术语。

## Vite 和 fork 术语

* Vite

    此引擎：基于 NVIDIA NvRTX 分支的虚幻引擎 4.27 的一个分支，专注于 4.27 版本上游未包含的性能和渲染技术。引擎关联标识符为 UE_ViteFork。

* NvRTX

    NVIDIA 的公开虚幻引擎分支，其光线追踪技术领先于上游。Vite 借鉴了 **NvRTX 4.27 的焦散特性**和 **NvRTX 5.0**。

* VITE_* 开关

    一个编译时预处理器开关，用于控制某个功能是否被构建到引擎中。定义于 `Engine\Source\Runtime\Core\Public\Misc\CoreDefines.h`。更改此开关需要重新构建引擎。请参阅[编译时开关](../Performance/Compile-Time-Switches.md)。

* 编译排除

    一个功能，其着色器排列和代码路径通过编译时开关从构建中排除。其控制台变量仍然存在并已成功设置；但不会渲染任何内容。 Vite 中“此设置无效”的最常见原因。

* 精简

    从引擎树中移除未使用的平台二进制文件、模板、示例和插件，以减少磁盘占用空间。请参阅[精简指南](../Performance/Debloat-Guide.md)。

* 已安装版本

    预编译的可再分发引擎，其工作方式类似于启动器安装。无法编译引擎 C++。请参阅[已安装版本](../Tools/Installed-Builds.md)。


## 渲染

* DDGI

    动态漫反射全局光照（Dynamic Diffuse Global Illumination, DDGI）。NVIDIA 基于探针的全局光照技术，由 RTXGI 插件提供。探针存储辐照度和深度信息；表面对周围的探针进行采样以获取间接光。Vite 推荐使用动态全局光照。参见[动态 DDGI](../Rendering/DDGI-Dynamic.md)。

* SSGI

    屏幕空间全局光照（Screen Space Global Illumination, SSGI）。从渲染帧中获取间接光。成本低且细节丰富，但仅限于屏幕上的内容。通常与 DDGI 叠加使用，而不是单独使用。参见 [SSGI](../Rendering/SSGI.md)。

* RTXDI

    RTX 直接光照（RTX Direct Illumination, RTXDI）。适用于包含大量光源的场景的光线追踪直接光照，使用储层重采样。Vite 使用的是独立版本，而不是 UE 5.1 及更高版本中集成的 Lumen 版本。默认情况下编译时会禁用。参见 [RTXDI](../Rendering/RTXDI.md)。


* ReSTIR / 储层重采样

    RTXDI 背后的采样技术。每个像素都维护着一个光样本库，该样本库在空间和时间上都经过精细化处理，因此少量光线即可近似模拟更大的光照集。

* 路径追踪

    完整的光传输模拟。参考质量，而非运行时目标。默认情况下会被编译排除。请参阅[路径追踪](../Rendering/Path-Tracing.md)。

* DXR

    DirectX 光线追踪。Vite 中所有光线追踪功能背后的 API，也是它们都需要 DirectX 12 的原因。

* PSO

    管线状态对象（Pipeline State Object, PSO）。着色器和固定功能状态的编译包。光线追踪 PSO 体积庞大且构建速度慢，而 `VITE_RT_PSO_DEBLOAT` 的存在正是为了减少这种情况。请参阅[着色器编译和 PSO](../Performance/Shader-Compilation-And-PSO.md)。

* 着色器排列

    针对特定功能组合的着色器的一种编译变体。排列数量会随着着色模型、可选功能和质量级别的增加而增加：每个着色模型、每个可选功能和每个质量级别都会扩大排列集合。这是影响着色器编译时间和包大小的主要因素。

* 焦散

    光线通过透明或镜面反射表面发生折射或反射而聚焦。Vite 拥有来自 NvRTX 焦散分支的网格焦散和水焦散。默认情况下编译时禁用。参见[RT 半透明和焦散](../Rendering/RT-Translucency-And-Caustics.md)。

* Callisto BRDF

    Vite 特有的角色和皮肤着色模型，通过重新利用标准材质引脚，实现了逆反射、漫反射菲涅尔和阴影终止平滑。参见[着色模型](../Rendering/Shading-Models.md)。

* 阴影终止

    曲面上光照区域和非光照区域之间的边界。其硬度是实时渲染最明显的特征之一；Callisto BRDF 可以直接控制阴影终止。

* SMAA

    亚像素形态学抗锯齿（Subpixel Morphological Anti-Aliasing, SMAA）。Vite 推荐的抗锯齿方法。它基于空间而非时间，因此不会像 TAA 那样引入重影和拖影。参见[抗锯齿](Anti-Aliasing.md)。

* TAA

    时间抗锯齿（Temporal Anti-Aliasing, TAA）。虚幻引擎的默认设置。跨帧累积采样；效果显著，但会在运动时引入重影、拖影和画面柔化。

* HBAO+

    NVIDIA 的基于地平线的环境光遮蔽。比标准 SSAO 质量更高，通过乘以屏幕空间 AO 缓冲区来实现。尽管帮助文档中另有说明，但它在 DX11 和 D3D12 上都能正常工作。参见[环境光遮蔽](../Rendering/Ambient-Occlusion.md)。

* 曲面细分

    GPU 上对三角形进行硬件细分，从而驱动置换贴图。UE5 中移除了曲面细分，转而使用 Nanite；Vite 中保留了曲面细分。参见[曲面细分](../Rendering/Tessellation.md)。

* TressFX

    AMD 的基于毛发的渲染和模拟系统。5.0 版本已包含在内。它是虚幻引擎 Groom 系统的替代方案。参见[毛发渲染](../Rendering/Hair-Rendering.md)。

* NRD / ReLAX

    NVIDIA 实时降噪器。消除低采样数光线追踪中固有的噪声。默认启用。


## 超分辨率

* DLSS

    NVIDIA 深度学习超分辨率采样（NVIDIA Deep Learning Super Sampling, DLSS）。以较低分辨率渲染并使用 AI 模型进行重建。仅限 NVIDIA 硬件。Vite 捆绑了 4.5 版本。参见[超分辨率](../Rendering/Upscalers.md)部分。

* DLAA

    深度学习抗锯齿（Deep Learning Anti-Aliasing, DLAA）。DLSS 的重建以原生分辨率应用——注重画质而非性能。

* 光线重建

    DLSS 组件，用学习到的模型替换手动调整的光线追踪降噪器。

* FSR

    AMD FidelityFX 超分辨率（FidelityFX Super Resolution）。跨厂商图像放大。Vite 捆绑了 FSR 4.1.1 版本，该版本仅支持 DX12。

* XeSS

    Intel Xe 超采样（Xe Super Sampling, XeSS）。跨厂商，在 Intel Arc 硬件上速度更快。Vite 捆绑了 3.0.5 版本。

* NIS

    NVIDIA 图像缩放（NVIDIA Image Scaling, NIS）。空间放大，不包含时间分量。价格最低，画质也最差，但几乎适用于所有平台。

* Streamline

    NVIDIA 的插件层，提供 DLSS 帧生成和 Reflex 功能。

* 帧生成

    在渲染帧之间合成中间帧。提高显示帧率，但不降低输入延迟，并且会略微增加输入延迟。

* Reflex

    NVIDIA 的输入延迟降低技术，在帧生成生效时尤为重要。


## 物理引擎

* PhysX

    NVIDIA 的物理引擎，虚幻引擎 4.27 及之前版本中的默认引擎。Vite 保留了 PhysX，而没有迁移到 Chaos。参见 [PhysX](../Physics/PhysX.md)。

* Chaos

    Epic 的物理引擎，在 UE5 中取代了 PhysX。Vite 未使用 Chaos。

* Apex Destruction

    PhysX 的可破坏网格系统。在 UE5 中移除；Vite 保留了该功能。参见 [Destruction 和 Cloth](../Physics/Destruction-And-Cloth.md)。

* Apex Cloth

    PhysX 的布料模拟。在 UE5 中移除；Vite 保留了该功能。


* Blast

    NVIDIA 的新型破坏库，比 Apex Destruction 更灵活。包含独立的运行时插件和创作插件。

* 固定时间步长

    无论帧时间如何，物理计算都以恒定的增量推进，从而实现与帧速率无关的确定性。在 Vite 中，该功能是可选的，位于 `VITE_PHYSX_FIXED_TIMESTEP` 之后。参见[固定时间步长](../Physics/Fixed-Timestep.md)。

* 子步进

    将帧的物理更新细分为更小的步骤以提高稳定性。这与固定时间步长不同，后者在所有帧中都保持相同的步长。

* CCD
    
    连续碰撞检测（Continuous Collision Detection, CCD）。它会在帧之间扫描物体的运动，而不是仅仅检测其端点，从而防止快速移动的物体穿过薄弱的几何体。比离散碰撞检测更耗费资源。

* CMC

    角色移动组件（Character Movement Component, CMC）。虚幻引擎的标准角色移动组件，也是人群场景中每个实例开销最大的组件之一。参见 [400 个角色 CMC 基准测试](../ProjectsAndDemos/400-Characters-CMC-Bench.md)。


## 引擎和构建

* DDC

    派生数据缓存（Derived Data Cache）。用于存储已编译的着色器、已处理的纹理和其他派生工件。参见[缓存管理](../Tools/Cache-Management.md)。

* UAT / UBT

    虚幻自动化工具（Unreal Automation Tool, UAT）和虚幻构建工具（Unreal Build Tool, UBT）。UAT 驱动高级操作，例如安装构建和打包；UBT 负责编译。

* BuildGraph

    UAT 的 XML 驱动的构建编排系统，用于生成已安装的构建。

* ISM / HISM

    实例化静态网格体（Instanced Static Mesh, ISM）和分层实例化静态网格体（Hierarchical Instanced Static Mesh, HISM）组件。在少量绘制调用中渲染同一网格体的多个副本。[PhysX 实例化子系统](../Physics/Instanced-Physics.md)的渲染端。

* ABI

    应用程序二进制接口（Application Binary Interface, ABI）。在 Vite 的上下文中，指 CPU 和 GPU 之间共享结构的二进制布局。更改 ABI 会破坏 PSO 缓存、序列化和光线追踪的稳定性，并会导致贡献立即被拒绝。参见[编码指南](../Contributing/Coding-Guidelines.md)。

* ACL

    动画压缩库（Animation Compression Library, ACL）。用更优的尺寸质量平衡曲线替换了虚幻引擎内置的动画压缩。

* 运动匹配

    在运行时，通过将当前姿态和轨迹与数据库进行匹配来选择动画帧，而不是遵循状态机。由 Motion Symphony 插件提供。


## 另请参阅

- [控制台变量](./Console-Variables.md)
- [编译时开关](../Performance/Compile-Time-Switches.md)
- [常见问题解答](./FAQ.md)
