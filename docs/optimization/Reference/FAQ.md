# 常见问题解答

以下是常见问题，附有简短解答和完整解释的链接。

## 关于分支

### 为什么选择 4.27 而不是 UE5？

因为对于很多项目来说，UE5 的主要功能带来的收益远大于成本。Nanite、Lumen 和 Chaos 本身都有基础成本，无论是否使用，而且 UE5 的核心类和 tick 开销也更高。Vite 以 4.27 较低的基准成本为基础，并添加了真正值得拥有的渲染技术。

详细的论证（包含数据）请参见[为什么选择 NvRTX 4.27](../EngineOverview/Why-NvRTX-427.md) 以及 [UE4 与 UE5 的成本分析](../EngineOverview/UE4-Versus-UE5-Cost-Analysis.md)。


### Vite 可以直接替代 UE5 4.27 吗？

差不多，但并不完全一样。Vite 为了提升性能，修改了一些引擎默认设置，例如重叠事件处理、可缩放性设置、帧刷新行为、光线追踪剔除等等。从 UE5 4.27 迁移过来的项目大部分都能正常运行，但建议阅读[引擎默认设置]((../Performance/Engine-Defaults.md))以了解具体更改。


### 我可以使用 UE5 商城资源吗？

不能直接使用。资源降级插件会将 UE5 资源转换为 UE5 4.27 版本，并移除 UE5 4.27 版本中不存在的数据，例如 Nanite 几何体。UE5 4.27 版本中不存在的功能无法移植，只能移除。请参阅[从 UE5 迁移](../GettingStarted/Migrating-From-UE5.md)。


### Vite 支持哪些平台？

Windows 64 位系统，主要支持 DirectX 12。其他平台继承了 UE5 4.27 的支持，但未经测试和优化。请参阅[平台](../Platforms/Platforms.md)部分。


## 安装和构建

### 我需要什么工具链？

Vite 目前基于 Visual Studio 2026、MSVC 14.50 和 Windows SDK 10.0.26100 开发。

**警告：** `ViteSetup.bat` 的环境检查仍然强制要求使用 Visual Studio 2022、MSVC 14.44 和 SDK 10.0.26100.7705，因此在新工具链上会失败。如果遇到此问题，请通过手动路径或单独的菜单选项进行构建。请参阅[工具链要求](../GettingStarted/Toolchain-Requirements.md)。


### 在全新克隆的环境中，构建立即失败。

最常见的原因是缺少 .NET Framework 4.5 引用程序集，而最新的 Visual Studio 安装程序默认情况下不再包含这些程序集。请参阅[构建故障排除](../GettingStarted/Build-Troubleshooting.md)。

### 我需要从源代码构建吗？

仅当您需要修改引擎 C++ 代码或更改[编译时开关](../Performance/Compile-Time-Switches.md)时才需要从源代码构建。对于其他所有情况，[安装构建](../GettingStarted/Install-Binary-Build.md)启动速度更快，并且占用磁盘空间更少。


### 构建需要多长时间？

在典型的硬件上，从源代码构建需要几个小时，而安装构建则需要更长时间。着色器编译在之后的首次启动中占据主导地位。请参阅[着色器编译和 PSO](../Performance/Shader-Compilation-And-PSO.md)。


### 引擎占用过多磁盘空间。

在 `devops\` 目录下运行精简套件。它会将平台二进制文件、模板、示例以及（可选的）插件从代码树中移出，并提供试运行模式和恢复文件夹，而不是直接删除。请参阅[精简指南](../Performance/Debloat-Guide.md)。


## 渲染

### 我设置了一个光线追踪控制台变量，但没有任何反应。

这是最常见的问题，答案几乎总是 `VITE_RT_PSO_DEBLOAT`。它的默认值为 `1`，会编译掉以下着色器排列组合：

- RTXDI (`r.RayTracing.SampledDirectLighting`)
- 路径追踪
- 光线追踪半透明效果和焦散系统
- 逐像素光线追踪全局光照
- 非延迟反射路径，以及由此产生的反射半透明效果和光线追踪反射捕获

控制台变量仍然存在且仍然有效。但没有任何渲染效果。请使用 `VITE_RT_PSO_DEBLOAT=0` 重新构建引擎以使用这些变量。请参阅[编译时开关](../Performance/Compile-Time-Switches.md)和[光线追踪](../Rendering/Ray-Tracing.md)。


### 哪些光线追踪功能开箱即用？

反射、阴影、环境光遮蔽和天空光。这四项在默认版本中可用；其余功能会被编译排除。完整表格请参见[光线追踪](../Rendering/Ray-Tracing.md)部分。


### 我应该使用哪种全局光照？

对于大多数项目，建议使用 DDGI 和 SSGI。DDGI 提供低频间接反射，而 SSGI 则添加了其探测网格无法解析的接触尺度细节。如果物体静止不动，静态 DDGI 更经济高效且效果​​更佳。逐像素光线追踪全局光照仅供参考，默认情况下会被编译排除。请参见[全局光照](../Rendering/Global-Illumination.md)部分。


### 为什么选择 SMAA 而不是 TAA？

TAA 的重影、拖影和运动模糊是结构性问题，而非调优问题。SMAA 是空间抗锯齿，不存在这些问题。它是 Vite 推荐的默认设置，可通过 `r.Vite.SMAA.Mode` 启用。请参见[抗锯齿](../Rendering/Anti-Aliasing.md)部分。


### 我应该使用哪种超分辨率器？

如果您使用的是 NVIDIA 硬件并且项目面向 NVIDIA 用户，请使用 DLSS。FSR 或 XeSS 可以跨厂商兼容。NIS 是一个低成本的备选方案，它不包含时间组件。大多数项目应该提供不止一种升频器。请参阅[超分辨率器](../Rendering/Upscalers.md)部分。


### 启用超分辨率后，体积雾看起来有问题。

`VITE_DLSS_PATCH` 可以修复这个问题。它添加了 `r.VolumetricFog.UseUpScaledSizeVolumetricFog`，该函数会以输出分辨率而不是内部分辨率渲染雾，同时还修复了半透明度问题。它的默认值为 `0`，因此需要重新构建。请参阅[超分辨率器](../Rendering/Upscalers.md)部分。


### Vite 是否支持 Nanite 或 Lumen？

不支持，而且以后也不会支持。这些是基于 UE5 假设构建的 UE5 系统。Vite 的解决方案是使用硬件细分和 LOD 来处理几何体，并使用 DDGI 和 SSGI 来处理全局光照。


## 物理引擎

### 为什么选择 PhysX 而不是 Chaos？

PhysX 在 Vite 的目标工作负载下速度更快、结果更可预测，并且保留了 UE5 中移除的 Apex Destruction 和 Apex Cloth。请参阅 [PhysX](../Physics/PhysX.md)。


### 如何获得确定性物理效果？

使用 `VITE_PHYSX_FIXED_TIMESTEP=1` 重新构建，然后启用 `p.VitePhysXFixedTimestep.Enabled`。如果没有编译时开关，控制台变量将不起作用。请参阅[固定时间步长](../Physics/Fixed-Timestep.md)。


### 我需要数千个物理对象

使用 [PhysX 实例化子系统](../Physics/Instanced-Physics.md)。传统的逐个物体物理引擎在处理几千个物体时，会受到 Actor 的开销限制，而不是求解器的限制。该子系统通过直接驱动实例化网格变换来消除这种开销。


## 性能

### 我应该从哪里开始优化？

统计单位（`stat unit），并确定游戏、绘制或 GPU 中哪个占用资源最多。优化错误的单位不会带来任何帧时间上的提升。请参阅[性能分析](../Performance/Profiling.md)。


### 我的游戏线程是瓶颈

常见的瓶颈，大致按出现频率排序：角色移动组件、动画评估、蓝图 tick 逻辑以及 Actor tick 开销。请参阅 [400 个角色 CMC 基准测试](../ProjectsAndDemos/400-Characters-CMC-Bench.md)和[性能目标](../EngineOverview/Performance-Targets.md)。


### 着色器编译耗时过长

在假设无法避免之前，请检查您的排列计数。每个使用的着色模型、每个启用的插件和每个质量级别都会增加排列计数。`VITE_RT_PSO_DEBLOAT=1` 已经移除了大部分光线追踪排列，这也是它成为默认值的原因。请参阅[着色器编译和 PSO](../Performance/Shader-Compilation-And-PSO.md)。


### 着色器的行为就像我的更改没有生效一样

缓存过期。首先尝试运行 `recompileshaders changed` 或 `recompileshaders global` 命令——这两种方法都比完全清除缓存要省事得多。如果这两种方法都无效，请运行 `WipeShaderCache.bat`。请参阅[缓存管理](../Tools/Cache-Management.md)部分。


## 插件

### 编辑器中缺少某个功能

请检查该插件是否已启用。大多数捆绑插件默认处于关闭状态。只有 RTXGI、NRD 和自定义启动画面默认启用。请参阅[已捆绑的插件](../Plugins/Bundled-Plugins.md)。


### 能否将插件 X 捆绑到项目中？

如果它与 4.21–4.27 版本兼容，采用宽松许可，并且其体积和编译成本与其价值相符，请提交申请。仅适用于 UE5 的插件不在本次讨论范围内。请参阅[提议的插件](../Plugins/Proposed-Plugins.md)。


### 精简脚本删除了我需要的插件

`ExcludedPlugins.txt` 文件包含的插件很多，因此会进行大幅度的清理。如果您以移动模式运行脚本，这些文件会位于引擎文件夹旁边的 `ViteDebloat_Moved` 文件夹中。如果您以删除模式运行脚本，这些文件将被删除，您需要从源代码控制系统中恢复。请参阅[精简指南](../Performance/Debloat-Guide.md)。


## 贡献指南

### 哪些修改会导致拉取请求最快被拒绝？

修改 ABI：光线追踪有效载荷位域、着色器可见枚举、打包的 RHI 或 RenderCore 位掩码、反射位掩码定义，或任何 CPU/GPU 共享的结构体布局。无论其他更改如何，这些都会被拒绝。请参阅[编码指南](../Contributing/Coding-Guidelines.md)。


### 我可以使用递归、虚函数或模板吗？

禁止使用递归。新的虚函数需要严格的理由。除非模板的编译时成本和二进制文件大小的增加是值得的，否则不建议使用模板。新的蓝图公开函数需要获得明确批准。


### 如何记录我的更改？

在同一个拉取请求中。新的控制台变量放在相关的功能页面，新的 `VITE_*` 开关放在[编译时开关](../Performance/Compile-Time-Switches.md)页面，更改的默认值放在[引擎默认值](../Performance/Engine-Defaults.md)页面。请参阅[文档贡献](../Contributing/Documentation-Contributions.md)。


## 另请参阅

- [控制台变量](./Console-Variables.md)
- [术语表](./Glossary.md)
- [编译时开关](../Performance/Compile-Time-Switches.md)
- [构建故障排除](../GettingStarted/Build-Troubleshooting.md)
