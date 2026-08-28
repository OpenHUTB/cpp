# 创建您的第一个项目

请按常规方式创建项目，然后确定您实际需要哪些光线追踪效果——Vite 默认启用了其中大部分效果。请使用 `r.GlobalIllumination.ExperimentalPlugin 1` 启用 [DDGI](../Rendering/DDGI-Dynamic.md)，并配合 `r.SSGI.Enable 1` 使用。

在 Vite 中创建项目的流程与在标准版 Unreal Engine 4.27 中完全相同。两者的区别在于默认渲染配置，因此本页面将重点介绍项目打开后应立即进行的操作。

**注意：** 本手册仅说明 Vite 所做的更改，而非 Unreal Engine 本身的功能。对于未更改的部分（如项目结构、模板、内容浏览器、蓝图、材质、打包等），[模拟引擎的文档](https://openhutb.github.io/engine_doc/zh-CN/index.html)依然适用并可作为参考。


## 创建项目


### 创建一个新的 Vite 项目

1. 通过桌面快捷方式或 `Engine\Binaries\Win64\UE4Editor.exe` 启动编辑器。

2. 选择一个模板。“第三人称”（Third Person）模板是精简预设（debloat presets）所保留的模板，也是最稳妥的起点。

3. 如果你打算通过代码来驱动渲染功能（本手册中的大多数示例均基于此方式），请选择 C++ 而非蓝图（Blueprint）。


如果您要打开现有项目，请右键点击 `.uproject` 文件，选择**Switch Unreal Engine version（切换虚幻引擎版本）**，然后选择 `UE_ViteFork`。


## 在构建内容前，请先了解默认设置

**警告：** Vite 默认启用了全套光线追踪功能——包括阴影、反射、半透明效果和环境光遮蔽（Ambient Occlusion, AO）。这样设计是为了让新用户能立即体验这些功能，但也意味着相比标准的 4.27 版本，空项目会消耗更多性能资源。

请尽早确定项目实际需要哪些效果，因为这将决定您的美术制作与性能预算规划。一款目标为 4K120 帧的风格化竞技游戏通常仅启用 DDGI；而一款追求高画质、目标为 4K60 帧的游戏，则可能会额外加入光线追踪反射和曲面细分（Tessellation）技术。请参阅[性能目标](../EngineOverview/Performance-Targets.md)部分，了解 Vite 针对优化的四种参考配置。

在着手构建大量内容之前，另一组值得了解的默认设置是[引擎默认设置变更](../Performance/Engine-Defaults.md)。其中几项更改不仅影响性能开销，还会改变功能行为——最显著的例子包括：图元组件（primitive components）上的重叠事件（overlap events）默认处于禁用状态，且导入资源时默认关闭光照贴图 UV 生成功能。


## 通过代码启用功能

在示例项目中，设置这些功能的常规位置是角色类的构造函数或 `BeginPlay` 方法。Vite 示例项目正是这样做的。

```c++
// Global illumination
IConsoleManager::Get().FindConsoleVariable(TEXT("r.GlobalIllumination.ExperimentalPlugin"))->Set(1);
IConsoleManager::Get().FindConsoleVariable(TEXT("r.SSGI.Enable"))->Set(1);

// Ray tracing effects
IConsoleManager::Get().FindConsoleVariable(TEXT("r.RayTracing.AmbientOcclusion"))->Set(1);
IConsoleManager::Get().FindConsoleVariable(TEXT("r.RayTracing.Reflections"))->Set(1);
IConsoleManager::Get().FindConsoleVariable(TEXT("r.RayTracing.Shadows"))->Set(1);
IConsoleManager::Get().FindConsoleVariable(TEXT("r.RayTracing.Translucency"))->Set(1);
IConsoleManager::Get().FindConsoleVariable(TEXT("r.RayTracing.SampledDirectLighting"))->Set(1);
IConsoleManager::Get().FindConsoleVariable(TEXT("r.RayTracing.MeshCaustics.Enable"))->Set(1);
```

设置 `r.GlobalIllumination.ExperimentalPlugin 1` 可启用 DDGI。建议同时运行 SSGI，而非将其视为可选项：DDGI 负责处理全局尺度的光线反弹，而 SSGI 则用于补充高频接触细节——这些细节往往因探针体积（probe volumes）的精度限制而难以捕捉。请参阅[DDGI 与 SSGI 结合使用](../Rendering/SSGI.md)相关内容。

在生产环境中，建议在配置文件而非代码中设置这些参数，以便利用其可扩展性，并支持针对不同平台进行覆盖设置：

```ini
; Config/DefaultEngine.ini
[/Script/Engine.RendererSettings]
r.GlobalIllumination.ExperimentalPlugin=1
r.SSGI.Enable=1
r.RayTracing.Reflections=1
```

设置 `r.RayTracing.ForceAllRayTracingEffects 1` 可同时启用所有光线追踪效果。这有助于快速预览引擎的能力，但不适合用于最终发布的配置。


## 验证其是否生效

按下波浪号（~）键打开控制台，并检查以下内容：

- `stat unit` —— 查看帧（frame）、游戏（game）、绘制（draw）和 GPU 的耗时。启用光线追踪效果后，如果 GPU 耗时占主导地位，这是正常现象；此时可尝试逐一关闭各项效果以进行排查。
- `stat gpu` —— 查看各渲染通道（pass）的 GPU 开销；通过此命令可判断反射（reflections）或全局光照（GI）哪一项更耗费资源。
- `r.RayTracing.Reflections 0` —— 在运行时切换效果，观察性能数据的变化。

Vite 还集成了基于 ImGui 的基准测试工具，用于编辑器内及游戏运行时的性能分析。请参阅[性能分析与基准测试](../Performance/Profiling.md)相关内容。


## 值得打开的示例项目

与其从零开始构建测试场景，不如直接使用那些已展示相关功能的现有场景：

- [技术演示项目](../ProjectsAndDemos/Tech-Demo-Project.md) —— 包含 DDGI 康奈尔盒 (Cornell Box)、Apex 破坏测试平台、Apex 布料示例以及结合 DDGI 与 SSGI 的高端洞穴场景。
- [废弃公寓](../ProjectsAndDemos/Abandone-Apartment.md) 与 [阁楼场景](../ProjectsAndDemos/Attic-Scene.md) —— NVIDIA 原创的 RTGI 展示场景。
- [物理立方体基准测试](../ProjectsAndDemos/Physics-Cube-Bench.md) 与 [400 角色 CMC 基准测试](../ProjectsAndDemos/400-Characters-CMC-Bench.md)
  —— 本手册中所引用数据对应的基准测试场景。

完整列表请参阅[项目与演示](../ProjectsAndDemos.md)部分。


## 另请参阅

- [全局光照](../Rendering/Global-Illumination.md)
- [光线追踪](../Rendering/Ray-Tracing.md)
- [引擎默认设置变更](../Performance/Engine-Defaults.md)
- [控制台变量参考](../Reference/Console-Variables.md)
- [从 Unreal Engine 5 迁移](Migrating-From-UE5.md)
