# 缓存管理


<code>WipeShaderCache.bat</code> at the engine root deletes the engine's derived data cache, intermediate shaders and shader debug info. Reach for it when the editor's behaviour stops matching your source changes.


Unreal caches aggressively. That is what makes iteration bearable, but it also means a stale cache can make a correct change appear not to work &mdash; or an incorrect one appear to.

## 脚本清除了什么

`WipeShaderCache.bat` deletes three directories relative to the engine root:

| 目录 | 内容 |
|---|---|
| `Engine\DerivedDataCache` | Compiled shaders, cooked textures, built meshes and every other derived artefact |
| `Engine\Intermediate\Shaders` | Intermediate shader compilation output |
| `Engine\Saved\ShaderDebugInfo` | Shader debug symbols and preprocessed source dumps |

It reports each directory as deleted, not found, or failed. A failure almost always means something still
holds a file handle.

<warning>
Close the editor and all <code>ShaderCompileWorker.exe</code> processes before running the script. Shader
compile workers can outlive the editor; check Task Manager if a deletion fails.
</warning>

After wiping, the next editor launch recompiles everything it needs. On a large project this can be tens of
minutes or more. Do not do it casually.

## 什么时候清除

<deflist>
<def title="You changed a .usf or .ush file and nothing happened">
Shader source changes are usually picked up, but engine-level global shader changes sometimes are not.
Try <code>recompileshaders global</code> or <code>recompileshaders changed</code> in the console first
&mdash; it is far faster than a full wipe.
</def>
<def title="You changed a compile-time switch">
Changing <a href="../Performance/Compile-Time-Switches.md">VITE_RT_PSO_DEBLOAT</a>, <code>VITE_O_SSAO</code> or any other
<code>VITE_*</code> switch changes which shader permutations exist. The engine rebuild handles the C++,
but cached shaders from the old configuration can linger. Wipe after switching.
</def>
<def title="You pulled engine changes that touched the renderer">
A merge that changes shader code or the shader map keying can leave a cache the engine misreads.
</def>
<def title="You are getting shader compilation errors that do not match the source">
A classic stale-cache symptom: the reported error line does not exist in the file you are looking at.
</def>
<def title="The editor crashes on startup after an engine change">
Worth trying before deeper investigation, since it is cheap to rule out.
</def>
</deflist>

## 什么时候不清除

Wiping the DDC is a heavy hammer and is frequently applied to problems it cannot solve.

- **Project-level problems.** The script clears the *engine* DDC. Your project has its own
  `DerivedDataCache` folder and its own `Intermediate` and `Saved` directories. Engine-level wiping does
  not touch them.
- **运行时渲染问题。** If an effect renders wrongly but consistently, that is a code or
  configuration problem, not a cache problem. Check the
  [compile-time switch availability table](../Rendering/Ray-Tracing.md) first &mdash; many ray tracing features are
  compiled out by default and their console variables silently do nothing.
- **长渲染编译时间。** Wiping makes this worse, not better. See
  [着色器编译和 PSO](../Performance/Shader-Compilation-And-PSO.md).

## 更轻量级的选择

Try these before a full wipe:

| 方法 | 清除 | 花费 |
|---|---|---|
| `recompileshaders changed` | 仅修改的着色器 | 秒 |
| `recompileshaders global` | 全局着色器 | Under a minute |
| `recompileshaders material <name>` | One material | Seconds |
| Delete the project's `Intermediate\` | Project build intermediates | Project rebuild |
| Delete `Engine\Intermediate\Shaders` only | Intermediate shader output, keeping the DDC | Partial recompile |

The last one is worth knowing: the DDC is the expensive part to rebuild. If you only need to clear
intermediate shader state, deleting that one directory by hand is much cheaper than running the full
script.

## 缓存位置

| 缓存 | 路径 | 是否被脚本清除 |
|---|---|---|
| Engine DDC | `Engine\DerivedDataCache` | 是 |
| Engine intermediate shaders | `Engine\Intermediate\Shaders` | 是 |
| Shader debug info | `Engine\Saved\ShaderDebugInfo` | 是 |
| Project DDC | `<Project>\DerivedDataCache` | 否 |
| Project intermediates | `<Project>\Intermediate` | 否 |
| Shared / network DDC | Per `BaseEngine.ini` DDC configuration | 否 |
| Local user DDC | `%LOCALAPPDATA%\UnrealEngine\Common\DerivedDataCache` | 否 |

<note>
If your team uses a shared network DDC, wiping locally is usually pointless: the local cache repopulates
from the share, including whatever stale entries prompted the wipe. Confirm the shared DDC's state before
concluding your local cache was the problem.
</note>

## Disk footprint

The engine DDC grows without bound during development. On a project that exercises many material and
ray-tracing permutations it can reach tens of gigabytes. Periodically wiping it is a legitimate way to
reclaim disk space, accepting the recompile cost.

If disk space is the actual concern, the [Debloat Guide](../Performance/Debloat-Guide.md) covers larger and more permanent
savings.

## 另请参见

- [着色器编译和 PSO](../Performance/Shader-Compilation-And-PSO.md)
- [Compile-Time Switches](../Performance/Compile-Time-Switches.md)
- [Build Troubleshooting](../GettingStarted/Build-Troubleshooting.md)
- [精简指南](../Performance/Debloat-Guide.md)
