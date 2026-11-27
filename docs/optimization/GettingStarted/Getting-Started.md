# 入门

!!! tip "最快方法"
    下载已安装的二进制版本，运行 `RegistryAdd.bat`，然后打开编辑器。在现代桌面 CPU 上，从源码构建需要约 15–30 分钟编译时间，并且依赖固定的工具链。


本节带你从零起步，直到运行编辑器并在虚幻引擎 Vite 上跑起第一个项目。如果还没确定这个分支是否适合你的项目，请先读 [Vite 简介](Introduction-to-Vite.md)。

## 选择安装方式

Vite 有两种交付形式。两者是功能相同的编辑器，区别只在于你是否自己编译引擎。

| | 安装的二进制版本 | 源码构建 |
|---|---|---|
| 设置时间 | 分钟 | 15–30 分钟编译，加上依赖项下载 |
| 磁盘占用空间 | 更小，无中间文件 | 更大，包括完整源代码和中间文件 |
| 可以修改引擎代码 | 不可以 | 可以 |
| 可以调试到引擎代码 | 仅附带符号 | 可以 |
| 推荐用于 | 美术、设计师、评估 | 程序员，以及任何为分支做贡献的人 |

大多数团队采用混合配置：工程师用源码构建，其他人用从同一提交生成的已安装构建。请参阅[已安装的构建](../Tools/Installed-Builds.md)了解如何生成一个。

## 按顺序操作

1. 检查[系统要求](System-Requirements.md)，以便了解您需要什么硬件和磁盘空间。
2. 安装[工具链要求](Toolchain-Requirements.md)。这一步是强制且不留情面的——MSVC 或 Windows SDK 版本不对，得到的是令人困惑的编译错误，而不是清晰提示。
3. [安装二进制版本](Install-Binary-Build.md) 或 [从源代码构建](Build-From-Source.md)。
4. 创建您的[第一个项目](First-Project.md)。
5. 如果您要引入现有内容，请阅读[从虚幻引擎 5 迁移](Migrating-From-UE5.md)。

如果在此过程中出现任何问题，[构建故障排除](Build-Troubleshooting.md) 会涵盖最常出现的错误，包括表示工具链不匹配的`C4668: '__has_feature' is not defined`错误。

## 与原生虚幻引擎 4.27 有何不同

如果你以前从源码构建过虚幻引擎，有三件事会让你意外。

**GitDeps 已修复。** 原生 4.27 因为 CDN 端点迁移而无法下载依赖；Vite 已带上修复，`Setup.bat` 无需手工修改即可运行。当 `Setup.bat` 询问是否覆盖本地更改时，回答 `N`。

**光线追踪默认开启。** 新项目开箱即支持光线追踪阴影、反射、半透明与环境光遮蔽。这是有意为之，好让用户能直接发现这些功能，但也意味着新建空项目比原生 4.27 项目更重。关闭单项效果的方法见[光线追踪](../Rendering/Ray-Tracing.md)。

**为性能调整了多项引擎默认值。** 重叠事件、骨骼网格体设置、光照贴图 UV 生成以及许多插件都与原生版本不同。这些改动会影响游戏行为，而不只是帧时间，发布前请阅读[引擎默认设置变更](../Performance/Engine-Defaults.md)。

## 虚幻引擎基础知识

本手册记录的是这个分支，而不是它下面的引擎。Vite 没有改动的内容，行为都等同虚幻引擎 4.27，应参考 Epic 自己的 4.27 文档：

| 话题 | Epic 4.27 的文档 |
|---|---|
| 项目、模板和目录结构 | [使用虚幻项目和模板](https://dev.epicgames.com/documentation/unreal-engine/working-with-unreal-projects-and-templates?application_version=4.27) |
| 设置 Visual Studio 进行源代码构建 | [设置 Visual Studio](https://dev.epicgames.com/documentation/en-us/unreal-engine/setting-up-visual-studio-development-environment-for-cplusplus-projects-in-unreal-engine?application_version=4.27) |
| 蓝图和游戏框架 | [虚幻引擎 4.27 文档](https://dev.epicgames.com/documentation/en-us/unreal-engine/unreal-engine-4-27-documentation) |

请把文档版本选择器保持在 4.27。UE5 文档描述的是 Vite 有意不采用的系统，从 UE5 过来的人若照它操作，常常会困惑。

## 另请参阅

- [Vite 简介](Introduction-to-Vite.md)
- [工具链要求](Toolchain-Requirements.md)
- [Vite 设置助手](../Tools/ViteSetup.md)
- [引擎默认更改](../Performance/Engine-Defaults.md)
