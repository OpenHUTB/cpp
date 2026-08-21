# ViteSetup Assistant

<tldr>
<p>
<code>ViteSetup.bat</code> at the engine root runs a nine-step guided setup from environment check to a
launched editor. <code>ViteSetup.bat menu</code> gives you the individual operations instead.
</p>
</tldr>

The assistant exists because setting up an Unreal source build has many steps, each with failure modes that
produce unhelpful errors when you get them wrong. It checks the environment first, so a missing toolchain
component fails at step 1 with a clear message rather than at step 5 with a linker error.

## Guided mode

Run `ViteSetup.bat` with no arguments.

| Step | Action |
|---|---|
| 1 | Check the environment and enforce the toolchain |
| 2 | Set up dependencies, with platform selection |
| 3 | Generate Visual Studio project files |
| 4 | Choose source or binary build |
| 5 | Build the selected target |
| 6 | Register the selected build |
| 7 | Create `UEViteFork.lnk` on the desktop |
| 8 | Optional engine debloat |
| 9 | Finish; source builds launch the editor, binary builds do not |

Each step pauses so you can read the output before continuing. Failures stop the assistant and report an
error code rather than pushing on.

## Step 1 &mdash; environment check

The assistant enforces a specific toolchain and will not proceed without it:

| Requirement | Enforced value |
|---|---|
| Visual Studio | 2022 |
| MSVC | 14.44 |
| Windows SDK | 10.0.26100, subversion .7705 or higher, major pinned to 26100 |
| .NET Framework reference assemblies | v4.6.2 and v4.5 |

<warning>
The enforced toolchain is not the same as the toolchain Vite is currently developed against, which is
Visual Studio 2026 with MSVC 14.50 and Windows SDK 10.0.26100. The assistant's check has not been updated.
<p>
If you are on the newer toolchain, the check will fail. Build through
<a href="../GettingStarted/Build-From-Source.md">the manual path</a> or use the individual menu options rather than the
guided flow. See <a href="../GettingStarted/Toolchain-Requirements.md">Toolchain Requirements</a>.
</p>
</warning>

The .NET Framework 4.5 reference assemblies are the component people most often lack, since recent Visual
Studio installers no longer offer them by default. See
[Build Troubleshooting](../GettingStarted/Build-Troubleshooting.md).

## Step 2 &mdash; dependency setup

Rather than running `Setup.bat` with everything, the assistant offers setup profiles:

| Profile | What it does |
|---|---|
| Step-by-step assistant | Choose platforms and optional content individually, then review |
| Recommended Win64 | Win64 only. Excludes non-Windows platforms, samples, templates, feature packs, docs, XR and consoles. |
| Win64 ultra compact | As recommended, with the smallest optional content set |
| Win64 + templates / feature packs | Keeps project templates and starter content |
| Win64 + Android | Adds Android dependencies |
| Win64 + Linux | Adds Linux and cross-compile dependencies |
| Full setup | Passes no `-exclude` argument to `Setup.bat` |

The step-by-step assistant exposes individual platform toggles for Android, iOS/tvOS, Mac, Linux, WinRT,
HTML5 and consoles. Win32 support files are always included, since Win64 requires them.

**Recommended Win64 is the right choice for most people.** Downloading dependencies you will never use
costs bandwidth, disk space and setup time. If you later need a platform, re-run setup with it enabled.

## Step 4 &mdash; source or binary

| Option | Result |
|---|---|
| Source build | Builds `UE4Editor`, `ShaderCompileWorker` and `UnrealLightmass` in the source tree, Win64 Development |
| Binary Installed Build | Incremental BuildGraph run producing `LocalBuilds\Engine\Windows` plus `UE_ViteFork.7z` |

Choose source if you are working on the engine or want to modify
[compile-time switches](../Performance/Compile-Time-Switches.md). Choose binary if you want a redistributable engine to
hand to people who do not need to compile it. See [Installed Builds](./Installed-Builds.md).

## Steps 6 and 7 &mdash; registration and shortcut

Registration writes the engine path under
`HKCU\Software\Epic Games\Unreal Engine\Builds` as `UEViteFork`, which is what makes the build selectable in
a `.uproject` file's engine association. The engine association identifier is `UE_ViteFork`.

The shortcut step creates `UEViteFork.lnk` pointing at `UE4Editor.exe`.

Both are also available standalone as `LocalBuilds\RegistryAdd.bat` and `LocalBuilds\MakeShortcut.bat`.

## Step 8 &mdash; debloat

Optionally runs the debloat assistant, offering:

| Option | Behaviour |
|---|---|
| Move to a recovery folder | Restorable later. Destination is `ViteDebloat_Moved` next to the engine folder. Recommended. |
| Delete permanently | Smallest footprint, not reversible |

Full detail in the [Debloat Guide](../Performance/Debloat-Guide.md).

## Menu mode

`ViteSetup.bat menu` skips the guided flow and presents the operations individually:

| Option | Action |
|---|---|
| 1 | Check the environment |
| 2 | Set up dependencies (assistant) |
| 3 | Generate Visual Studio files |
| 4 | Build Unreal Engine source |
| 5 | Setup, then generate Visual Studio files |
| 6 | Open `UE4.sln` |
| 7 | Show setup profiles |
| 8 | Build installed binary |
| 9 | Debloat engine (assistant) |
| 0 | Exit |

Menu mode is what you want for day-to-day work. Regenerating project files after adding a module, or
rebuilding after a pull, does not need the whole nine-step flow.

## Key paths

| Variable | Path |
|---|---|
| Editor executable | `Engine\Binaries\Win64\UE4Editor.exe` |
| Installed build staging | `LocalBuilds\Engine\Windows` |
| Installed build archive | `LocalBuilds\Engine\UE_ViteFork.7z` |
| Source cache | `UE4_Source_Cache` |
| Engine association | `UE_ViteFork` |

## See also

- [Build from Source](../GettingStarted/Build-From-Source.md)
- [Toolchain Requirements](../GettingStarted/Toolchain-Requirements.md)
- [Installed Builds](./Installed-Builds.md)
- [Build Troubleshooting](../GettingStarted/Build-Troubleshooting.md)
- [Debloat Guide](../Performance/Debloat-Guide.md)
