# 贡献


Vite 比普通的 Unreal 有更严格的规则：没有递归、没有新的虚拟、没有新的蓝图暴露、严格的 Clang 合规性和绝对的应用程序二进制接口（Application Binary Interface，ABI） 稳定性。在编写代码之前阅读指南，而不是在打开拉取请求之前。


Vite的存在是因为UE 4.27可以变得又快又稳定。妥协的贡献要么就失去了意义。下面的规则对于游戏引擎分支来说是非常严格的，这是故意的。

## 在这个部分

| 话题 | 覆盖内容 |
|---|---|
| [编码指南](Coding-Guidelines.md) | 核心原则、禁止结构、技术和性能规则 |
| [提交约定](Commit-Conventions.md) | 前缀、归属、分支卫生 |
| [向后移植](Backporting.md) | 将 UE5 和上游功能引入 4.27 |
| [文档](Documentation-Contributions.md) | 为这些文档做出贡献 |

## 开始之前

### 贡献工作流程

1. 阅读<a href="Coding-Guidelines.md">编码指南</a>。几种常见的 C++ 模式被彻底禁止，并且在编写代码后发现的成本很高。

2. 检查更改是否已被覆盖。 <a href="Compile-Time-Switches.md">编译时开关</a>和<a href="Engine-Defaults.md">引擎默认更改</a>页面记录了大量现有工作。

3. 在分支中写代码。将正在进行的工作推送到替代分支，并注明剩余内容，并让其他分叉者意识到这一点。

4. 验证：在 MSVC 下干净地编译，符合 Clang 标准，启动、关闭或技术展示项目时不会崩溃，没有垃圾日志，没有 ABI 更改。

5. 在打开拉取请求之前，先完成<a href="Coding-Guidelines.md">审核清单</a>。


## 拒绝大多数拉取请求的三个规则

* ABI稳定性

    请勿修改光线追踪负载位域、着色器可见的枚举或标志、RHI 或 RenderCore 使用的打包位掩码、反射系统位掩码定义或任何 CPU/GPU 共享结构布局。打破这些会破坏 PSO 缓存、光线追踪稳定性、序列化和跨供应商 GPU 行为。无论其余更改有多好，ABI 违规都会立即被拒绝。


* 性能基线

    更改是根据 ARM 级 ~1 GHz CPU 基准进行评估的。在桌面上进行测量并快速找到它并不能清除障碍。请参阅<a href="Performance-Targets.md">性能目标</a>。

* 版权清洁度

    贡献必须是原创作品或获得许可（MIT、Apache 2.0、BSD、Zlib）。从 UE5 复制到 4.27 分支的代码受 Epic 许可，这与宽松许可是不同的问题 -请参阅<a href="Backporting.md">向后移植</a>。


## 什么是好的贡献

适合 Vite 的贡献往往有一个共同的特征：

- **可测量。** 对代表性内容的前后分析捕获胜过争论。
- **被守护。** 交付中不需要的任何内容都位于[编译时开关](Compile-Time-Switches.md) 或控制台变量后面，并且在需要花费一些代价时默认关闭。
- **范围小。** 涉及一个子系统的变更是可以审查的。涉及渲染器、物理层和构建系统的更改则不然。
- **记录在案。** 新的控制台变量、开关和默认值需要文档页面或现有页面中的一个部分。

## 准备工作

请参阅[从源代码构建](Build-From-Source.md) 和 [工具链要求](Toolchain-Requirements.md)。请注意，引擎开发需要源构建 -[已安装的构建](Installed-Builds.md) 无法编译引擎 C++。

## 参见

- [编码指南](Coding-Guidelines.md)
- [提交约定](Commit-Conventions.md)
- [向后移植](Backporting.md)
- [从源代码构建](Build-From-Source.md)
- [性能目标](Performance-Targets.md)
