# Installing a Binary Build

<tldr>
<p>
Extract <code>UE_ViteFork.7z</code>, run <code>RegistryAdd.bat</code> to register the engine, run
<code>MakeShortcut.bat</code> for a desktop launcher, then set
<code>"EngineAssociation": "UE_ViteFork"</code> in your <code>.uproject</code>.
</p>
</tldr>

An installed build is a precompiled, redistributable copy of the editor. It behaves like an
Epic Games Launcher installation: you can open and cook projects, but you cannot modify or debug into
engine source. This is the right choice for artists, designers and anyone evaluating the fork.

Installed builds are produced from a source build by the team. If you need to make one yourself, see
[Installed Builds](Installed-Builds.md).

## Contents of a build archive

A packaged Vite build extracts to a folder containing:

| Item | Purpose |
|---|---|
| `Engine\` | The engine itself, including `Engine\Binaries\Win64\UE4Editor.exe` |
| `MakeShortcut.bat` | Creates `UEViteFork.lnk` pointing at the editor executable |
| `RegistryAdd.bat` | Registers the folder as the `UEViteFork` engine association |
| `RegistryRemove.bat` | Removes that registration |
| `Templates\` | Project templates, if the build was packaged with them |
| `FeaturePacks\` | Feature packs, if the build was packaged with them |
| `Samples\` | Sample content, if the build was packaged with them |

Lean builds omit templates, feature packs, samples and debug symbols. Some builds ship an accompanying
`ExcludedPlugins.7z` containing plugins that were split out of the main archive to keep it small; extract
it over the same folder if you need those plugins.

## Installation

<procedure title="Install and register an installed build" id="install-binary">
    <step>
        Extract <code>UE_ViteFork.7z</code> to a path with no spaces or non-ASCII characters, for example
        <code>D:\Engines\UE_ViteFork</code>. Avoid <code>Program Files</code>; the editor writes to its own
        directory and the permission prompts are not worth it.
    </step>
    <step>
        If the build shipped a separate <code>ExcludedPlugins.7z</code> and you need those plugins, extract
        it into the same folder now.
    </step>
    <step>
        Run <code>RegistryAdd.bat</code>. This writes the engine path to
        <code>HKCU\Software\Epic Games\Unreal Engine\Builds</code> under the name <code>UE_ViteFork</code>,
        and removes stale GUID-keyed entries that pointed at the same folder.
    </step>
    <step>
        Run <code>MakeShortcut.bat</code> to create <code>UEViteFork.lnk</code> next to the engine folder.
    </step>
    <step>
        Launch the editor through the shortcut, or directly via
        <code>Engine\Binaries\Win64\UE4Editor.exe</code>.
    </step>
</procedure>

> Registration is per-user, written under `HKCU`. Each Windows user account that needs the engine must run
> `RegistryAdd.bat` once.
>
{style="note"}

## Pointing a project at the build

Open your `.uproject` file in a text editor and set the engine association:

```json
{
    "FileVersion": 3,
    "EngineAssociation": "UE_ViteFork",
    "Category": "",
    "Description": ""
}
```

Projects packaged inside an installed build archive already have this set. If you right-click a `.uproject`
and choose **Switch Unreal Engine version**, `UE_ViteFork` will appear in the list once the build is
registered.

## Moving or removing a build

The registry entry stores an absolute path, so moving the engine folder breaks it. If you relocate a build:

1. Run `RegistryRemove.bat` from the old location, or delete the `UE_ViteFork` value manually.
2. Move the folder.
3. Run `RegistryAdd.bat` from the new location.

To uninstall entirely, run `RegistryRemove.bat` and then delete the folder. Nothing is written outside the
engine directory and the single `HKCU` registry value, apart from the usual per-user Unreal directories
under `%LOCALAPPDATA%\UnrealEngine` and `%APPDATA%\Unreal Engine`.

## Running source and binary builds side by side

You can have both installed. They are separate engine associations as long as they register under different
names, so if you want a source build and an installed build available at the same time, change the
`ENGINE_ASSOCIATION` value in one of them before registering. Note that both builds share the per-user
Unreal directories, so editor layout and some settings will be common between them.

## See also

- [Getting Started](Getting-Started.md)
- [Installed Builds](Installed-Builds.md)
- [Packaging and Distribution](Installed-Builds.md)
- [Creating Your First Project](First-Project.md)
