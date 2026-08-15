# Toolchain Requirements

<tldr>
<p>
Current Vite toolchain: <b>Visual Studio 2026</b> with <b>MSVC 14.50</b> and <b>Windows SDK 10.0.26100</b>.
Uninstall or deselect every other MSVC toolset. A mismatched toolchain does not produce a clear error &mdash;
it produces <code>C4668: '__has_feature' is not defined</code> and similar noise.
</p>
</tldr>

Compiling Unreal Engine Vite from source requires a specific, pinned set of build tools. Unreal Build Tool
picks a toolchain by scanning what is installed, so having several MSVC versions side by side is the single
most common cause of build failure. Install exactly what this page lists and remove the rest.

## Supported toolchains

Vite is validated against the following combinations. The first row is the current recommendation.

| Toolchain                    | MSVC | Windows SDK | Status                                                                                              |
|------------------------------|---|---|-----------------------------------------------------------------------------------------------------|
| Visual Studio 2026 (latest)  | 14.50 | 10.0.26100 | **Current.** Highest compiler performance and the toolchain Vite development targets.               |
| Visual Studio 2022 (latest)  | 14.44 | 10.0.26100 | Supported. Matches the last toolchain update verified by Epic's UE 4.27 Plus branch.                |
| Visual Studio 2019 / 2022    | 14.29 | 10.0.18362 | Maximum stability. Oldest supported combination; use if your project needs the original toolchains. |

> The `ViteSetup.bat` assistant currently hard-pins Visual Studio 2022, MSVC 14.44 and Windows SDK
> 10.0.26100 subversion 7705 or higher, and refuses to continue otherwise. If you are on the VS 2026 /
> MSVC 14.50 toolchain, run the build steps manually rather than through the assistant, or update the
> `REQUIRED_*` variables at the top of the script. See [ViteSetup Assistant](ViteSetup.md).
>
{style="warning"}

## Required components

Install these through the Visual Studio Installer, under **Individual components**.

- Desktop development with C++ workload
- MSVC v14.50 &ndash; VS 2026 C++ x64/x86 build tools (or the version matching your chosen row above)
- Windows 11 SDK (10.0.26100)
- .NET Framework 4.6.2 targeting pack &mdash; required by UnrealBuildTool and AutomationTool
- .NET Framework 4.5 targeting pack &mdash; required by SwarmAgent, SwarmCoordinator, NetworkProfiler and
  UnrealControls

To skip the manual component selection, download and import the
[Vite VSConfig](https://drive.google.com/file/d/1NwpPUiM_7yVI_kjhW94kYxvVP42ViV3Q/view?usp=sharing) file
through the Visual Studio 26 Installer's **Import configuration** option.

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
- [ViteSetup Assistant](ViteSetup.md)
- [System Requirements](System-Requirements.md)
