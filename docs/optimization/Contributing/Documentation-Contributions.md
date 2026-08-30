# 文档贡献

这些文档是 JetBrains Writerside 项目的一部分。请在 `Writerside\topics\` 目录下添加一个 Markdown 文件，将其注册到 `Writerside\hi.tree` 文件中，然后推送到 `main` 分支——GitHub Actions 会构建并部署到 GitHub Pages。

文档滞后于引擎更新比没有文档更糟糕，因为用户会信任文档。如果您更改了默认值、添加了控制台变量或编译了某个功能，则描述该功能的页面也需要在同一过程中进行更改。


## 仓库布局

| 路径 | 内容 |
|---|---|
| `Writerside\topics\` | 所有主题 Markdown 文件，按章节分类整理在子文件夹中 |
| `Writerside\images\` | 图片，仅通过文件名引用 |
| `Writerside\hi.tree` | 导航树；未在此处列出的主题将不会发布 |
| `Writerside\writerside.cfg` | 项目配置 |
| `Writerside\v.list` | 变量 |
| `Writerside\c.list` | 分类 |
| `.github\workflows\DocsBuild.yml` | 构建和部署流程 |

## 添加主题

### 添加文档页面

1. 在相应的 `Writerside\topics\` 子文件夹中创建 Markdown 文件。使用 `Title-Case-With-Hyphens.md`。

2. 在 `Writerside\hi.tree` 的右侧部分添加一个 `<toc-element topic="Your-File.md"/>` 条目。

3. 添加交叉链接。将其添加到父级部分页面的主题表以及相关页面的`另请参阅`列表中。

4. 推送到`main`分支。工作流将自动构建和部署。


**注意：** Writerside 通过文件名而非路径解析主题链接，因此[光线追踪](../Rendering/Ray-Tracing.md)可以正常工作，无论文件位于哪个文件夹中。因此，文件名在整个项目中必须唯一。


## 写作规范

### 结构

大多数页面遵循相同的结构：

1. `# 标题`
2. 一个 `<tldr>` 块，用两三句话概括答案
3. 正文部分
4. 一个 `## 另请参阅` 列表

在篇幅较长的页面上，`<tldr>` 块必不可少。通过搜索找到该页面的读者需要在一句话内确认自己是否找到了正确的内容。


### Writerside 元素

| 元素 | 用途 |
|---|---|
| `<tldr>` | 顶部摘要 |
| `<note>` | 有用的上下文信息，不会造成危害 |
| `<warning>` | 会浪费读者时间或导致构建失败的内容 |
| `<procedure>` | 带有 id 属性的编号步骤 |
| `<deflist>` / `<def>` | 术语和解释列表 |
| `<code-block lang="ini">` | 类似 HTML 元素内的代码，其中围栏式代码块不起作用 |

在 `<tldr>`、`<note>`、`<warning>` 和 `<def>` 标签内，请使用 HTML 而不是 Markdown，例如 `<b>`、`<code>`、`<a href="Page.md">` 和 `<p>`。这些标签内不会处理 Markdown 语法。


### 语气

- 先陈述答案，再解释原因。不要反过来。
- 说明某项功能的成本，而不仅仅是它的功能。
- 如果某项功能默认情况下不可用，请在页面顶部说明，而不是在脚注中。[RTXDI](../Rendering/RTXDI.md) 和[路径追踪](../Rendering/Path-Tracing.md)页面是可遵循的模式。
- 用表格列出可列举的事实，用散文进行解释。不要在表格单元格中编写解释。

### 准确性

在引擎源代码中查找依据。控制台变量名、默认值和帮助文本应对照其定义进行检查，而不是死记硬背。

```
Engine\Source\Runtime\Renderer\Private\        renderer CVars
Engine\Source\Runtime\Engine\Classes\Engine\Scene.h    post-process settings
Engine\Source\Runtime\Core\Public\Misc\CoreDefines.h   VITE_* compile switches
```

**警告：** 引擎中的控制台变量帮助文本有时会过时。HBAO+ 在其帮助字符串中将自身描述为仅支持 DX11，但实际上它有一个可用的 D3D12 实现。当源代码和帮助文本不一致时，请以源代码为准，并记录差异。


## 本地构建

为 JetBrains IDE 安装 Writerside 插件，或安装 Writerside 独立版，然后打开仓库根目录。实例 ID 为 `hi`。

Writerside 自身的检查功能可以捕获损坏的主题链接、未引用的主题和缺失的图像，这比部署后再发现这些问题要快得多。


## 构建流水线

`.github\workflows\DocsBuild.yml` 文件会在每次推送到主分支以及手动分发时运行。它使用 Writerside Docker 构建器构建 hi 实例，上传构建产物，然后部署到 GitHub Pages。

构建失败通常意味着 `hi.tree` 中引用的主题不存在，或者主题存在但未被引用。这两种情况都会在工作流日志中按名称报告。


## 记录引擎变更

如果您的引擎变更影响到以下任何内容，请在同一拉取请求中更新文档：

| 更改 | 待更新页面 |
|---|---|
| 新增或已更改的控制台变量 | 相关功能页面 |
| 新增 `VITE_*` 开关 | [编译时开关](../Performance/Compile-Time-Switches.md) |
| 已更改的引擎默认值 | [引擎默认值](../Performance/Engine-Defaults.md) |
| 新增或已更新的插件 | [已捆绑的插件](../Plugins/Bundled-Plugins.md) |
| 新增工具或脚本 | [工具部分](../Tools/Tools.md) section |
| 默认版本中不再提供的功能 | 该功能的页面，以及[光线追踪](../Rendering/Ray-Tracing.md)（如适用） |

## 另请参阅

- [贡献指南](Contributing.md)
- [编码规范](Coding-Guidelines.md)
- [提交约定](Commit-Conventions.md)
