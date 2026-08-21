# 构建故障排除

<tldr>
<p>
Most build failures on Vite are toolchain failures wearing a disguise. Before investigating anything else,
confirm you have exactly one MSVC toolset and one Windows SDK installed, matching
<a href="Toolchain-Requirements.md">Toolchain Requirements</a>.
</p>
</tldr>

This page collects the errors that come up most often, what they actually mean, and how to clear them.

## Toolchain errors

### `error C4668: '__has_feature' is not defined as a preprocessor macro`

This is the signature error of a toolchain mismatch. Unreal Engine 4.27 predates the MSVC version you have
installed, and UBT has selected a toolset whose preprocessor behaviour the engine's headers do not expect.

**Fix.** Install a supported MSVC version, remove the others, and pin the SDK in `BuildConfiguration.xml`.
See [Toolchain Requirements](Toolchain-Requirements.md). Regenerate project files after changing anything.

### UnrealBuildTool picks the wrong compiler or SDK

UBT autodetects by scanning installed toolsets and generally prefers the newest. When you have several
installed, that is often not the one you want.

**Fix.** Pin both explicitly in `%APPDATA%\Unreal Engine\UnrealBuildTool\BuildConfiguration.xml`:

```xml
<?xml version="1.0" encoding="utf-8" ?>
<Configuration xmlns="https://www.unrealengine.com/BuildConfiguration">
    <WindowsPlatform>
        <Compiler>VisualStudio2022</Compiler>
        <WindowsSdkVersion>10.0.26100.0</WindowsSdkVersion>
    </WindowsPlatform>
</Configuration>
```

Then run `GenerateProjectFiles.bat` again. Watch the build log just before linking to confirm the toolchain
UBT reports is the one you intended.

### `ViteSetup.bat` refuses to continue with a `[FAIL]` line

The assistant enforces its toolchain requirements deliberately and offers no bypass. The failing line names
the missing component. Note that the assistant currently pins Visual Studio 2022, MSVC 14.44 and Windows SDK
10.0.26100.7705 or higher; if you are on the newer VS 2026 / MSVC 14.50 toolchain, either build manually as
described in [Building from Source](Build-From-Source.md), or update the `REQUIRED_*` variables at the top
of the script.

### C# tool projects fail to build (SwarmAgent, NetworkProfiler, UnrealControls)

These target .NET Framework 4.5, whose targeting pack modern Visual Studio installers no longer ship.

**Fix.** Install it, as described in [Toolchain Requirements](Toolchain-Requirements.md). A `v4.5` folder
containing only XML files is a runtime stub, not a targeting pack &mdash; check for
`v4.5\RedistList\FrameworkList.xml`.

## Dependency and setup errors

### `Setup.bat` fails to download dependencies

In stock Unreal Engine 4.27 this happens because the GitDeps endpoints have moved. **Vite already ships the
fix**, so if you are seeing download failures on this fork, the cause is environmental: a proxy, a firewall,
or an interrupted earlier run leaving a corrupt cache.

**Fix.** Delete the `UE4_Source_Cache` folder in the repository root and run `Setup.bat` again.
`ViteSetup.bat` removes this cache automatically after a successful setup.

### `Setup.bat` asks to overwrite local changes

Answer `N`. Answering `Y` reverts fork-specific files to their upstream state and will break the build.

### Missing `astcenc.exe` or other third-party tools in a Win64 build

You excluded the Win32 folders during setup. Win64 installed builds depend on tools that live under Win32,
including `ARM\Win32\astcenc.exe`.

**Fix.** Re-run `Setup.bat` without any `-exclude=Win32` argument. The `ViteSetup.bat` presets never exclude
Win32 for this reason.

## Compile and link errors

### Out of memory or heap exhaustion during compilation

Unreal's unity build system compiles very large translation units and parallelises aggressively. On machines
with high core counts and modest RAM, the default parallelism can exhaust memory.

**Fix.** Limit parallel actions in `BuildConfiguration.xml`:

```xml
<?xml version="1.0" encoding="utf-8" ?>
<Configuration xmlns="https://www.unrealengine.com/BuildConfiguration">
    <ParallelExecutor>
        <MaxProcessorCount>12</MaxProcessorCount>
    </ParallelExecutor>
</Configuration>
```

### The editor builds but fails to launch

Confirm all three targets were built: `UE4Editor`, `ShaderCompileWorker` and `UnrealLightmass`. Building
only the editor target produces a binary that starts and then fails as soon as it needs to compile a shader.

### Stale shaders or derived data after pulling engine changes

Rendering changes invalidate the shader cache. If you see shader compilation errors, missing materials or
visual corruption after a pull, wipe the engine-level caches:

```batch
WipeShaderCache.bat
```

This removes `Engine\DerivedDataCache`, `Engine\Intermediate\Shaders` and `Engine\Saved\ShaderDebugInfo`.
The engine rebuilds them on the next launch, which will take a while. Close the editor and any
`ShaderCompileWorker` processes first, or the deletion will fail. See
[Cache Management](../Tools/Cache-Management.md).

## Runtime issues after a successful build

### A new project runs much slower than expected

Ray tracing is enabled by default in Vite, including shadows, reflections, translucency and ambient
occlusion. This is intentional so that the features are discoverable, but it means an empty project is
heavier than a stock 4.27 one.

**Fix.** Disable the effects you do not need. See [Ray Tracing](../Rendering/Ray-Tracing.md) for the console variables.

### Gameplay behaves differently from stock 4.27

Vite changes several engine defaults for performance, and some of them affect behaviour rather than just
frame time &mdash; overlap events are disabled by default on primitive components, for instance.

**Fix.** Read [Engine Default Changes](../Performance/Engine-Defaults.md) in full. Every changed default is listed there
with a link to the commit that changed it.

## Getting help

If none of the above applies, the `#support` channels on the
[community Discord](https://discord.gg/n9zQrYFhMb) are the fastest route. Include your Visual Studio
version, MSVC toolset version, Windows SDK version, the branch you are building, and the first error in the
log rather than the last &mdash; Unreal's build output cascades, and the final error is rarely the useful
one.

## See also

- [Toolchain Requirements](Toolchain-Requirements.md)
- [Building from Source](Build-From-Source.md)
- [Cache Management](../Tools/Cache-Management.md)
- [Engine Default Changes](../Performance/Engine-Defaults.md)
