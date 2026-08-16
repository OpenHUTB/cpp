# 插件

<tldr>
<p>
Vite bundles vendor technology (NVIDIA, AMD, Intel), backported UE5 plugins and third-party additions
directly in the engine tree. Most live under <code>Engine\Plugins\Runtime\VitePlugins</code>.
</p>
</tldr>

Stock UE 4.27 ships a large plugin set, most of which Vite keeps. On top of that, Vite bundles plugins that
either do not exist for 4.27 upstream, exist only for UE5, or are vendor SDKs that normally have to be
integrated by hand.

## 在这部分

| 主题 | 覆盖内容 |
|---|---|
| [Bundled 插件](Bundled-Plugins.md) | Everything Vite adds on top of stock 4.27, with versions and enablement state |
| [提议的插件](Proposed-Plugins.md) | 候选集成并推荐的外部插件 |

## Where plugins live

| Directory | Contents |
|---|---|
| `Engine\Plugins\Runtime\VitePlugins` | Vite's own additions: FSR 4, XeSS, ACL, ImGui, Motion Symphony, Kawaii Physics, PhysX Instanced Subsystem and others |
| `Engine\Plugins\Runtime\Nvidia` | DLSS, Streamline, NIS, NRD, RTXGI, DeepDVC, Reflex, Ansel |
| `Engine\Plugins\Runtime\TressFX` | AMD TressFX 5.0 hair |
| `Engine\Plugins\GameWorks\Blast` | NVIDIA Blast destruction runtime |
| `Engine\Plugins\Experimental\BlastPlugin` | Blast authoring tools |
| Everywhere else | Stock 4.27 plugins |

## Enabling a plugin

Most bundled plugins ship with `EnabledByDefault` set to `false`, so they cost nothing until you ask for
them. Enable per project:

<procedure title="Enable a bundled plugin" id="enable-plugin">
    <step>Open <b>Edit &gt; Plugins</b> in the editor.</step>
    <step>Find the plugin and tick <b>Enabled</b>.</step>
    <step>Restart the editor when prompted.</step>
</procedure>

Or add it to your `.uproject` directly:

```json
{
  "Plugins": [
    {
      "Name": "DLSS",
      "Enabled": true
    }
  ]
}
```

<note>
Enabling a plugin adds its shader permutations, modules and startup cost to every build. Enable what you
use; leave the rest off. See <a href="Shader-Compilation-And-PSO.md">Shader Compilation and PSO</a>.
</note>

## Plugins and debloating

The [debloat suite](Debloat-Guide.md) can strip plugins from the engine tree using
`ExcludedPlugins.txt`. That list is aggressive and includes plugins many projects genuinely need &mdash;
`GameplayAbilities`, `ApexDestruction`, `HairStrands` and the NVIDIA plugins among them.

<warning>
Read <code>ExcludedPlugins.txt</code> before running a plugin strip. Removing a plugin a project depends on
will not fail cleanly &mdash; assets referencing it fail to load.
</warning>

The same list is used by `LocalBuilds\CompressBuildSeparate.bat` to split plugins into their own archive,
which is a non-destructive alternative. See [Installed Builds](Installed-Builds.md).

## Adding your own

Project plugins go in `<Project>\Plugins`, as with stock Unreal. Engine plugins that the whole team should
have go in `Engine\Plugins\Runtime\VitePlugins` and require an engine rebuild and redistribution.

If you want a plugin bundled with Vite, it must be compatible with 4.21&ndash;4.27. UE5-only plugins are
out of scope. See [Proposed Plugins](Proposed-Plugins.md) for the criteria and the current candidate list.

## See also

- [Bundled Plugins](Bundled-Plugins.md)
- [Proposed Plugins](Proposed-Plugins.md)
- [Debloat Guide](Debloat-Guide.md)
- [Upscalers](Upscalers.md)
