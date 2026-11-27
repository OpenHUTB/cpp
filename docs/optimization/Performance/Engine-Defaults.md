# 引擎默认更改

!!! note "概要"
    Vite 修改了虚幻引擎 4.27 的一批默认值以提升性能。这些改动会直接影响你的项目——其中一些改变的是运行时行为，不只是开销。在断定某个东西坏了之前，先读这一页。

为了提升性能，Vite 修改了虚幻引擎 4.27 的一些默认设置。这些修改会直接影响您的项目——其中一些会改变运行时行为，而不仅仅是成本。在断定项目出现问题之前，请先阅读此页面。

虚幻引擎的默认设置旨在确保所有功能开箱即用，这意味着它们启用了大多数项目永远不会用到的功能。Vite 修改了其中一些设置以提升性能，其原则是：您需要的功能应该是您主动开启的，而不是您忘记关闭的。

但代价是，从虚幻引擎 4.27 迁移到 Vite 的项目可能会出现不同的行为。此页面列出了所有变更。

## 运行时行为变更

!!! warning
    这些改动改变的是行为，不只是性能。如果游戏逻辑依赖原生默认值，就需要处理。

### 默认关闭重叠事件

除非显式启用，基本组件（Primitive Component）不再生成重叠事件。

只要有这个开关，生成重叠事件就有开销，不管有没有东西监听。原生项目里，大多数图元生成的重叠事件根本没人听。

**这对你意味着什么：** 需要重叠事件的组件必须显式勾选 **Generate Overlap Events**。触发体积、拾取检测，以及任何由 `OnComponentBeginOverlap` 驱动的逻辑都要打开这个标志。项目迁移后「我的触发器不工作了」，最可能就是这个原因。

例外是 `ACharacter`：自 3 月主要版本起，它的胶囊组件保持重叠事件开启，因为大多数项目都依赖它处理角色。想关掉的话，`Character.cpp` 里那一行有标记。

*`UPrimitiveComponent` 里的一行改动，作用于每个项目里的每个图元。*

[提交](https://github.com/GapingPixel/UnrealEngineVite-PhysX/commit/02d7c0ad0a7542b382a70dc3e37877d6ec052d76)


### 优化 Actor 运行时

Actor 运行时默认值已更改，以减少 `AActor::InitializeDefaults` 中每个 Actor 的开销：

| 默认值 | 原生 4.27 | Vite | 原因 |
|---|---|---|---|
| `SetCanBeDamaged` | `true` | `false` | 只有用到伤害系统的 Actor 才需要它 |
| `bRelevantForNetworkReplays` | `true` | `false` | 除非特意需要，否则不把 Actor 纳入演示录制 |
| `bRelevantForLevelBounds` | `true` | `false` | 避免对没有定义边界的 Actor 做关卡边界遍历 |

*大型网格、阻挡体积与必须定义世界边界的植被，需要把 `bRelevantForLevelBounds` 设回 `true`。*

[提交](https://github.com/GapingPixel/UnrealEngineVite-PhysX/commit/970cbb989c3712f13be1fa370b778e769e5d864c)

### 骨骼网格体优化配置

骨骼网格体是大多数项目中 CPU 占用率最高的组件之一，因此 `USkeletalMeshComponent` 默认提供的是性能优化配置，而非 Epic 提供的完整功能配置。游戏内资源通常包含大量骨骼网格体，因此为每个组件启用优化设置并不实际；可行的做法是将性能较低的配置设为默认，并在需要时启用性能较高的配置。

| 默认值 | 原生 4.27 | Vite |
|---|---|---|
| `VisibilityBasedAnimTickOption` | `AlwaysTickPoseAndRefreshBones` | `OnlyTickPoseWhenRendered` |
| `bEnableUpdateRateOptimizations` | `false` | `true` |
| `bHasCustomNavigableGeometry` | `Yes` | `No` |
| `bDisablePostProcessBlueprint` | `false` | `true` |
| `bUpdateOverlapsOnAnimationFinalize` | `true` | `false` |
| `bSkipKinematicUpdateWhenInterpolating` | `false` | `true` |
| `bSkipBoundsUpdateWhenInterpolating` | `false` | `true` |

*每一行改动的末尾注释都保留着 Epic 的原值，不用翻上游就能恢复原生行为。*

[提交](https://github.com/GapingPixel/UnrealEngineVite-PhysX/commit/33fe7c638829b8120e8a02ecc639acda761df835)

`VisibilityBasedAnimTickOption` 是需要关注的选项。`OnlyTickPoseWhenRendered` 表示屏幕外的 Actor 会完全停止评估姿势；若玩法代码要读取未渲染 Actor 的骨骼变换或插槽（例如武器枪口位置、IK 目标、远处 Actor 上的连接点），必须把组件设回 `AlwaysTickPose` 或 `AlwaysTickPoseAndRefreshBones`。

`bDisablePostProcessBlueprint = true` 是第二项：后期处理动画蓝图（常用于 IK 与骨骼校正）不再运行，除非为每个组件重新打开。

两个 `bSkip...WhenInterpolating` 开关只在开启更新率优化时才有意义，而 Vite 也默认开启了它：在动画只是插值而不是求值的那几帧，运动学物理体与边界不更新，等到下一次求值 tick 再补上。对于每帧都必须跟着姿势走的运动学物理体（比如近距离命中检测用的那个），请把这两个开关关掉。


### 默认不生成光照贴图 UV

静态网格体导入默认不再生成光照贴图 UV（[提交](https://github.com/GapingPixel/UnrealEngineVite-PhysX/commit/acce6fbe432fe9fe5a2536f0979befd5dcd741d1)）。

大多数 Vite 项目使用 [DDGI](../Rendering/DDGI-Dynamic.md) 而非烘焙光照，因此为每个导入的网格体生成光照贴图 UV 会浪费导入时间和 UV 通道。如果您的项目使用烘焙光照，请在静态网格体导入选项中启用此设置。


### 动画默认用 ACL 压缩

`BaseEngine.ini` 把默认的骨骼与曲线压缩设置指向 [Animation Compression Library](../Plugins/Bundled-Plugins.md)（`/ACLPlugin/ACLAnimBoneCompressionSettings` 与 `ACLAnimCurveCompressionSettings`），新导入的动画因此走 ACL。这也是 ACL 插件默认开启、并且 [ViteSetup](../Tools/ViteSetup.md) 的 Express 精简会保留它的原因。已有动画仍使用保存时的压缩设置。

### 独立运行与打包版本帧率上限 120

`Engine/Config/BaseGameUserSettings.ini` 里的 `FrameRateLimit=120.0` 把独立运行与打包版本限制在 120 FPS，以获得稳定的帧节奏（自 7 月主要版本起）。

**这对你意味着什么：** 跑得比 120 FPS 快的基准测试（包括好几个[演示项目](../ProjectsAndDemos.md)）在解除上限前只会报 120。运行时用 `t.MaxFPS 0` 解除上限，或者在项目的 `DefaultGameUserSettings.ini` 里改 `FrameRateLimit`。

## 可扩展性

Vite 重新平衡了 `Engine/Config/BaseScalability.ini` 里的阴影可扩展性分组。原生 4.27 从 Low 几乎没有阴影，一路铺到 Epic 与 Cinematic 非常昂贵；Vite 把这个跨度收窄了：Low 有可用的阴影，High 与 Epic 更便宜，Medium 保持不变。

| 分组 | `r.ShadowQuality`（原生 → Vite） | 相对原生的其他变化 |
|---|---|---|
| Low（`@0`） | 0 → **2** | 四个级联而不是一个，最大阴影分辨率 1024，距离场与胶囊体阴影开启，光照函数开启，光源保留完整绘制距离 |
| Medium（`@1`） | 3 → 3 | 无 |
| High（`@2`） | 5 → **4** | 阴影距离缩放 0.85 → 0.75，级联过渡 0.8 → 0.7 |
| Epic（`@3`） | 5 → 5 | 最多 8 个级联而不是 10 个，半径阈值 0.01 → 0.02，距离缩放 1.0 → 0.85，体积雾网格 16 px × 64 层而不是 8 px × 128 |
| Cinematic | 5 → 5 | 最大阴影分辨率 4096 → 2048，体积雾网格 8 px 而不是 4 px，历史未命中的超采样 4 次而不是 16 |

如果项目靠原生的 Epic 或 Cinematic 阴影预算来出预告片或离线截图，请在项目的 `DefaultScalability.ini` 里覆盖这些值，而不是改引擎文件。

## 切换回原生游戏默认值

上面这些重叠与骨骼网格体默认值最容易弄坏迁移过来的项目。自 8 月主要版本起，引擎根目录的 `ViteDefaults.bat` 可以在 Vite 与原生 UE 4.27 之间切换这些默认值，不用手改引擎源码：

```
ViteDefaults.bat                                  :: interactive menu showing the current state
ViteDefaults.bat -DetectOnly                      :: report the current state only
ViteDefaults.bat -SetOverlap ue427 -SetSkm vite   :: stock overlap, Vite skeletal mesh defaults
```

它会改写 `PrimitiveComponent.cpp` 与 `SkeletalMeshComponent.cpp`，所以之后要重新编译编辑器。能改项目里的组件就优先改项目；把引擎默认值切回去，等于把开销重新摊到每个关卡的每个图元或骨骼网格体上。

## 已禁用的插件

Vite 的插件配置会关掉大多数 Win64 项目从不加载的引擎插件，从而缩短编辑器启动时间、减少模块加载数量、减小打包体积。这份配置如何落到你的代码树，取决于版本：

| 版本 | 配置的应用方式 |
|---|---|
| LTS 至 6 月主要版本 | 直接内置在源码树里：插件以 `EnabledByDefault = false` 随附 |
| 7 月主要版本起 | 源码树保留原生 4.27 的值以维持兼容。运行 `devops\ueVite-ApplyVitePluginDefaults.bat` 应用配置 |

配置清单在 `devops\ViteEnginePluginDefaults.txt`，关掉 39 个引擎插件，打开 1 个：

- **关闭：** Paper2D；CLion、CodeLite 与 KDevelop 源码访问器；Plastic SCM 与 Subversion 源码管理；Chaos Cloth、Chaos Editor、Chaos Niagara 与 Chaos 求解器插件；Alembic 导入器；Facial Animation；Crypto Keys；Android、Apple 与 WMF 媒体播放；OpenXR、Oculus VR 与 SteamVR；以及 Google Cloud Messaging、Google PAD、Chunk Downloader、Mobile Patching Utils 等移动服务。
- **开启：** Blast，也就是 Vite 的[破碎](../Physics/Destruction-And-Cloth.md)路径。

```
devops\ueVite-ApplyVitePluginDefaults.bat            :: apply the Vite profile
devops\ueVite-ApplyVitePluginDefaults.bat --dryrun   :: report what would change
devops\ueVite-ApplyVitePluginDefaults.bat --stock    :: restore the stock 4.27 values
```

`--yes` 跳过确认提示。脚本只改 `.uplugin` 文件，因此在 `.uproject` 里显式点名某个插件的项目不受影响。

**VR 插件要单独注意**，需要就必须重新打开；Windows 上通过 Media Framework 播视频的项目，WMF Media 也一样。

[提交 1](https://github.com/GapingPixel/UnrealEngineVite-PhysX/commit/e9aebc2ef9f8acb7326a7e989f288ef68969342f) &middot;
[提交 2](https://github.com/GapingPixel/UnrealEngineVite-PhysX/commit/6c5948bf12ea61f82784193ccaa5d43c4f574ae9)

## 可以关掉的 Tick

### Niagara

插件启用时 Niagara 就会 Tick。不用就在项目级别关掉 Niagara 插件；4.27 里 Cascade 仍然可用。

## 光线追踪效果默认关闭

原生 4.27 让后期处理体积决定光追反射、半透明与环境光遮蔽是否运行（`-1`），并把光追阴影打开（`1`）。从 LTS 起的每个 Vite 版本都把四项设为 `0`：

| CVar | 原生 4.27 | Vite |
|---|---|---|
| `r.RayTracing.Reflections` | `-1`（后期处理体积决定） | `0` |
| `r.RayTracing.Translucency` | `-1`（后期处理体积决定） | `0` |
| `r.RayTracing.AmbientOcclusion` | `-1`（后期处理体积决定） | `0` |
| `r.RayTracing.Shadows` | `1` | `0` |

值为 `0` 时，这些效果的后期处理体积设置会被忽略；在体积里开过它们的项目，迁到 Vite 后一个都跑不起来。把 CVar 设为 `-1` 恢复原生行为，设为 `1` 则强制开启：

```ini
; Config/DefaultEngine.ini
[/Script/Engine.RendererSettings]
r.RayTracing.Reflections=-1
r.RayTracing.AmbientOcclusion=-1
```

光追半透明默认也被 `VITE_RT_PSO_DEBLOAT` 编译移除了，所以它的 CVar 只在关掉这个开关的构建里才有意义。它的帮助文本仍把 `-1` 写成默认值，代码里其实是 `0`。

## 光线追踪剔除：未改动，但值得开启

Vite 在这里保留原生 4.27 的行为：`r.RayTracing.Culling` 默认 `0`，因此不管光追图元在相机哪个方位，都留在加速结构里。在大场景里，打开它是最便宜的光追优化之一：

```ini
; Config/DefaultEngine.ini
[SystemSettings]
r.RayTracing.Culling=1
r.RayTracing.Culling.Radius=10000
r.RayTracing.Culling.Angle=1
```

开启后，**相机后方**的物体如果距离超过 `r.RayTracing.Culling.Radius`（默认 10000，即 100 m），或投影角度小于 `r.RayTracing.Culling.Angle`，就会被剔除。相机前方的物体永远不会被剔除，所以直接可见的画面不变，但被剔除的物体会从反射与光追阴影里消失。开启后记得检查玩家身后的镜面与大型投影物。

!!! note
    UE5 的 `r.RayTracing.Culling.UseMinDrawDistance` 在 4.27 与 Vite 中都不存在。

## 编辑器默认值

这些不影响运行时性能，但会改变编辑器行为，设置在 `Engine/Config/BaseEditorSettings.ini` 与 `BaseEditorPerProjectUserSettings.ini`：

| 设置 | Vite 默认值 | 作用 |
|---|---|---|
| `bDisableAllTutorialAlerts` | `True` | 编辑器里不弹教学提示 |
| `bShowNewPluginsPopup` | `True` | 设为 `False` 可关掉「有新插件可用」弹窗 |
| `bSendUsageData`、`bSendUnattendedBugReports` | `False` | 编辑器不上报使用数据，也不发送无人值守崩溃报告 |
| `bEnableAsyncTextureCompilation` | `True` | 纹理在后台编译，不阻塞编辑器 |
| `bCopyStarterContentPreference` | `False` | 新项目不附带 Starter Content |
| `bGenerateLightmapUVs` | `False` | 见[默认不生成光照贴图 UV](#默认不生成光照贴图-uv) |

编辑器的新闻推送也被移除，动画资源始终在新标签页打开——这是从 UE5 移植过来的行为（[提交](https://github.com/GapingPixel/UnrealEngineVite-PhysX/commit/8bdae919e6eae61a27ef8d09d38027592a473d8c)）。

## 可以自行选择的精简

这些出于兼容性考虑没法在分支发布版里关掉，但单个项目可以用。

**移除 Vite 插件。** 移除新增的 Vite 插件能缩短编译时间。在 Ryzen 9 9950X3D 上、Houdini Engine 还随附时测得：完整仓库 15 分钟，去掉 Vite 插件 12 分钟，Houdini 是最大的单一贡献项。6 月主要版本移除了 Houdini，所以现在的差距更小。

工具见[精简指南](Debloat-Guide.md)。

## 迁移现有项目

1. 测试每个触发体积和由重叠驱动的交互。重叠事件是最常见的故障点。
2. 检查使用 leader/follower 骨骼网格体配置的角色，曲线的传播是否还在。
3. 如果项目烘焙光照，重新打开光照贴图 UV 生成，并确认已有网格体的光照贴图 UV 仍然有效。
4. 重新启用项目需要的插件，尤其是 VR 与 WMF Media。
5. 复查可扩展性设置，因为 Low、High、Epic、Cinematic 的阴影现在都与原生不同。
6. 如果项目靠后期处理体积来开光追反射、半透明或 AO，把这些 CVar 设为 `-1`，见[光线追踪效果默认关闭](#光线追踪效果默认关闭)。

## 另请参阅

- [从 UE5 迁移](../GettingStarted/Migrating-From-UE5.md)
- [精简指南](Debloat-Guide.md)
- [性能分析](Profiling.md)
- [编译时切换](Compile-Time-Switches.md)