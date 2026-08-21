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

### Removing conflicting toolsets

<procedure title="Remove conflicting MSVC versions" id="remove-conflicting-msvc">
    <step>Open the Visual Studio Installer.</step>
    <step>Find your Visual Studio installation and click <b>Modify</b>.</step>
    <step>Switch to the <b>Individual components</b> tab.</step>
    <step>Uncheck every MSVC v14.x x64/x86 build tools entry except the one your chosen toolchain requires.</step>
    <step>Uncheck every Windows SDK except the one your chosen toolchain requires.</step>
    <step>Click <b>Modify</b> and let the installer finish before generating project files again.</step>
</procedure>

Having more than one Windows SDK installed is not fatal, but it is a frequent source of link-time surprises.
If you can get to exactly one, do.

### The .NET Framework 4.5 targeting pack

The Visual Studio 2022 and 2026 installers no longer ship the 4.5 targeting pack, but the engine's C# tools
still need it. A folder at
`C:\Program Files (x86)\Reference Assemblies\Microsoft\Framework\.NETFramework\v4.5` that contains only XML
files is a stub left behind by the .NET runtime, not a real targeting pack. Check for
`v4.5\RedistList\FrameworkList.xml` to tell the difference.

`ViteSetup.bat` offers to install it automatically from the official Microsoft NuGet package. To do it
by hand:

<procedure title="Install the .NET Framework 4.5 targeting pack manually" id="install-net45">
    <step>Download <a href="https://www.nuget.org/api/v2/package/Microsoft.NETFramework.ReferenceAssemblies.net45/1.0.3">Microsoft.NETFramework.ReferenceAssemblies.net45 1.0.3</a>.</step>
    <step>Open the <code>.nupkg</code> file as a ZIP archive.</step>
    <step>Copy <code>build\.NETFramework\v4.5</code> into <code>C:\Program Files (x86)\Reference Assemblies\Microsoft\Framework\.NETFramework\v4.5</code>. This needs administrator rights.</step>
    <step>Confirm that <code>v4.5\RedistList\FrameworkList.xml</code> now exists.</step>
</procedure>

## Pinning the SDK through BuildConfiguration.xml

Unreal Build Tool reads a per-user configuration file that overrides its toolchain autodetection. This is
the reliable way to force a specific Compiler and Windows SDK when several are present.

Edit (VS26):

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


> Changes to `BuildConfiguration.xml` take effect on the next build, but you should regenerate project
> files after editing it so that the IDE and UBT agree.
>
{style="note"}

## Verifying your toolchain

Before you start a long build, confirm what is actually installed. `ViteSetup.bat` performs this check for
you and prints an `[OK]` or `[FAIL]` line for each requirement. To check by hand, look for:

- `C:\Program Files\Microsoft Visual Studio\<year>\<edition>\VC\Tools\MSVC\<version>\bin\Hostx64\x64\cl.exe`
  &mdash; the MSVC toolsets you have installed are the folder names under `VC\Tools\MSVC`.
- `C:\Program Files (x86)\Windows Kits\10\Include\<version>` &mdash; the Windows SDKs you have installed.

A second useful checkpoint: watch the build output just before the linking phase begins and confirm which
toolchain UBT reports using. Catching a mismatch there is much cheaper than catching it at link time.

## See also

- [Building from Source](Build-From-Source.md)
- [Build Troubleshooting](Build-Troubleshooting.md)
- [ViteSetup Assistant](../Tools/ViteSetup.md)
- [System Requirements](System-Requirements.md)
