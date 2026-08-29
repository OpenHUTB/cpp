# 工具和自动化

Vite 提供了涵盖整个引擎生命周期的批处理工具：用于设置与构建的 `ViteSetup.bat`，用于已安装构建版本的 `RunUAT.bat`，用于打包与注册的 `LocalBuilds` 脚本，以及用于处理缓存失效情况的 `WipeShaderCache.bat`。

从源码构建 Unreal 涉及一系列漫长的步骤，且每个步骤都可能出现各种故障。Vite 将这一系列流程封装在工具中，使得在执行常规操作时，无需记忆具体的命令行指令。


## 本节内容

| 主题 | 涵盖内容 |
|---|---|
| [ViteSetup 助手](./ViteSetup.md) | 引导式设置与构建助手及其菜单模式 |
| [已安装的构建版本](./Installed-Builds.md) | 生成并打包可分发的二进制引擎 |
| [缓存管理](./Cache-Management.md) | 在出现问题时清除着色器（Shader）和 DDC 缓存 |

## 工具概览

| 工具 | 位置 | 用途 |
|---|---|---|
| `ViteSetup.bat` | 引擎根目录 | 引导式九步设置，或包含各项独立操作的菜单 |
| `RunUAT.bat` | 引擎根目录 | 通过 BuildGraph 构建已安装版本的引擎 |
| `WipeShaderCache.bat` | 引擎根目录 | 清除引擎级的着色器（shader）和 DDC 缓存 |
| `LocalBuilds\CompressBuild*.bat` | `LocalBuilds\` | 将已暂存（staged）的安装版构建打包为 7z 压缩包 |
| `LocalBuilds\RegistryAdd.bat` | `LocalBuilds\` | 将该构建版本注册为 `UEViteFork` |
| `LocalBuilds\RegistryRemove.bat` | `LocalBuilds\` | 取消注册该构建版本 |
| `LocalBuilds\MakeShortcut.bat` | `LocalBuilds\` | 创建 `UEViteFork.lnk` 快捷方式  |
| `devops\ueVite-debloat-*.bat` | `devops\` | 引擎精简工具集 —— 参阅[精简指南](../Performance/Debloat-Guide.md)  |

## 你想要哪个

* 我刚克隆了该仓库。

    请运行 `ViteSetup.bat`。它会引导你完成从环境检查到启动编辑器的全过程。请参阅 [ViteSetup 助手](./ViteSetup.md)。


* 我需要一个可交付给美术人员使用的二进制版本引擎。

    运行 `ViteSetup.bat` 并在第 4 步选择二进制构建选项，或者直接运行 `RunUAT.bat` 及 `LocalBuilds\` 目录下的脚本。请参阅[已安装构建版本](./Installed-Builds.md)相关说明。


* 修改后着色器表现异常

    运行 `WipeShaderCache.bat`。请参阅[缓存管理](./Cache-Management.md)。


* 该引擎占用了过多磁盘空间。

    运行 `devops\debloat` 套件。请参阅 [精简指南](../Performance/Debloat-Guide.md)。

* 我只需要重新生成项目文件。

    在 `ViteSetup.bat` 菜单中选择选项 3，或者直接运行 `GenerateProjectFiles.bat`。


## 另请参阅

- [从源码构建](../GettingStarted/Build-From-Source.md)
- [工具链要求](../GettingStarted/Toolchain-Requirements.md)
- [构建故障排查](../GettingStarted/Build-Troubleshooting.md)
- [精简指南](../Performance/Debloat-Guide.md)
