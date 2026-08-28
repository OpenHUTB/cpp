# 平台

Vite 主要针对的是基于 DirectX 12 的 Windows 64 位平台；其他平台则沿用 4.27 版本的支持。

Vite 是一个专注于特定方向的分支版本。其核心特色功能——包括 NvRTX 光线追踪套件、DDGI、RTXDI、DLSS 及其替代技术、HBAO+ 以及硬件曲面细分（Hardware Tessellation）——均属于延迟渲染管线（Deferred Path）下的 GPU 特性，主要面向桌面平台及第九世代主机。

## 支持层次

| 平台 | 层次 | 说明                                                                  |
|---|---|------------------------------------------------------------------------|
| Windows 64 位 | **主要平台** | 构建与评估的基准平台                          |
| Windows 32 位 | 仅支持文件 | Win64工具链所需；非最终发布目标                       |
| Linux | 继承支持 | 支持原生4.27功能。光线追踪套件未经测试。                        |
| Mac | 继承支持 | 支持原生 4.27 功能。不支持 DXR，因此光线追踪套件不适用。   |
| Android | 继承支持 | 继承了针对前向渲染路径（Forward Path）的Vite更新。                                 |
| iOS / tvOS | 继承支持 | 继承了针对前向渲染路径（Forward Path）的Vite更新。                                 |
| HTML5 | 继承支持 | 支持原生 4.27 功能；上游版本早已弃用                           |
| 控制台 | 未处理 | 需要平台 SDK 支持 Vite |

“继承支持”（Inherited）意味着保留了原有的 4.27 代码且未被刻意破坏，但 Vite 不会对其进行测试，且该分支特有的渲染功能在其中无法使用或未经验证。


## 启用其他平台

平台依赖项可在设置时选择性启用。

通过 [ViteSetup](../Tools/ViteSetup.md)，您可以选择 **Win64 + Android** 或 **Win64 + Linux** 配置方案，也可以使用分步向导单独切换平台。若选择**完整设置**，程序将不会向 `Setup.bat` 传递 `-exclude` 参数，从而下载所有内容。

对于精简脚本（debloat scripts），请取消 `devops\config.txt` 中相关行的注释：

```ini
# Win64 is always kept. Uncomment a platform to ALSO download and keep
# its dependencies during setup.
#KEEP_PLATFORM=Android
#KEEP_PLATFORM=IOS
#KEEP_PLATFORM=Linux
#KEEP_PLATFORM=Mac
#KEEP_PLATFORM=HTML5
```

**警告：** 如果事后才添加平台，就需要重新进行依赖项设置并重新构建。如果可能，请在首次构建之前就确定好。


## 图形 API

| API | 状态 | 笔记 |
|---|---|---|
| DirectX 12 | **主要** | DXR（以及所有光线追踪功能）运行所必需 |
| DirectX 11 | 已支持 | 支持 HBAO+；不支持光线追踪 |
| Vulkan | 继承支持 | 原生支持 4.27 版本；Vite 的光线追踪套件基于 DXR |
| Metal | 继承支持 | 仅限 Mac |
| OpenGL ES | 继承支持 | 移动平台 |

### 需要 DX12 什么

所有光线追踪功能都需要 DXR 支持，在 Vite 中这意味着必须使用 DirectX 12：

- [光线追踪反射](../Rendering/RT-Reflections.md)、阴影、环境光遮蔽（AO）和天光（Sky Light）
- [动态 DDGI](../Rendering/DDGI-Dynamic.md) 和逐像素光线追踪全局光照（GI）
- [RTXDI](../Rendering/RTXDI.md) 和 [路径追踪](../Rendering/Path-Tracing.md) ——前提是使用 `VITE_RT_PSO_DEBLOAT=0` 重新构建
- [半透明效果和焦散](../Rendering/RT-Translucency-And-Caustics.md) ——同样需要上述条件

[FSR 4](../Rendering/Upscalers.md) 也仅支持 DX12，与光线追踪功能无关。

### 可在 DX11 下运行的功能

- [HBAO4+](../Rendering/Ambient-Occlusion.md) —— 尽管控制台变量的帮助文本提示其仅支持 DX11，但它实际上也有 D3D12 实现版本
- [SMAA](../Rendering/Anti-Aliasing.md)
- [SSGI](SSGI.md)
- [硬件曲面细分](../Rendering/Tessellation.md)
- [Callisto BRDF 和卡通渲染（Toon shading）模型](../Rendering/Shading-Models.md)

如果愿意放弃光线追踪，选择 DX11 目标是可行的。对于那些[性能](../EngineOverview/Performance-Targets.md)预算无法支撑 DXR 开销的项目来说，这是一个合适的选择。


## 硬件

请参阅[系统要求](../GettingStarted/System-Requirements.md)以了解开发和运行时所需的硬件。


## 打包

[安装版构建](../Tools/Installed-Builds.md)是针对 Win64 平台，使用“开发（Development）”和“发布（Shipping）”游戏配置生成的。如需其他配置或平台，请编辑 `RunUAT.bat` 的 BuildGraph 参数：


```
-set:WithLinux=true
-set:GameConfigurations=Development;Test;Shipping
```

每增加一个平台或配置，都会使原本就很漫长的构建过程耗时大幅增加。


## 另请参阅

- [系统要求](../GettingStarted/System-Requirements.md)
- [已安装好的构建](../Tools/Installed-Builds.md)
- [ViteSetup 助手](../Tools/ViteSetup.md)
- [光线追踪](../Rendering/Ray-Tracing.md)
- [精简指南](../Performance/Debloat-Guide.md)
