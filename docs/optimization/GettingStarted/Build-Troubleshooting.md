# 构建故障排除

Vite 中的大多数构建失败，其实都是伪装成其他问题的工具链故障。在排查其他问题之前，请务必确认你安装了且仅安装了一套 MSVC 工具集和一套 Windows SDK，且均符合[工具链要求](Toolchain-Requirements.md)。

本页面汇总了最常见的错误、它们的实际含义以及解决方法。


## 工具链错误

### `error C4668: '__has_feature' is not defined as a preprocessor macro`

这是工具链不匹配导致的典型错误。Unreal Engine 4.27 的发布时间早于你当前安装的 MSVC 版本，而 UBT 选用的工具集其预处理器行为与引擎头文件预期的不符。

**解决方法：** 安装受支持的 MSVC 版本，卸载其他版本，并在 `BuildConfiguration.xml` 中指定（锁定）SDK 版本。请参阅[工具链要求（Toolchain Requirements）](Toolchain-Requirements.md)。进行任何更改后，请重新生成项目文件。


### UnrealBuildTool 选择了错误的编译器或 SDK

UBT 通过扫描已安装的工具集进行自动检测，通常会优先选择最新版本。如果系统中安装了多个版本，自动选定的往往并非你想要的版本。

**解决方法：** 在 `%APPDATA%\Unreal Engine\UnrealBuildTool\BuildConfiguration.xml` 中显式指定（锁定）两者：

```xml
<?xml version="1.0" encoding="utf-8" ?>
<Configuration xmlns="https://www.unrealengine.com/BuildConfiguration">
    <WindowsPlatform>
        <Compiler>VisualStudio2022</Compiler>
        <WindowsSdkVersion>10.0.26100.0</WindowsSdkVersion>
    </WindowsPlatform>
</Configuration>
```

随后再次运行 `GenerateProjectFiles.bat`。请留意链接（linking）步骤之前的构建日志，确认 UBT 报告的工具链正是您预期的版本。


### `ViteSetup.bat` 停止运行，并显示一行 `[FAIL]` 提示。

该辅助脚本严格执行其工具链要求，且不提供绕过机制。报错信息会指出缺失的组件。请注意，该脚本目前指定使用 Visual Studio 2022、MSVC 14.44 以及 Windows SDK 10.0.26100.7705 或更高版本；如果您使用的是更新的 VS 2026 / MSVC 14.50 工具链，请选择以下任一方案：按照[从源码构建](Build-From-Source.md)部分的说明手动构建，或者更新脚本顶部的 `REQUIRED_*` 变量。


### C# 工具项目构建失败（SwarmAgent、NetworkProfiler、UnrealControls）

这些项目针对的是 .NET Framework 4.5，而现代 Visual Studio 安装程序已不再附带该版本的目标包（targeting pack）。

**解决方法：** 按照[工具链要求](Toolchain-Requirements.md)部分的说明进行安装。如果 v4.5 文件夹中仅包含 XML 文件，则该文件夹属于运行时存根（runtime stub）而非目标包——请检查是否存在 `v4.5\RedistList\FrameworkList.xml` 文件。


## 依赖项与设置错误

###  `Setup.bat` 无法下载依赖项

在原版 Unreal Engine 4.27 中，出现此问题是因为 GitDeps 的端点地址已变更。**Vite 版本已包含修复方案**，因此如果您在此分支（fork）上遇到下载失败，原因通常在于环境因素：例如代理设置、防火墙拦截，或是之前的运行过程意外中断导致缓存损坏。

解决方法：删除仓库根目录下的 `UE4_Source_Cache` 文件夹，然后重新运行 `Setup.bat`。`ViteSetup.bat` 会在设置成功后自动清理该缓存。


### `Setup.bat` 提示覆盖本地更改

请选择“N”（否）。如果选择“Y”（是），会将该分支特有的文件还原为上游（upstream）状态，从而导致构建失败。

### Win64 构建中缺少 `astcenc.exe` 或其他第三方工具

您在安装过程中排除了 Win32 文件夹。Win64 版本的构建依赖于位于 Win32 目录下的工具，其中包括 `ARM\Win32\astcenc.exe`。

**解决方法：** 重新运行 `Setup.bat`，且不要带任何 `-exclude=Win32` 参数。正因如此，`ViteSetup.bat` 的预设配置从不排除 Win32。


## 编译与链接错误

### 编译期间内存不足或堆耗尽

Unreal 的 Unity 构建系统会编译极大的翻译单元，并采用激进的并行处理策略。在核心数较多但内存容量有限的机器上，默认的并行度可能会导致内存耗尽。

**解决方法：** 在 `BuildConfiguration.xml` 中限制并行操作：

```xml
<?xml version="1.0" encoding="utf-8" ?>
<Configuration xmlns="https://www.unrealengine.com/BuildConfiguration">
    <ParallelExecutor>
        <MaxProcessorCount>12</MaxProcessorCount>
    </ParallelExecutor>
</Configuration>
```

### 编辑器构建成功，但无法启动

请确认以下三个目标（Target）均已构建：`UE4Editor`、`ShaderCompileWorker` 和 `UnrealLightmass`。如果仅构建编辑器目标，生成的二进制文件虽然能启动，但一旦需要编译着色器（shader）就会失败。

### 拉取引擎更改后，着色器或派生数据变得陈旧

渲染方面的更改会导致着色器缓存失效。如果在执行拉取（pull）操作后遇到着色器编译错误、材质缺失或画面显示异常，请清除引擎层级的缓存：

```batch
WipeShaderCache.bat
```

此操作会删除 `Engine\DerivedDataCache`、`Engine\Intermediate\Shaders` 和 `Engine\Saved\ShaderDebugInfo`。引擎会在下次启动时重新构建这些文件，这一过程需要一些时间。请务必先关闭编辑器及所有 `ShaderCompileWorker` 进程，否则删除操作将会失败。请参阅[缓存管理](../Tools/Cache-Management.md)相关内容。


## 构建成功后的运行时问题

### 新项目的运行速度远低于预期

Vite 默认启用了光线追踪功能（包括阴影、反射、半透明效果和环境光遮蔽）。这样设计是为了让用户能够发现这些功能，但也意味着空项目比标准的 4.27 版本项目更“重”（资源开销更大）。

**解决方法：** 禁用不需要的效果。请参阅[光线追踪](../Rendering/Ray-Tracing.md)部分以了解相关的控制台变量。


### 游戏逻辑（Gameplay）表现与标准 4.27 版本不同

为了提升性能，Vite 修改了引擎的若干默认设置；其中一些设置不仅影响帧时间，还会改变功能行为——例如，Primitive Component（图元组件）上的重叠事件（overlap events）默认处于禁用状态。


**解决方法：** 请完整阅读[引擎默认设置变更](../Performance/Engine-Defaults.md)文档。文中列出了每一项变更，并附有指向对应提交记录的链接。


## 获取帮助

如果上述情况均不适用，通过[社区 Discord](https://discord.gg/n9zQrYFhMb) 上的 `#support` 频道寻求帮助是最快的途径。请提供以下信息：Visual Studio 版本、MSVC 工具集版本、Windows SDK 版本、正在构建的分支，以及日志中的**第一个**错误（而非最后一个）——因为 Unreal 的构建输出往往呈连锁反应，最后的错误通常并非问题的关键所在。


## 另请参阅

- [工具链要求](Toolchain-Requirements.md)
- [从源码构建](Build-From-Source.md)
- [缓存管理](../Tools/Cache-Management.md)
- [引擎默认设置变更](../Performance/Engine-Defaults.md)
