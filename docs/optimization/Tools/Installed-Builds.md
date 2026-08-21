# 已安装的构建

<tldr>
<p>
An installed build is a precompiled, redistributable engine that works like an Epic Games Launcher install.
<code>RunUAT.bat</code> produces one; the <code>LocalBuilds\</code> scripts package and register it.
</p>
</tldr>

A source build requires a full toolchain, a long compile and a lot of disk. An installed build does not:
artists, designers and anyone who does not modify engine C++ can use one directly.

## Building

`RunUAT.bat` at the engine root drives the BuildGraph installed-build target:

```
Engine\Build\BatchFiles\RunUAT.bat BuildGraph
    -script="Engine\Build\InstalledEngineBuild.xml"
    -target="Make Installed Build Win64"
    -nosign
    -set:GameConfigurations=Development;Shipping
    -set:WithWin64=true
    -set:WithWin32=false
    -set:WithMac=false
    -set:WithAndroid=false
    -set:WithIOS=false
    -set:WithTVOS=false
    -set:WithLinux=false
    -set:WithLinuxAArch64=false
    -set:WithDDC=false
    -clean
```

The configuration is Win64-only with Development and Shipping game configurations, matching Vite's
[platform focus](../Platforms/Platforms.md). `WithDDC=false` skips derived data cache generation, which shortens the
build considerably at the cost of first-launch shader compilation for the end user.

You can also reach this through [ViteSetup](./ViteSetup.md) &mdash; step 4, binary option, or menu option 8.
The ViteSetup path is incremental; `RunUAT.bat` as shipped passes `-clean`.

<note>
This is a long build. Budget hours, not minutes, and considerable free disk space. Both the intermediate
and final outputs are large.
</note>

Output lands in `LocalBuilds\Engine\Windows`.

## Packaging

Four scripts in `LocalBuilds\` compress the staged build with 7-Zip. All expect
`LocalBuilds\Engine\Windows` to contain the staged build, and all look for 7-Zip at
`C:\Program Files\7-Zip\7z.exe` unless the `SEVEN_ZIP` environment variable points elsewhere.

| Script | Debug symbols | Plugins |
|---|---|---|
| `CompressBuild.bat` | Excluded per `ExcludedPdbs.txt` | Single archive |
| `CompressBuildSeparate.bat` | Excluded per `ExcludedPdbs.txt` | Split into a second archive |
| `CompressBuildSymbols.bat` | Excluded per `ExcludedPdbs.txt` | Single archive |
| `CompressBuildSymbolsSeparate.bat` | **Included** | Split into a second archive |

<note>
<code>CompressBuildSymbols.bat</code> is currently identical to <code>CompressBuild.bat</code> &mdash; both
exclude PDBs. If you need symbols in the archive, use
<code>CompressBuildSymbolsSeparate.bat</code>, which is the only script that does not pass
<code>-x@ExcludedPdbs.txt</code>.
</note>

All four exclude `FeaturePacks\`, `Samples\` and `Templates\` from the main archive.

Outputs:

| File | Contents |
|---|---|
| `LocalBuilds\Engine\UE_ViteFork.7z` | The engine |
| `LocalBuilds\Engine\ExcludedPlugins.7z` | Plugins, in the `Separate` variants only |

### The exclusion lists

`ExcludedPdbs.txt` lists roughly 1,100 PDB paths covering the engine modules and plugins. Debug symbols are
a very large fraction of a source build's size, so excluding them is the single biggest packaging saving.

`ExcludedPlugins.txt` is the same list used by the [debloat suite](../Performance/Debloat-Guide.md). In the `Separate`
variants it splits plugins into their own archive rather than removing them &mdash; users extract the base
engine and add the plugin archive only if they need it.

### Why split plugins

A team where most people need the base engine and only a few need the full plugin set can distribute a much
smaller primary download. The split is by archive, not by deletion, so nothing is lost.

## Installing on a target machine

<procedure title="Install a packaged Vite build" id="install-binary">
    <step>Extract <code>UE_ViteFork.7z</code> to its final location. Moving it later requires re-registering.</step>
    <step>If you were given <code>ExcludedPlugins.7z</code> and need those plugins, extract it over the same folder.</step>
    <step>
        Run <code>RegistryAdd.bat</code> from the extracted root. This writes the engine path to
        <code>HKCU\Software\Epic Games\Unreal Engine\Builds</code> under the name <code>UEViteFork</code>.
    </step>
    <step>Run <code>MakeShortcut.bat</code> to create <code>UEViteFork.lnk</code> pointing at the editor.</step>
    <step>
        Right-click a <code>.uproject</code>, choose <b>Switch Unreal Engine version</b>, and select
        <b>UEViteFork</b>.
    </step>
</procedure>

The packaged archive root contains `MakeShortcut.bat`, `RegistryAdd.bat` and `RegistryRemove.bat` alongside
`Engine\`, plus `FeaturePacks\`, `Templates\` and `Samples\` if they were included at build time.

## Moving or removing

Registration stores an absolute path, so moving the build breaks it.

<procedure title="Move an installed build" id="move-binary">
    <step>Run <code>RegistryRemove.bat</code> from the current location.</step>
    <step>Move the folder.</step>
    <step>Run <code>RegistryAdd.bat</code> from the new location.</step>
    <step>Run <code>MakeShortcut.bat</code> again; the old shortcut points at the old path.</step>
</procedure>

To remove entirely, run `RegistryRemove.bat`, delete the shortcut, and delete the folder.

## Running alongside a source build

Both can coexist, but they register under the same `UEViteFork` name, so only one is registered at a time.
Whichever ran `RegistryAdd.bat` most recently is the one projects will find.

If you regularly switch, note which is currently registered, or edit the registry key manually to give them
distinct names.

## Limitations

Installed builds cannot compile engine C++. Projects using them can still have their own C++ modules, which
compile against the installed engine's headers and libraries, but engine source changes require a source
build.

This includes [compile-time switches](../Performance/Compile-Time-Switches.md). If your project needs
`VITE_RT_PSO_DEBLOAT=0` for path tracing or RTXDI, that has to be baked into the installed build at the
time it is produced.

## See also

- [ViteSetup Assistant](./ViteSetup.md)
- [Install a Binary Build](../GettingStarted/Install-Binary-Build.md)
- [Build from Source](../GettingStarted/Build-From-Source.md)
- [Debloat Guide](../Performance/Debloat-Guide.md)
- [Compile-Time Switches](../Performance/Compile-Time-Switches.md)
