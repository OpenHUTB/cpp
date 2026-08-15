# Debloat Guide

<tldr>
<p>
Three scripts in <code>devops\</code>, driven by a single <code>config.txt</code>, remove engine content
you do not need. Everything defaults to <b>dry run</b> and <b>move rather than delete</b>, so a mistake is
recoverable.
</p>
</tldr>

A full Unreal Engine 4.27 source tree contains support for every platform, every workflow and every
template Epic ships. A Win64-only project uses a fraction of it. Debloating recovers disk space and reduces
build time.

Original Vite debloat scripts by Bikouz.

## The suite

| Script | Purpose |
|---|---|
| `ueVite-debloat-SetupSlim.bat` | Runs engine dependency setup with a Win64-only focus, so unneeded platform dependencies are never downloaded |
| `ueVite-debloat-StripExecute.bat` | Removes platform binaries, non-Windows tools, templates, sample content and optionally plugins |
| `ueVite-debloat-StripDebugSymbols.bat` | Removes debug symbols according to `ExcludedPdbs.txt` |

All three live in `devops\` at the engine root and read `devops\config.txt`. You can pass an alternative
config path as the first argument.

## Safety model

The scripts are deliberately conservative, and it is worth understanding how before running them.

**Dry run is the default.** `DRYRUN=1` prints every action without touching a file. Review the output
before changing it.

**Move is the default, not delete.** `MODE=move` relocates targeted files into `MOVE_DIR`, preserving their
relative layout so they can be restored by copying back. `MODE=delete` is permanent.

**The move destination cannot be inside the engine tree.** The script refuses, because a move destination
inside the tree would be caught by subsequent passes.

**Drive roots are refused.** Both the engine root and the move destination are checked.

**The engine root is validated.** The script confirms `Engine\Binaries\` exists before doing anything.

## Configuration

`devops\config.txt` uses `KEY=VALUE` lines. Comment a line out with `#` to **preserve** what it targets;
uncomment to **enable** an optional target. This inversion is worth internalising: an uncommented `STRIP`
line means that thing gets removed.

### Core settings

```
MODE=move
MOVE_DIR=..\ViteDebloat_Moved
DRYRUN=1
```

`MOVE_DIR` resolves against the engine root, so the default is a sibling folder of the engine.

### Platforms

Win64 is always kept. Uncomment a platform to also download and keep its dependencies during
`SetupSlim`:

```
#KEEP_PLATFORM=Android
#KEEP_PLATFORM=IOS
#KEEP_PLATFORM=Linux
#KEEP_PLATFORM=Mac
#KEEP_PLATFORM=HTML5
```

Do this **before** running setup. Not downloading a dependency is cheaper than downloading and then
deleting it.

### Templates

```
STRIP_OTHER_TEMPLATES=1
KEEP_TEMPLATE=TP_ThirdPerson
KEEP_TEMPLATE=TP_ThirdPersonBP
KEEP_TEMPLATE=TemplateResources
```

Every folder under `Templates\` not named by a `KEEP_TEMPLATE` line is targeted. `TemplateResources` is
shared by all templates and must be kept if you keep any template.

### Strip targets

The default target list, grouped by category:

**Platform binaries not needed for Win64**

```
STRIP=Engine\Binaries\Win64\Android
STRIP=Engine\Binaries\Win64\IOS
STRIP=Engine\Binaries\Win64\Lumin
STRIP=Engine\Binaries\DotNET\IOS
```

**Non-Windows deployment and development tools**

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

3ds Max scripts are preserved by default, since 3ds Max is a Windows DCC tool.

**UnrealFileServer**

```
STRIP=Engine\Binaries\Win64\UnrealFileServer.exe
#STRIP=Engine\Source\Programs\UnrealFileServer
```

UnrealFileServer serves cooked and staged files to remote devices and powers network Cook-on-the-Fly. It is
safe to remove if you develop and package only for local Win64, do not deploy to remote devices, do not use
network Cook-on-the-Fly, and do not target consoles or mobile through the file-server workflow.

The prebuilt binary is targeted by default; the source folder is preserved so the tool can be rebuilt later.

**Engine content**

```
STRIP=Samples\StarterContent
STRIP=Samples\MobileStarterContent
STRIP=FeaturePacks\StarterContent.upack
#STRIP=Samples\NGXTest
#STRIP=Samples\PixelStreaming
#STRIP=Samples\RTXGI_Test
```

Starter Content is removed per the debloat policy. The fork-specific test samples are preserved by default
&mdash; `RTXGI_Test` in particular is useful when verifying [DDGI](DDGI-Dynamic.md).

Entries that do not exist in the tree are skipped with a notice, which is harmless.

## The plugin pass

Disabled by default, because plugin needs vary per project:

```
#PLUGIN_LIST=ExcludedPlugins.txt
```

`ExcludedPlugins.txt` lists roughly 200 engine plugin paths, covering mobile and XR platforms, source
control providers other than the one you use, Chaos plugins (unnecessary since Vite uses
[PhysX](PhysX.md)), enterprise and virtual production tooling, and a large set of experimental plugins.

The file is looked up next to the scripts first, then in the engine root.

<warning>
Read <code>ExcludedPlugins.txt</code> before enabling this pass. It includes entries your project may need
&mdash; <code>Engine/Plugins/Runtime/GameplayAbilities/</code>,
<code>Engine/Plugins/Runtime/ApexDestruction/</code>, <code>Engine/Plugins/Runtime/HairStrands/</code>,
<code>Engine/Plugins/Runtime/Nvidia/</code> (which contains the
<a href="Upscalers.md">DLSS and Streamline plugins</a>) and
<code>Engine/Plugins/Runtime/PhysXVehicles/</code> are all on the list.
<p>
Comment out the lines for anything you use before running the pass.
</p>
</warning>

## Running it

<procedure title="Debloat an engine installation" id="run-debloat">
    <step>
        Open <code>devops\config.txt</code> and review every uncommented <code>STRIP</code> line. Comment
        out anything you need.
    </step>
    <step>Confirm <code>DRYRUN=1</code> and <code>MODE=move</code>.</step>
    <step>
        Run <code>ueVite-debloat-StripExecute.bat</code> and read the full output. This is the step people
        skip and then regret.
    </step>
    <step>Set <code>DRYRUN=0</code> and run it again.</step>
    <step>
        Build the engine and open the editor. Confirm your projects still load and the plugins you need
        are present.
    </step>
    <step>
        Once you are confident, delete <code>MOVE_DIR</code> to actually reclaim the space. Until you do,
        the files are still on disk.
    </step>
</procedure>

For a fresh clone, run `ueVite-debloat-SetupSlim.bat` instead of `Setup.bat` so unneeded platform
dependencies are never downloaded in the first place.

## Debug symbols

`ueVite-debloat-StripDebugSymbols.bat` removes PDB files according to `ExcludedPdbs.txt`. Debug symbols are
a large fraction of a source build's disk footprint.

Keep symbols for anything you might need to debug. Stripping symbols for the engine modules you never step
into is a large saving; stripping symbols for the ones you do turns a readable callstack into hexadecimal.

## Restoring

In `move` mode, `MOVE_DIR` mirrors the engine tree's relative layout. Copy the contents back over the engine
root to restore.

If you used `delete` mode, restoration means re-cloning or re-running `Setup.bat`.

## What this does not do

Debloating reduces disk footprint and, through the plugin pass, build time. It does **not** improve runtime
performance &mdash; a plugin that is present but disabled costs nothing at runtime.

For runtime performance, see [Profiling](Profiling.md) and
[Engine Default Changes](Engine-Defaults.md). For build time specifically, removing the added Vite plugins
saves roughly three minutes on a full 15-minute build; see
[Shader Compilation and PSO](Shader-Compilation-And-PSO.md).

## See also

- [Build from Source](Build-From-Source.md)
- [Engine Default Changes](Engine-Defaults.md)
- [Bundled Plugins](Bundled-Plugins.md)
- [Cache Management](Cache-Management.md)
