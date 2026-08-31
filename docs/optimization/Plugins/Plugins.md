# 插件

Vite 将各类厂商技术（NVIDIA、AMD、Intel）、反向移植的 UE5 插件以及第三方扩展直接集成在引擎目录结构中。其中大部分插件位于 [Engine\Plugins\Runtime\VitePlugins](https://github.com/GapingPixel/UnrealEngineVite-PhysX/tree/ueVite26-AugustMajor-release/Engine/Plugins/Runtime/VitePlugins) 目录下。

原生 UE 4.27 附带了大量插件，Vite 保留了其中的绝大多数。此外，Vite 还集成了一些额外的插件，包括 UE 4.27 上游版本中不存在的插件、仅适用于 UE5 的插件，以及通常需要手动集成的厂商 SDK。


## 在这部分

| 主题 | 覆盖内容 |
|---|---|
| [已捆绑的插件](./Bundled-Plugins.md) | Vite 在原生 4.27 基础上增加的所有内容，包含版本及启用状态 |
| [推荐的插件](./Proposed-Plugins.md) | 候选集成并推荐的外部插件 |

## 插件生效的位置

| 目录 | 内容 |
|---|---|
| `Engine\Plugins\Runtime\VitePlugins` | Vite 自行添加的插件：FSR 4、XeSS、ACL、ImGui、Motion Symphony、Kawaii Physics、PhysX Instanced Subsystem 等 |
| `Engine\Plugins\Runtime\Nvidia` | DLSS, Streamline, NIS, NRD, RTXGI, DeepDVC, Reflex, Ansel |
| `Engine\Plugins\Runtime\TressFX` | AMD TressFX 5.0 毛发系统 |
| `Engine\Plugins\GameWorks\Blast` | NVIDIA Blast 破坏效果运行时 |
| `Engine\Plugins\Experimental\BlastPlugin` | Blast 制作工具 |
| 其他所有位置 | 原生 4.27 插件 |

## 启用一个插件

大多数捆绑插件默认处于禁用状态（`EnabledByDefault` 设为 `false`），因此在您主动启用之前，它们不会产生任何开销。您可以按项目启用它们：

### 启用捆绑插件

1. 在编辑器中，打开 **Edit（编辑）** > **Plugins（插件）**。

2. 找到该插件并勾选 **Enabled（已启用）**。

根据提示重启编辑器。

或者直接将其添加到您的 `.uproject` 文件中：

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

**注意：** 启用插件会将该插件的着色器排列（shader permutations）、模块及启动开销包含在每一次构建中。请仅启用您实际使用的插件，其余保持禁用状态。相关信息请参阅[着色器编译与 PSO](../Performance/Shader-Compilation-And-PSO.md)。


## 插件与精简构建

[精简构建工具套件](../Performance/Debloat-Guide.md)可通过 `ExcludedPlugins.txt` 文件从引擎目录树中剔除插件。该列表的剔除策略较为激进，其中包含许多项目实际需要的插件，例如 `GameplayAbilities`、`ApexDestruction`、`HairStrands` 以及 NVIDIA 相关插件。

**警告：** 在执行插件剔除操作前，请务必查阅 `ExcludedPlugins.txt`。如果移除了项目所依赖的插件，系统无法“干净”地处理此类错误，导致引用该插件的资源无法加载。

`LocalBuilds\CompressBuildSeparate.bat` 脚本也使用同一列表将插件分离到独立的归档文件中；这是一种非破坏性的替代方案。相关信息请参阅[已安装构建](../Tools/Installed-Builds.md)。


## 添加自己的插件

项目插件应放置在 `<Project>\Plugins` 目录下（与标准 Unreal 引擎的规范一致）。供整个团队使用的引擎插件应放置在 [Engine\Plugins\Runtime\VitePlugins](https://github.com/GapingPixel/UnrealEngineVite-PhysX/tree/ueVite26-AugustMajor-release/Engine/Plugins/Runtime/VitePlugins) 目录下；此类插件需要重新构建引擎并进行重新分发。

若希望将插件打包集成到 Vite 中，该插件必须兼容 4.21–4.27 版本。仅支持 UE5 的插件不在纳入范围内。请参阅[推荐的插件](./Proposed-Plugins.md)以了解相关标准及当前的候选列表。


## 另请参阅

- [Bundled 插件](./Bundled-Plugins.md)
- [推荐的插件](./Proposed-Plugins.md)
- [Debloat 指南](../Performance/Debloat-Guide.md)
- [超分辨率](../Rendering/Upscalers.md)
