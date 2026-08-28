# 已安装的构建


“已安装版本”（Installed Build）是一种预编译、可重新分发的引擎版本，其运作方式与通过 Epic Games Launcher 安装的引擎相同。`RunUAT.bat` 负责生成该版本，而 `LocalBuilds` 脚本则负责对其进行打包和注册。


相比之下，构建“源码版本”需要完整的工具链、漫长的编译过程以及大量的磁盘空间；而“已安装版本”则无需这些——美术师、设计师以及任何无需修改引擎 C++ 代码的人员均可直接使用。


## 构建过程

位于引擎根目录下的 `RunUAT.bat` 负责执行 BuildGraph 的已安装版本构建目标：

```shell
Engine\Build\BatchFiles\RunUAT.bat BuildGraph
    -script="Engine\Build\InstalledEngineBuild.xml"
    -target="Make Installed Build Win64"
    -nosign
    -set:GameConfigurations=Development;Shipping
    -set:WithWin64=true
    -set:WithWin32=false
    -set:WithMac=false
    -set:WithAndroid=false
    -set:WithIOS=false
    -set:WithTVOS=false
    -set:WithLinux=false
    -set:WithLinuxAArch64=false
    -set:WithDDC=false
    -clean
```

该配置仅针对 Win64 平台，包含 Development（开发）和 Shipping（发布）两种游戏配置，这与 Vite 的[平台侧重点](../Platforms/Platforms.md)相一致。设置 `WithDDC=false` 可跳过派生数据缓存（DDC）的生成，从而大幅缩短构建时间，但代价是最终用户在首次启动时需要进行着色器编译。


您也可以通过 [ViteSetup](./ViteSetup.md) 进行此操作——具体路径为步骤 4 中的二进制选项，或菜单中的选项 8。ViteSetup 采用增量构建方式，而默认提供的 `RunUAT.bat` 脚本则会传入 `-clean` 参数。


**注意：** 这是一个耗时很长的构建过程。请预留数小时（而非数分钟）的时间，并确保有充足的可用磁盘空间，因为中间产物和最终输出文件都非常大。

输出文件将生成在 `LocalBuilds\Engine\Windows` 目录下。


## 打包

`LocalBuilds\` 目录下的四个脚本使用 7-Zip 压缩已暂存（staged）的构建版本。这些脚本均要求 `LocalBuilds\Engine\Windows` 包含该已暂存的构建版本，并且默认在 `C:\Program Files\7-Zip\7z.exe` 查找 7-Zip，除非 `SEVEN_ZIP` 环境变量指向其他位置。


| 脚本 | 调试符号 | 插件 |
|---|---|---|
| `CompressBuild.bat` | 按 `ExcludedPdbs.txt` 排除 | 单个压缩包 |
| `CompressBuildSeparate.bat` | 按 `ExcludedPdbs.txt` 排除 | 拆分为第二个压缩包 |
| `CompressBuildSymbols.bat` | 按 `ExcludedPdbs.txt` 排除 | 单个压缩包 |
| `CompressBuildSymbolsSeparate.bat` | **包含** | 拆分为第二个压缩包 |

**注意：** `CompressBuildSymbols.bat` 目前与 `CompressBuild.bat` 完全相同——两者均排除了 PDB 文件。如果归档文件中需要包含符号文件，请使用 `CompressBuildSymbolsSeparate.bat`；这是唯一一个不传递 `-x@ExcludedPdbs.txt` 参数的脚本。


这四者均将 `FeaturePacks\`、`Samples\` 和 `Templates\` 从主归档文件中排除。

输出:

| 文件 | 内容 |
|---|---|
| `LocalBuilds\Engine\UE_ViteFork.7z` | 引擎 |
| `LocalBuilds\Engine\ExcludedPlugins.7z` | 插件，仅限“独立（Separate）”变体 |

### 排除列表

`ExcludedPdbs.txt` 列出了约 1,100 个 PDB 文件路径，涵盖了引擎模块和插件。调试符号占源码构建版本体积的很大一部分，因此将其排除是实现打包体积缩减的最主要手段。

`ExcludedPlugins.txt` 使用的是与[精简工具集](../Performance/Debloat-Guide.md)相同的列表。在“分离变体”（Separate variants）模式下，插件会被打包到独立的归档文件中，而不是直接被移除——用户只需解压基础引擎，仅在需要时才添加插件归档包。


### 为什么要分离插件？

如果团队中大多数人只需要基础引擎，而只有少数人需要完整的插件集，那么就可以分发体积小得多的主下载包。这种方式是通过归档分离而非直接删除来实现的，因此不会丢失任何内容。


## 在目标机器上安装

### 安装打包好的 Vite 构建版本

1. 将 `UE_ViteFork.7z` 解压至最终存放位置。若事后移动该文件夹，则需要重新进行注册。

2. 如果您收到了 `ExcludedPlugins.7z` 且需要其中的插件，请将其解压并覆盖至同一文件夹中。

3. 在解压后的根目录下运行 `RegistryAdd.bat`。此操作会将引擎路径写入注册表项 `HKCU\Software\Epic Games\Unreal Engine\Builds`，并命名为 `UEViteFork`。

4. 运行 `MakeShortcut.bat`，创建指向编辑器的快捷方式 `UEViteFork.lnk`。

5. 右键点击 `.uproject`文件，选择 **Switch Unreal Engine version**（切换 Unreal Engine 版本），然后选择 **UEViteFork**。


打包后的归档根目录包含 `MakeShortcut.bat`、`RegistryAdd.bat` 和 `RegistryRemove.bat`，以及 `Engine\`、`FeaturePacks\`、`Templates\` 和 `Samples\`（如果在构建时已包含）。



## 移动或移除注册表

注册表存储的是绝对路径，因此移动构建会导致注册表失效。

### 移动已安装的构建

1. 在当前位置运行 `RegistryRemove.bat`。

2. 移动该文件夹。

3. 在新位置运行 `RegistryAdd.bat`。

4. 再次运行 `MakeShortcut.bat`；旧的快捷方式指向的是旧路径。

若要彻底移除，请运行 `RegistryRemove.bat`，并删除相应的快捷方式及文件夹。


## 与源码构建版本共存

两者可以共存，但它们注册时均使用相同的名称（`UEViteFork`），因此同一时间只能有一个处于注册状态。项目将识别并使用最近一次运行过 `RegistryAdd.bat` 的那个版本。

如果您需要频繁切换，请留意当前注册的是哪个版本，或者手动编辑注册表项，为它们指定不同的名称。


## 局限性

已安装的构建版本无法编译引擎自身的 C++ 代码。使用这些版本的项目仍可包含自己的 C++ 模块（这些模块针对已安装引擎的头文件和库进行编译），但若要修改引擎源码，则必须使用源码构建版本。

这同样适用于[编译时开关](../Performance/Compile-Time-Switches.md)。如果您的项目需要设置 `VITE_RT_PSO_DEBLOAT=0` 以支持路径追踪或 RTXDI，则该设置必须在生成已安装构建版本时就已包含在内。


## 另请参见

- [ViteSetup 助手](./ViteSetup.md)
- [安装二进制构建](../GettingStarted/Install-Binary-Build.md)
- [从源代码构建](../GettingStarted/Build-From-Source.md)
- [精简指南](../Performance/Debloat-Guide.md)
- [Compile-Time Switches](../Performance/Compile-Time-Switches.md)
