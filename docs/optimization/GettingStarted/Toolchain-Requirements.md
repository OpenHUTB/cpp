# 工具链要求

当前 Vite 工具链：**Visual Studio 2026**，**MSVC 版本 14.50**，**Windows SDK 版本 10.0.26100**。请卸载或取消选择所有其他 MSVC 工具集。工具链不匹配不会产生明确的错误信息，而是会显示 `C4668: '__has_feature' is not defined` 等类似错误。



从源代码编译 Unreal Engine Vite 需要一组特定的构建工具。Unreal 构建工具会扫描已安装的工具链来选择合适的工具链，因此同时安装多个 MSVC 版本是导致构建失败的最常见原因。请仅安装此页面列出的工具链，并移除其余工具链。


## 支持的工具链

Vite 已针对以下工具链组合进行验证。第一行是当前推荐的工具链组合。

| 工具链                    | MSVC | Windows SDK | 状态                                                                                              |
|------------------------------|---|---|-----------------------------------------------------------------------------------------------------|
| Visual Studio 2026 (latest)  | 14.50 | 10.0.26100 | **当前。** 最高编译器性能和工具链 Vite 开发目标。               |
| Visual Studio 2022 (latest)  | 14.44 | 10.0.26100 | 支持。与 Epic 的 UE 4.27 Plus 分支验证的最新工具链更新相匹配。                |
| Visual Studio 2019 / 2022    | 14.29 | 10.0.18362 | 最大稳定性。最旧的支持组合；如果您的项目需要原始工具链，请使用。 |

!!! 警告

  `ViteSetup.bat` 助手目前强制要求使用 Visual Studio 2022、MSVC 14.44 和 Windows SDK 10.0.26100 子版本 7705 或更高版本，否则将无法继续运行。如果您使用的是 VS 2026 / MSVC 14.50 工具链，请手动运行构建步骤，而不是通过助手运行，或者更新脚本顶部的 `REQUIRED_*` 变量。请参阅 [ViteSetup 助手](../Tools/ViteSetup.md)。


## 所需组件

通过 Visual Studio 安装程序在**独立组件**下安装这些组件。

- 使用 C++ 工作负载的桌面开发
- MSVC v14.50 – VS 2026 C++ x64/x86 构建工具（或与上面您选择的行匹配的版本）
- Windows 11 SDK (10.0.26100)
- .NET Framework 4.6.2 目标包 — UnrealBuildTool 和 AutomationTool 所需
- .NET Framework 4.5 目标包 — SwarmAgent、SwarmCoordinator、NetworkProfiler 和 UnrealControls 所需

要跳过手动组件选择，请通过 Visual Studio 26 安装程序的 **导入配置**选项下载并导入 [Vite VSConfig](https://drive.google.com/file/d/1NwpPUiM_7yVI_kjhW94kYxvVP42ViV3Q/view?usp=sharing) 文件。

### 移除冲突的工具集

**移除冲突的 MSVC 版本**

1. 打开 Visual Studio 安装程序。

2. 找到您的 Visual Studio 安装目录，然后单击“修改(Modify)”。

3. 切换到“单个组件(Individual components)”选项卡。

4. 取消选中除您选择的工具链所需的 MSVC v14.x x64/x86 生成工具条目之外的所有条目。

5. 取消选中除您选择的工具链所需的 Windows SDK 条目之外的所有条目。

6. 单击“修改(Modify)”，等待安装程序完成安装后再重新生成项目文件。

安装多个 Windows SDK 并非致命问题，但经常会导致链接时出现意外情况。如果可以，请尽量只安装一个。


### .NET Framework 4.5 目标包

Visual Studio 2022 和 2026 安装程序不再包含 4.5 目标包，但引擎的 C# 工具仍然需要它。位于 `C:\Program Files (x86)\Reference Assemblies\Microsoft\Framework\.NETFramework\v4.5` 的文件夹仅包含 XML 文件，它是 .NET 运行时留下的存根，并非真正的目标包。检查 `v4.5\RedistList\FrameworkList.xml` 文件即可区分。

`ViteSetup.bat` 提供从官方 Microsoft NuGet 包自动安装目标包的功能。要手动安装：

**手动安装 .NET Framework 4.5 目标包**

1. 下载 Microsoft.NETFramework.ReferenceAssemblies.net45 1.0.3。

2. 将 `.nupkg` 文件以 ZIP 压缩包的形式打开。

3. 将 `build\.NETFramework\v4.5` 复制到 `C:\Program Files (x86)\Reference Assemblies\Microsoft\Framework\.NETFramework\v4.5`。此操作需要管理员权限。

4. 确认 v4.5\RedistList\FrameworkList.xml 文件存在。




## 通过 BuildConfiguration.xml 固定 SDK

虚幻引擎构建工具会读取一个用户配置文件，该文件会覆盖其工具链自动检测。当存在多个编译器和 Windows SDK 时，这是强制使用特定编译器和 SDK 的可靠方法。

编辑（VS26）：

```
%APPDATA%\Unreal Engine\UnrealBuildTool\BuildConfiguration.xml
```

```xml
<?xml version="1.0" encoding="utf-8"?>
<Configuration xmlns="https://www.unrealengine.com/BuildConfiguration">
  <WindowsPlatform>
    <CompilerVersion>14.50</CompilerVersion> <!--Vite has been tested with MSVC 14.50 for about 9 months to this date -->
    <!--<CompilerVersion>14.44</CompilerVersion> VS22 Option: Latest Epic's 4.27 compliance -->
    <!--<CompilerVersion>14.29</CompilerVersion> Original 4.27 Toolchain VS 2019 + W10 SDK 10.0.18362 / Clang 11.0.0 -->
    <!--<WindowsSdkVersion>10.0.18362.0</WindowsSdkVersion> -->
    <!-- <WindowsSdkVersion>10.0.22621.0</WindowsSdkVersion> -->
    <!-- <WindowsSdkVersion>10.0.26100.0</WindowsSdkVersion> Enable if specification would be needed -->
    <!-- <Compiler>VisualStudio2022</Compiler>  -->
  </WindowsPlatform>
</Configuration>
```

!!! 警告
    对 `BuildConfiguration.xml` 的更改将在下次构建时生效，但编辑后应重新生成项目文件，以便 IDE 和 UBT 保持一致。


## 验证您的工具链

在开始长时间构建之前，请确认实际安装的组件。`ViteSetup.bat` 会自动执行此检查，并针对每个要求打印 `[OK]` 或 `[FAIL]` 行。要手动检查，请查找：

- `C:\Program Files\Microsoft Visual Studio\<year>\<edition>\VC\Tools\MSVC\<version>\bin\Hostx64\x64\cl.exe` — 您已安装的 MSVC 工具集位于 `VC\Tools\MSVC` 下的文件夹名称中。

- `C:\Program Files (x86)\Windows Kits\10\Include\<version>` — 您已安装的 Windows SDK。

- 另一个有用的检查点：在链接阶段开始之前查看构建输出，并确认 UBT 报告使用的工具链。在此阶段发现不匹配比在链接时发现要高效得多。


## 另请参阅

- [从源代码构建](Build-From-Source.md)
- [构建故障排除](Build-Troubleshooting.md)
- [ViteSetup 助手](../Tools/ViteSetup.md)
- [系统要求](System-Requirements.md)
