# ViteSetup Assistant

位于引擎根目录的 `ViteSetup.bat` 脚本提供了一个包含九个步骤的引导式设置流程，涵盖了从环境检查到启动编辑器的全过程。此外，`ViteSetup.bat` 的菜单也允许用户单独执行其中的各项操作。

之所以提供这一辅助工具，是因为构建 Unreal 源码版本涉及多个步骤，且每个步骤都可能因操作不当而导致失败；若不使用该工具，这些失败往往会引发难以排查的错误信息。该工具会预先检查环境，因此如果缺少某个工具链组件，系统会在第一步就给出明确提示，从而避免等到第五步才因链接器错误而导致构建失败。


## 引导模式

直接运行 `ViteSetup.bat`（不带任何参数）。

| 步骤 | 操作 |
|---|---|
| 1 | 检查环境并指定工具链 |
| 2 | 配置依赖项（包含平台选择） |
| 3 | 生成 Visual Studio 项目文件 |
| 4 | 选择源码构建或二进制构建 |
| 5 | 构建选定目标 |
| 6 | 注册选定的构建版本 |
| 7 | 在桌面上创建 `UEViteFork.lnk` 快捷方式  |
| 8 | 可选：精简引擎（移除冗余组件） |
| 9 | 完成；源码构建版本将启动编辑器，二进制构建版本则不会 |

每个步骤执行后都会暂停，以便您在继续之前查看输出结果。如果遇到失败，助手会停止运行并报告错误代码，而不会继续执行后续步骤。


## 步骤 1 — 环境检查

该助手强制要求使用特定的工具链，若缺少该工具链，则无法继续执行：


| 要求 | 强制值 |
|---|---|
| Visual Studio | 2022 |
| MSVC | 14.44 |
| Windows SDK | 10.0.26100，子版本 .7705 或更高，主版本固定为 26100 |
| .NET Framework 引用程序集 | v4.6.2 和 v4.5 |

**警告：** 强制使用的工具链与 Vite 当前开发所用的工具链（即 Visual Studio 2026 配合 MSVC 14.50 和 Windows SDK 10.0.26100）不一致，而辅助检查程序尚未针对此变化进行更新。
如果您使用的是较新的工具链，该检查将会失败。请改用[手动构建方式](../GettingStarted/Build-From-Source.md)或直接选择菜单中的各项操作，而不要使用引导式流程。详情请参阅[工具链要求](../GettingStarted/Toolchain-Requirements.md)。


.NET Framework 4.5 引用程序集（reference assemblies）是用户最常缺失的组件，因为较新的 Visual Studio 安装程序已不再默认包含它们。详情请参阅[构建故障排查](../GettingStarted/Build-Troubleshooting.md)。


## 步骤 2 — 依赖项设置

该向导不直接运行包含所有内容的 `Setup.bat`，而是提供了多种设置配置：


| 配置概览 | 功能说明 |
|---|---|
| 分步向导 | 逐项选择平台及可选内容，随后进行确认 |
| 推荐配置 (Win64) | 仅限 Win64。不包含非 Windows 平台、示例、模板、功能包、文档、XR 及主机平台相关内容。 |
| Win64 超精简版 | 基于推荐配置，但仅包含最少量的可选内容 |
| Win64 + 模板/功能包 | 保留项目模板及入门资源 |
| Win64 + Android | 增加 Android 相关依赖 |
| Win64 + Linux | 增加 Linux 及交叉编译相关依赖 |
| 完整安装 | 运行 `Setup.bat` 时不使用 `-exclude` 参数 |

分步安装向导提供了针对 Android、iOS/tvOS、Mac、Linux、WinRT、HTML5 和游戏主机平台的独立选项开关。Win32 支持文件会默认包含在内，因为 Win64 运行需要这些文件。

**对于大多数用户而言，推荐选择 Win64 版本**。下载永远不会用到的依赖项会浪费带宽、磁盘空间和安装时间。如果日后需要某个平台，只需重新运行安装程序并勾选相应选项即可。


## 步骤 4 —— 源码或二进制文件

| 选项 | 结果 |
|---|---|
| 源码构建 | 在源码树中构建 `UE4Editor`、`ShaderCompileWorker` 和 `UnrealLightmass`（Win64 Development 配置） |
| 二进制安装版本构建 | 运行增量 BuildGraph，生成 `LocalBuilds\Engine\Windows` 及 `UE_ViteFork.7z` |

如果您正在开发引擎本身，或者需要修改[编译时开关](../Performance/Compile-Time-Switches.md)，请选择源码版本。如果您需要一个可分发的引擎版本，以便提供给无需自行编译的用户，请选择“二进制”版本。详情请参阅[已安装构建](./Installed-Builds.md)版本相关内容。


## 步骤 6 和 7 —— 注册与创建快捷方式

注册操作会将引擎路径写入注册表项 `HKCU\Software\Epic Games\Unreal Engine\Builds` 下，键名为 `UEViteFork`；这使得该构建版本能够在 `.uproject` 文件的引擎关联选项中被选中。其引擎关联标识符为 `UE_ViteFork`。

创建快捷方式的步骤会生成一个指向 `UE4Editor.exe` 的快捷方式文件 `UEViteFork.lnk`。

这两个步骤对应的操作也可作为独立脚本运行，分别为 `LocalBuilds\RegistryAdd.bat` 和 `LocalBuilds\MakeShortcut.bat`。

## 步骤 8 — 精简系统

可选择运行精简助手，提供以下功能：

| 选项 | 行为 |
|---|---|
| 移至恢复文件夹 | 稍后可恢复。目标位置为引擎文件夹旁的 `ViteDebloat_Moved` 文件夹。推荐。  |
| 永久删除 | 占用空间最小，不可撤销 |

完整详情请参阅[精简指南](../Performance/Debloat-Guide.md)。


## 菜单模式

`ViteSetup.bat` 的菜单模式会跳过引导式流程，直接列出各项操作供选择：

| 选项 | 操作 |
|---|---|
| 1 | 检查环境 |
| 2 | 设置依赖项（向导） |
| 3 | 生成 Visual Studio 文件 |
| 4 | 编译 Unreal Engine 源码 |
| 5 | 设置并生成 Visual Studio 文件 |
| 6 | 打开 `UE4.sln` |
| 7 | 显示设置配置 |
| 8 | 构建已安装的二进制文件 |
| 9 | 精简引擎（向导） |
| 0 | 退出 |

日常工作中，您需要使用的是菜单模式。在添加模块后重新生成项目文件，或者在执行 pull 操作后进行重新构建，并不需要走完完整的九步流程。


## 关键路径

| 变量 | 路径 |
|---|---|
| 编辑器可执行文件 | `Engine\Binaries\Win64\UE4Editor.exe` |
| 已安装构建暂存 | `LocalBuilds\Engine\Windows` |
| 已安装构建归档 | `LocalBuilds\Engine\UE_ViteFork.7z` |
| 源码缓存 | `UE4_Source_Cache` |
| 引擎关联 | `UE_ViteFork` |

## 另请参见

- [从源码构建](../GettingStarted/Build-From-Source.md)
- [工具链要求](../GettingStarted/Toolchain-Requirements.md)
- [已安装构建](./Installed-Builds.md)
- [构建故障排查](../GettingStarted/Build-Troubleshooting.md)
- [精简指南](../Performance/Debloat-Guide.md)
