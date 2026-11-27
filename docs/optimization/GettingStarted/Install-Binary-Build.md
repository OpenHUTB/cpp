# 安装二进制构建

解压 `UE_ViteFork.7z`，运行 `RegistryAdd.bat` 注册引擎，运行 `MakeShortcut.bat` 创建桌面启动快捷方式，然后在您的 `.uproject` 文件中设置 `"EngineAssociation": "UE_ViteFork"`。

「已安装版本」是编辑器的一份预编译、可分发包。它和通过 Epic Games Launcher 安装的版本用法相同：可以打开项目、烘焙资源（Cook）并打包，但无法修改引擎源码，也不能做源码级调试。美术、设计师以及任何想评估这个分支的人，都适合用它。

「已安装版本」由团队基于源码构建生成。如需自行制作，见[已安装版本](../Tools/Installed-Builds.md)。


## 构建归档包含的内容

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

1. 把 `UE_ViteFork.7z` 解压到不含空格或非 ASCII 字符的路径，例如 `D:\Engines\UE_ViteFork`。不要解压到 `Program Files`：编辑器会往自己的目录写文件，权限提示很麻烦。

2. 如果构建版本中包含单独的 `ExcludedPlugins.7z` 文件，并且您需要这些插件，请将其解压到同一文件夹。

3. 运行 `RegistryAdd.bat`。它会把引擎路径写入 `HKCU\Software\Epic Games\Unreal Engine\Builds`，注册名为 `UE_ViteFork`，同时清理指向同一文件夹的过时 GUID 条目。

4. 运行 `MakeShortcut.bat` 在引擎文件夹旁边创建 `UEViteFork.lnk` 文件。

5. 通过快捷方式启动编辑器，或直接运行 `Engine\Binaries\Win64\UE4Editor.exe`。

!!! warning
    注册是按用户进行的，写在 HKCU 注册表里。每个需要这个引擎的 Windows 用户账户都要运行一次 `RegistryAdd.bat`。


## 让项目指向该构建

在文本编辑器中打开您的 `.uproject` 文件并设置引擎关联：

```json
{
    "FileVersion": 3,
    "EngineAssociation": "UE_ViteFork",
    "Category": "",
    "Description": ""
}
```

构建归档里自带的项目已经设好这一项。右键单击 `.uproject` 文件并选择 **Switch Unreal Engine version（切换虚幻引擎版本）**，注册完成后 `UE_ViteFork` 就会出现在列表里。


## 移动或删除构建

注册表项存的是绝对路径，移动引擎文件夹会使其失效。如果确实要移动：

1. 从原位置运行 `RegistryRemove.bat`，或手动删除 `UE_ViteFork` 值。

2. 移动文件夹。

3. 从新位置运行 `RegistryAdd.bat`。


要彻底卸载，先运行 `RegistryRemove.bat`，再删除文件夹。除此之外，只会写入引擎目录、一个 HKCU 注册表值，以及 `%LOCALAPPDATA%\UnrealEngine` 和 `%APPDATA%\Unreal Engine` 下的用户级 Unreal 目录。


## 同时运行源码构建与已安装构建

两个版本可以同时安装。只要注册名不同，它们就是独立的引擎关联；所以若想并用源码构建与已安装构建，请在注册前改掉其中一个的 `ENGINE_ASSOCIATION` 值。注意两者共享同一用户级 Unreal 目录，编辑器布局和部分设置会通用。

## 另请参阅

- [入门指南](Getting-Started.md)
- [已安装版本](../Tools/Installed-Builds.md)
- [打包与分发](../Tools/Installed-Builds.md)
- [创建您的第一个项目](First-Project.md)
