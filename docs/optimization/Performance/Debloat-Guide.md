# 精简指南


`devops\` 目录下的三个脚本，由一个 `config.txt` 文件驱动，用于移除您不需要的引擎内容。所有操作默认都以**试运行**的方式进行，并**将内容移动到其他位置而非删除**，因此即使出错也可以恢复。

完整的虚幻引擎 4.27 源代码树支持 Epic 提供的所有平台、工作流程和模板。一个仅限 Win64 平台的项目仅占用其中一小部分资源。精简代码可以释放磁盘空间并缩短构建时间。

原始 Vite 精简脚本由 Bikouz 编写。

## 该套件

| 脚本 | 用途 |
|---|---|
| `ueVite-debloat-SetupSlim.bat` | 仅针对 Win64 平台运行引擎依赖项设置，因此不会下载不必要的平台依赖项。 |
| `ueVite-debloat-StripExecute.bat` | 移除平台二进制文件、非 Windows 工具、模板、示例内容以及（可选）插件。 |
| `ueVite-debloat-StripDebugSymbols.bat` | 根据 `ExcludedPdbs.txt` 移除调试符号。 |

这三个脚本都位于引擎根目录下的 `devops\` 目录中，并读取 `devops\config.txt` 文件。您可以将备用配置路径作为第一个参数传递。

## 安全模型

这些脚本刻意采用保守策略，因此在运行它们之前，务必了解其工作原理。

**默认设置为“试运行”。** `DRYRUN=1` 会打印每个操作，但不会实际修改任何文件。请在修改之前查看输出。

**默认设置为“移动”，而非“删除”。** `MODE=move` 会将目标文件移动到 `MOVE_DIR` 目录，并保留其相对布局，以便可以通过复制的方式恢复。`MODE=delete` 则为永久性删除。

**移动目标不能位于引擎目录树内。** 脚本会拒绝，因为位于目录树内的移动目标会被后续的迭代捕获。

**脚本拒绝使用根目录。** 它会同时检查引擎目录和移动目标。

**脚本会验证引擎目录。** 在执行任何操作之前，脚本会确认 `Engine\Binaries\` 目录是否存在。

## 配置

`devops\config.txt` 使用 `KEY=VALUE` 行。用 `#` 注释掉一行可以**保留**其目标；取消注释可以**启用**可选目标。这种反转值得注意：取消注释 **STRIP** 行意味着要删除该行。  

### 核心设置

```
MODE=move
MOVE_DIR=..\ViteDebloat_Moved
DRYRUN=1
```

`MOVE_DIR` 指向引擎根目录，因此默认值是引擎的同级文件夹。

### 平台

Win64 始终保留。取消注释平台以在 `SetupSlim` 期间下载并保留其依赖项：

```
#KEEP_PLATFORM=Android
#KEEP_PLATFORM=IOS
#KEEP_PLATFORM=Linux
#KEEP_PLATFORM=Mac
#KEEP_PLATFORM=HTML5
```

请在运行安装程序**之前**执行此操作。不下载依赖项比下载后再删除它更划算。

### 模板

```
STRIP_OTHER_TEMPLATES=1
KEEP_TEMPLATE=TP_ThirdPerson
KEEP_TEMPLATE=TP_ThirdPersonBP
KEEP_TEMPLATE=TemplateResources
```

模板目录下所有未被 `KEEP_TEMPLATE` 行指定的文件夹都会被移除。`TemplateResources` 文件夹由所有模板共享，如果您保留任何模板，则必须保留该文件夹。

### 移除目标

默认目标列表，按类别分组：

**Win64 不需要的平台二进制文件**

```
STRIP=Engine\Binaries\Win64\Android
STRIP=Engine\Binaries\Win64\IOS
STRIP=Engine\Binaries\Win64\Lumin
STRIP=Engine\Binaries\DotNET\IOS
```

**非 Windows 部署和开发工具**

```
STRIP=Engine\Extras\Android
STRIP=Engine\Extras\iTunes
STRIP=Engine\Extras\Xcode
STRIP=Engine\Extras\Instruments
STRIP=Engine\Extras\GDBPrinters
STRIP=Engine\Extras\LLDBDataFormatters
STRIP=Engine\Extras\Maya_AnimationRiggingTools
STRIP=Engine\Extras\MayaVelocityGridExporter
```

由于 3ds Max 是一款 Windows DCC 工具，因此默认情况下会保留 3ds Max 脚本。

**UnrealFileServer**

```
STRIP=Engine\Binaries\Win64\UnrealFileServer.exe
#STRIP=Engine\Source\Programs\UnrealFileServer
```

UnrealFileServer 为远程设备提供已处理和暂存的文件，并支持网络即时处理 (Cook-on-the-Fly)。如果您仅针对本地 Win64 进行开发和打包，不部署到远程设备，不使用网络即时处理，也不通过文件服务器工作流程面向主机或移动设备，则可以安全地移除它。

默认情况下，目标平台为预编译的二进制文件；源文件夹将被保留，以便稍后重新编译该工具。

**引擎内容**

```
STRIP=Samples\StarterContent
STRIP=Samples\MobileStarterContent
STRIP=FeaturePacks\StarterContent.upack
#STRIP=Samples\NGXTest
#STRIP=Samples\PixelStreaming
#STRIP=Samples\RTXGI_Test
```

根据精简策略，初始内容已被移除。默认情况下，分支特定的测试样本会被保留——特别是 `RTXGI_Test`，它在验证 [DDGI](DDGI-Dynamic.md) 时非常有用。

树中不存在的条目会被跳过并显示一条通知，这不会造成任何影响。

## 插件通过

默认情况下禁用，因为每个项目的插件需求各不相同：

```
#PLUGIN_LIST=ExcludedPlugins.txt
```

`ExcludedPlugins.txt` 文件列出了大约 200 个引擎插件路径，涵盖移动和 XR 平台、除您当前使用的源代码控制提供商之外的其他源代码控制提供商、Chaos 插件（由于 Vite 使用 [PhysX](PhysX.md)，因此无需列出）、企业和虚拟生产工具，以及大量实验性插件。


系统首先会在脚本目录下查找该文件，然后在引擎根目录下查找。


!!! 警告

    启用此渲染通道前，请阅读 `ExcludedPlugins.txt` 文件。该文件包含您的项目可能需要的插件条目，例如 `Engine/Plugins/Runtime/GameplayAbilities/`、`Engine/Plugins/Runtime/ApexDestruction/`、`Engine/Plugins/Runtime/HairStrands/`、`Engine/Plugins/Runtime/Nvidia/`（包含 [DLSS 和 Streamline 插件](Upscalers.md)）以及 `Engine/Plugins/Runtime/PhysXVehicles/`。

    运行此渲染通道前，请注释掉您使用的所有插件对应的行。

    

## 运行

### 精简引擎安装

1. 打开 `devops\config.txt` 文件，检查所有未注释的 `STRIP` 行。注释掉所有不需要的行。

2. 确认 `DRYRUN=1` 和 `MODE=move`。

3. 运行 `ueVite-debloat-StripExecute.bat` 并阅读完整输出。很多人会跳过这一步，然后后悔。

4. 设置 `DRYRUN=0` 并再次运行。

5. 构建引擎并打开编辑器。确认您的项目仍然可以加载，并且所需的插件都已安装。

6. 确认无误后，删除 `MOVE_DIR` 以真正回收空间。在此之前，文件仍然保留在磁盘上。

对于全新克隆，请运行 `ueVite-debloat-SetupSlim.bat` 而不是 `Setup.bat`，这样就不会一开始就下载不需要的平台依赖项。


## 调试符号

`ueVite-debloat-StripDebugSymbols.bat` 会根据 `ExcludedPdbs.txt` 文件中的设置移除 PDB 文件。调试符号会占用源代码构建磁盘空间的很大一部分。

保留所有可能需要调试的符号。移除那些你从不单步执行的引擎模块的符号可以节省大量空间；移除那些你执行的模块的符号可以将可读的调用堆栈转换为十六进制格式。

## 恢复

在移动（`move`）模式下，`MOVE_DIR` 会镜像引擎树的相对布局。将内容复制回引擎根目录即可恢复。

如果您使用的是删除（`delete`）模式，则恢复意味着重新克隆或重新运行 `Setup.bat`。

## 此操作不会做什么

精简插件可以减少磁盘占用空间，并通过插件阶段缩短构建时间。但它**不会**提高运行时性能——即使插件存在但已禁用，运行时也不会消耗任何资源。

有关运行时性能，请参阅[性能分析](Profiling.md)和[引擎默认更改](Engine-Defaults.md)。关于构建时间，移除添加的 Vite 插件可以在 15 分钟的完整构建过程中节省大约 3 分钟；请参阅[着色器编译和PSO](Shader-Compilation-And-PSO.md)。

## 另请参阅

- [从源代码构建](Build-From-Source.md)
- [引擎默认设置更改](Engine-Defaults.md)
- [捆绑插件](Bundled-Plugins.md)
- [缓存管理](Cache-Management.md)
