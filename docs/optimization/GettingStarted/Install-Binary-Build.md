# 安装二进制构建

解压 `UE_ViteFork.7z`，运行 `RegistryAdd.bat` 注册引擎，运行 `MakeShortcut.bat` 创建桌面启动快捷方式，然后在您的 `.uproject` 文件中设置 `"EngineAssociation": "UE_ViteFork"`。

“已安装版本”是指编辑器的一种预编译、可分发的副本。它的运行方式与通过 Epic Games Launcher 安装的版本相同：您可以打开并打包（Cook）项目，但无法修改引擎源码或进行源码级调试。对于美术师、设计师以及任何想要评估此分支版本的人员来说，这是理想的选择。

“已安装版本”是由团队基于源码构建版本生成的。如果您需要自行制作，请参阅[已安装版本](../Tools/Installed-Builds.md)相关文档。


## 构建归档文件的内容

打包后的 Vite 构建文件解压到一个包含以下内容的文件夹中：

| 项 | 用途 |
|---|---|
| `Engine\` | 引擎本身，包括 `Engine\Binaries\Win64\UE4Editor.exe` |
| `MakeShortcut.bat` | 创建指向编辑器可执行文件的 `UEViteFork.lnk` 文件 |
| `RegistryAdd.bat` | 将该文件夹注册为 `UEViteFork` 引擎关联文件 |
| `RegistryRemove.bat` | 移除该注册 |
| `Templates\` | 项目模板（如果构建版本包含模板） |
| `FeaturePacks\` | 功能包（如果构建版本包含功能包） |
| `Samples\` | 示例内容（如果构建版本包含示例内容） |

精简版构建会省略模板、功能包、示例和调试符号。部分构建会附带一个名为 `ExcludedPlugins.7z` 的文件，其中包含为了保持主压缩包体积较小而从主压缩包中分离出来的插件；如果您需要这些插件，请将其解压到同一文件夹。


## 安装

### 安装并注册已安装的构建

1. 将 `UE_ViteFork.7z` 解压到不包含空格或非 ASCII 字符的路径，例如 `D:\Engines\UE_ViteFork`。避免解压到 `Program Files` 文件夹；编辑器会写入到它自己的目录，而且权限提示很麻烦。

2. 如果构建版本中包含单独的 `ExcludedPlugins.7z` 文件，并且您需要这些插件，请将其解压到同一文件夹。

3. 运行 `RegistryAdd.bat`。此操作会将引擎路径写入 `HKCU\Software\Epic Games\Unreal Engine\Builds`，并命名为 `UE_ViteFork`，同时删除指向同一文件夹的过时 GUID 键值条目。

4. 运行 `MakeShortcut.bat` 在引擎文件夹旁边创建 `UEViteFork.lnk` 文件。

5. 通过快捷方式启动编辑器，或直接运行 `Engine\Binaries\Win64\UE4Editor.exe`。

**警告：** 注册是按用户进行的，使用 HKCU 注册表。每个需要引擎的 Windows 用户帐户都必须运行一次 `RegistryAdd.bat` 文件。


## 将项目指向构建

在文本编辑器中打开您的 `.uproject` 文件并设置引擎关联：

```json
{
    "FileVersion": 3,
    "EngineAssociation": "UE_ViteFork",
    "Category": "",
    "Description": ""
}
```

已安装的构建归档文件中打包的项目已设置此项。如果您右键单击 `.uproject` 文件并选择**Switch Unreal Engine version（切换虚幻引擎版本）**，则在构建注册后，UE_ViteFork 将出现在列表中。


## 移动或删除构建

注册表项存储的是绝对路径，因此移动引擎文件夹会破坏它。如果您移动构建：

1. 从原位置运行 `RegistryRemove.bat`，或手动删除 `UE_ViteFork` 值。

2. 移动文件夹。

3. 从新位置运行 `RegistryAdd.bat`。


要彻底卸载，请运行 `RegistryRemove.bat`，然后删除文件夹。除了 `%LOCALAPPDATA%\UnrealEngine` 和 `%APPDATA%\Unreal Engine` 下的用户专属 Unreal 目录之外，引擎目录和单个 HKCU 注册表值之外不会写入任何内容。


## 同时运行源码构建版本与安装版构建版本

你可以同时安装这两个版本。只要它们以不同的名称进行注册，便属于独立的引擎关联；因此，如果你希望同时使用源码构建版本和已安装版本，请在注册前修改其中一个版本的 `ENGINE_ASSOCIATION` 值。请注意，这两个版本共用同一用户级的 Unreal 目录，因此编辑器布局及部分设置将在两者间通用。

## 另请参阅

- [入门指南](Getting-Started.md)
- [已安装版本](../Tools/Installed-Builds.md)
- [打包与分发](../Tools/Installed-Builds.md)
- [创建您的第一个项目](First-Project.md)
