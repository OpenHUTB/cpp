# 入门


!!! 最快方法

   下载已安装的二进制版本，运行 <code>RegistryAdd.bat</code>，然后打开编辑器。在现代桌面 CPU 上，从源代码构建大约需要 15-30 分钟的编译时间，并且需要固定的工具链。


本节将带您从不安装任何东西到运行编辑器和虚幻引擎 Vite 上的第一个项目。如果您还没有确定分叉是否适合您的项目，请先阅读[Vite简介](Introduction-to-Vite.md)。

## 选择安装方式

Vite 交付有两种形式。它们在功能上是相同的编辑器；区别在于你是否自己编译引擎。

| | 安装的二进制版本 | 源码构建 |
|---|---|---|
| 设置时间 | 分钟 | 15–30 分钟编译，加上依赖项下载 |
| 磁盘占用空间 | 更小，无中间文件 | 更大，包括完整源代码和中间文件 |
| 可以修改引擎代码 | 不可以 | 可以 |
| 可以调试到引擎代码 | 仅附带符号 | 可以 |
| 推荐用于 | 艺术家、设计师、评价 | 程序员，任何为分支做贡献的人 |

大多数团队都运行混合设置：工程师负责源构建，其他人则负责从同一提交生成的已安装构建。请参阅 [Installed Builds](Installed-Builds.md) 了解如何生成一个。

## 步骤按顺序

1. 检查[系统要求](System-Requirements.md)，以便了解您需要什么硬件和磁盘空间。
2. 安装[工具链要求](Toolchain-Requirements.md)。此步骤是强制性且不可原谅的 -错误的 MSVC 或 Windows SDK 版本会产生令人困惑的编译错误，而不是清晰的消息。
3. [安装二进制构建](Install-Binary-Build.md) 或 [从源代码构建](Build-From-Source.md)。
4. 创建您的[第一个项目](First-Project.md)。
5. 如果您要引入现有内容，请阅读[从虚幻引擎 5 迁移](Migrating-From-UE5.md)。

如果在此过程中出现任何问题，[构建故障排除](Build-Troubleshooting.md) 会涵盖最常出现的错误，包括表示工具链不匹配的`C4668: '__has_feature' is not defined`错误。

## 与原版虚幻引擎 4.27 有何不同

如果您以前从源代码构建过虚幻引擎，那么三件事会让您感到惊讶。

**GitDeps 已修补**。Stock 4.27 无法下载依赖项，因为 CDN 端点已移动。 Vite 提供了修复程序，因此“Setup.bat”无需手动编辑即可工作。当“Setup.bat”询问是否覆盖本地更改时，回答“N”。

**默认情况下，光线追踪处于开启状态**。新项目支持开箱即用的光线追踪阴影、反射、半透明和环境光遮挡。这是故意的，以便可以发现这些功能，但这意味着新的空项目比库存 4.27 项目重。请参阅[光线追踪](Ray-Tracing.md)了解如何关闭个别效果。

**为了提高性能，更改了多个引擎默认值**。重叠事件、骨架网格物体设置、光照贴图 UV 生成和许多插件与库存不同。这些更改会影响游戏行为，而不仅仅是帧时间，因此请在发布之前阅读[引擎默认更改](Engine-Defaults.md)。

## 虚幻引擎基础知识

本手册记录了分支，而不是其下方的引擎。 Vite 不会改变的所有内容都按照虚幻引擎 4.27 的工作方式工作，Epic 自己的 4.27 文档是它的正确参考：

| 话题 | Epic 4.27 的文档 |
|---|---|
| 项目、模板和目录结构 | [使用虚幻项目和模板](https://dev.epicgames.com/documentation/unreal-engine/working-with-unreal-projects-and-templates?application_version=4.27) |
| 设置 Visual Studio 进行源代码构建 | [设置 Visual Studio](https://dev.epicgames.com/documentation/en-us/unreal-engine/setting-up-visual-studio-development-environment-for-cplusplus-projects-in-unreal-engine?application_version=4.27) |
| 蓝图和游戏框架 | [虚幻引擎 4.27 文档](https://dev.epicgames.com/documentation/en-us/unreal-engine/unreal-engine-4-27-documentation) |

将版本选择器保持在 4.27。 UE5 文档描述了 Vite 故意不具备的系统，对于来自 UE5 的人们来说，遵循它是一个常见的困惑源。

## 参见

- [Vite 简介](Introduction-to-Vite.md)
- [工具链要求](Toolchain-Requirements.md)
- [Vite 设置助手](ViteSetup.md)
- [引擎默认更改](Engine-Defaults.md)
