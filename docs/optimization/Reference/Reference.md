# 参考

查找资料：控制台变量、术语和常见问题解答。如果您知道要查找的内容，请使用这些资料；如果您不确定，请使用功能章节。

## 本节内容

| 主题 | 涵盖内容 |
|---|---|
| [控制台变量](./Console-Variables.md) | Vite 特有的常用控制台变量，按子系统分组 |
| [编译时开关](../Performance/Compile-Time-Switches.md) | `VITE_*` 构建开关及其作用 |
| [术语表](./Glossary.md) | 本文档中使用的术语 |
| [常见问题](./FAQ.md) | 反复出现的问题 |

## 快速解答

* 我设置的控制台变量不起作用

    很可能是该功能被编译掉了。`VITE_RT_PSO_DEBLOAT` 默认值为 `1`，它会移除 RTXDI、路径追踪、光线追踪半透明、焦散和逐像素光线追踪全局光照的着色器排列组合。变量仍然设置着，但没有任何渲染效果。请参阅[编译时开关](../Performance/Compile-Time-Switches.md)。

* 编辑器中缺少某个功能

    检查其插件是否已启用。大多数捆绑插件默认处于关闭状态——请参阅[已捆绑的插件](../Plugins/Bundled-Plugins.md)。

* 全新克隆构建失败

    通常是工具链不匹配或缺少 .NET Framework 4.5 引用程序集。请参阅[构建故障排除](../GettingStarted/Build-Troubleshooting.md)。

* 着色器表现得好像我的更改没有生效一样

    缓存过期。请参阅[缓存管理](../Tools/Cache-Management.md)。


## See also

- [入门](../GettingStarted/Getting-Started.md)
- [性能和优化](../Performance/Performance.md)
- [贡献](../Contributing/Contributing.md)
