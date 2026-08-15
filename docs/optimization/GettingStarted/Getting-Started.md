# Getting Started

<tldr>
<p>
Fastest path: download an installed binary build, run <code>RegistryAdd.bat</code>, and open the editor.
Building from source takes roughly 15&ndash;30 minutes of compile time on a modern desktop CPU and
requires a pinned toolchain.
</p>
</tldr>

This section takes you from nothing installed to a running editor and a first project on Unreal Engine
Vite. Read [Introduction to Vite](Introduction-to-Vite.md) first if you have not yet decided whether the
fork is right for your project.

## Choose how you install

Vite ships in two forms. They are functionally the same editor; the difference is whether you compile
the engine yourself.

| | Installed binary build | Source build |
|---|---|---|
| Setup time | Minutes | 15&ndash;30 min compile, plus dependency download |
| Disk footprint | Smaller, no intermediates | Large, includes full source and intermediates |
| Can modify engine code | No | Yes |
| Can debug into engine code | Only with symbols shipped | Yes |
| Recommended for | Artists, designers, evaluation | Programmers, anyone contributing to the fork |

Most teams run a mixed setup: engineers on source builds, everyone else on an installed build produced
from the same commit. See [Installed Builds](Installed-Builds.md) for how to produce one.

## Steps in order

1. Check [System Requirements](System-Requirements.md) so you know what hardware and disk space you need.
2. Install the [Toolchain Requirements](Toolchain-Requirements.md). This step is mandatory and unforgiving
   &mdash; the wrong MSVC or Windows SDK version produces confusing compile errors rather than a clear
   message.
3. Either [Install a Binary Build](Install-Binary-Build.md) or [Build from Source](Build-From-Source.md).
4. Create your [First Project](First-Project.md).
5. If you are bringing existing content across, read [Migrating from Unreal Engine 5](Migrating-From-UE5.md).

If anything fails along the way, [Build Troubleshooting](Build-Troubleshooting.md) covers the errors that
come up most often, including the `C4668: '__has_feature' is not defined` error that signals a toolchain
mismatch.

## What is different from stock Unreal Engine 4.27

If you have built Unreal Engine from source before, three things will surprise you.

**GitDeps is already patched.** Stock 4.27 fails to download dependencies because the CDN endpoints have
moved. Vite ships the fix, so `Setup.bat` works with no manual edits. When `Setup.bat` asks whether to
overwrite local changes, answer `N`.

**Ray tracing is on by default.** New projects enable ray-traced shadows, reflections, translucency and
ambient occlusion out of the box. This is deliberate, so that the features are discoverable, but it means
a new empty project is heavier than a stock 4.27 one. See [Ray Tracing](Ray-Tracing.md) for how to turn
individual effects off.

**Several engine defaults are changed for performance.** Overlap events, skeletal mesh settings, lightmap
UV generation and a number of plugins differ from stock. These changes affect gameplay behaviour, not just
frame time, so read [Engine Default Changes](Engine-Defaults.md) before you ship.

## Unreal Engine fundamentals

This manual documents the fork, not the engine underneath it. Everything Vite does not change works the way
Unreal Engine 4.27 works, and Epic's own 4.27 documentation is the correct reference for it:

| Topic | Epic's 4.27 documentation |
|---|---|
| Projects, templates and directory structure | [Working with Unreal Projects and Templates](https://dev.epicgames.com/documentation/unreal-engine/working-with-unreal-projects-and-templates?application_version=4.27) |
| Setting up Visual Studio for source builds | [Setting Up Visual Studio](https://dev.epicgames.com/documentation/en-us/unreal-engine/setting-up-visual-studio-development-environment-for-cplusplus-projects-in-unreal-engine?application_version=4.27) |
| Blueprints and the gameplay framework | [Unreal Engine 4.27 documentation](https://dev.epicgames.com/documentation/en-us/unreal-engine/unreal-engine-4-27-documentation) |

Keep the version selector on 4.27. UE5 documentation describes systems Vite deliberately does not have,
and following it is a common source of confusion for people arriving from UE5.

## See also

- [Introduction to Vite](Introduction-to-Vite.md)
- [Toolchain Requirements](Toolchain-Requirements.md)
- [ViteSetup Assistant](ViteSetup.md)
- [Engine Default Changes](Engine-Defaults.md)
