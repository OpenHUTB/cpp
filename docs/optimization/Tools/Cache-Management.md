# 缓存管理


引擎根目录下的 `WipeShaderCache.bat` 文件会删除引擎的派生数据缓存、中间着色器和着色器调试信息。当编辑器的行为与你的源代码更改不符时，就可以使用它。

虚幻引擎会积极地进行缓存。这使得迭代变得容易，但也意味着过时的缓存会导致正确的更改看起来不起作用，或者错误的更改看起来起作用。


## 脚本清除了什么

`WipeShaderCache.bat` 会删除引擎根目录下的三个目录：

| 目录 | 内容 |
|---|---|
| `Engine\DerivedDataCache` | 编译后的着色器、生成的纹理、构建的网格以及所有其他衍生产物 |
| `Engine\Intermediate\Shaders` | 中间着色器编译输出 |
| `Engine\Saved\ShaderDebugInfo` | 着色器调试符号和预处理后的源代码转储 |

它会将每个目录报告为已删除、未找到或失败。失败几乎总是意味着某个程序仍然持有该文件句柄。

**警告：** 运行脚本前，请关闭编辑器和所有 `ShaderCompileWorker.exe` 进程。着色器编译工作进程的生命周期可能比编辑器更长；如果删除失败，请检查任务管理器。


清除后，下次启动编辑器时会重新编译所有需要的文件。对于大型项目，这可能需要几十分钟甚至更长时间。请勿随意操作。


## 什么时候清除

* 您修改了 .usf 或 .ush 文件，但没有任何效果。

    着色器源文件的更改通常会被识别，但引擎级别的全局着色器更改有时不会被识别。请先尝试在控制台中运行 `recompileshaders global` 或 `recompileshaders changed` 命令——这比完全清除着色器要快得多。

* 您更改了编译时开关。

    更改 [VITE_RT_PSO_DEBLOAT](../Performance/Compile-Time-Switches.md)、`VITE_O_SSAO` 或任何其他 `VITE_*` 开关会改变着色器排列组合。引擎重建会处理 C++ 代码，但旧配置中缓存的着色器可能会残留。切换后请清除缓存。


* 你拉取了修改渲染器的引擎更改。

    合并操作如果更改了着色器代码或着色器贴图键控，可能会留下引擎无法正确读取的缓存。


* 您遇到的着色器编译错误与源代码不符。

    这是典型的缓存过期症状：报告的错误行在您查看的文件中不存在。


* 更换引擎后，编辑器启动时崩溃。

    在进行更深入的调查之前，值得一试，因为排除故障的成本不高。


## 什么时候不清除

清除 DDC 缓存是一种过于激进的做法，而且经常被用于解决一些它无法解决的问题。

- **项目级问题。** 该脚本会清除引擎的 DDC 缓存。您的项目拥有自己的 `DerivedDataCache` 文件夹以及 `Intermediate` 和 `Saved` 目录。引擎级清除操作不会影响这些目录。
- **运行时渲染问题。** 如果某个特效渲染错误但始终存在，那么这是代码或配置问题，而不是缓存问题。请先检查[编译时开关可用性表](../Rendering/Ray-Tracing.md)——许多光线追踪功能默认会被编译掉，它们的控制台变量也不会产生任何作用。
- **着色器编译时间过长。** 清除缓存只会让情况变得更糟，而不是更好。请参阅[着色器编译和 PSO](../Performance/Shader-Compilation-And-PSO.md)。 

## 更轻量级的选择

在彻底清除之前，请尝试以下方法：

| 方法 | 清除 | 花费 |
|---|---|---|
| `recompileshaders changed` | 仅修改的着色器 | 秒 |
| `recompileshaders global` | 全局着色器 | 不到一分钟 |
| `recompileshaders material <name>` | 单个材质 | 秒 |
| Delete the project's `Intermediate\` | 项目构建中间文件 | 项目重建 |
| 仅删除 `Engine\Intermediate\Shaders` | 中间着色器输出，保留 DDC | 部分重新编译 | 

最后一点值得注意：DDC 是重建过程中开销最大的部分。如果您只需要清除中间着色器状态，手动删除该目录比运行整个脚本要便宜得多。


## 缓存位置

| 缓存 | 路径 | 是否被脚本清除 |
|---|---|---|
| 引擎 DDC | `Engine\DerivedDataCache` | 是 |
| 引擎中间着色器 | `Engine\Intermediate\Shaders` | 是 |
| 着色器调试信息 | `Engine\Saved\ShaderDebugInfo` | 是 |
| 项目 DDC | `<Project>\DerivedDataCache` | 否 |
| 项目中间文件 | `<Project>\Intermediate` | 否 |
| 共享/网络 DDC | 根据 `BaseEngine.ini` 配置 | 否 |
| 本地用户 DDC | `%LOCALAPPDATA%\UnrealEngine\Common\DerivedDataCache` | 否 |

**注意：** 如果您的团队使用共享网络 DDC，则清除本地缓存通常是徒劳的：本地缓存会从共享 DDC 重新填充，包括导致清除的过期条目。在断定是本地缓存的问题之前，请先确认共享 DDC 的状态。


## 磁盘占用空间

在开发过程中，引擎的 DDC 文件会无限增长。对于使用多种材质和光线追踪方案的项目，它的大小可能达到数十 GB。定期清除 DDC 文件是回收磁盘空间的合理方法，但需要重新编译。

如果磁盘空间是您真正关心的问题，请参阅[精简指南](../Performance/Debloat-Guide.md)，其中介绍了更大、更持久的节省方法。


## 另请参见

- [着色器编译和 PSO](../Performance/Shader-Compilation-And-PSO.md)
- [Compile-Time Switches](../Performance/Compile-Time-Switches.md)
- [Build Troubleshooting](../GettingStarted/Build-Troubleshooting.md)
- [精简指南](../Performance/Debloat-Guide.md)
