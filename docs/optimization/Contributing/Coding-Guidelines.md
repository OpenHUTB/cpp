# 编码指南


稳定性第一、注重性能、严格遵守 Clang 22、版权清洁、ABI 安全。没有递归，没有新的虚拟，没有新的蓝图曝光，最少的模板。在 `CoreDefines.h` 中控制任何增加开销或改变守卫背后行为的内容。


这些规则比虚幻自己的编码标准更严格。它们的存在是因为 Vite 的价值主张是在控制台类硬件上的可预测性能，并且因为破坏与其自身着色器的二进制兼容性的分叉是无法使用的。

## 核心原则

* **稳定第一**

    在 MSVC 和 Clang 下，代码必须保持可预测性和安全性。碰巧在一个编译器上运行的行为是不可接受的。

* **注重性能**

    所有更改都必须考虑 CPU、GPU 和内存影响，并以 ARM 级 ~1 GHz CPU 作为基准目标。在本地桌面 CPU 上测量的性能不得被视为充分或代表现实世界的目标硬件。

* **严格遵守 Clang 22**

    没有 MSVC 特定的行为、编译器扩展或未定义的构造。仅因为 MSVC 允许而构建的代码不符合此标准，即使 MSVC 是您个人使用的编译器也是如此。

* **许可清晰**

    所有贡献都必须不受版权保护。仅当代码是贡献者的原创作品或获得许可（MIT、Apache 2.0、BSD、Zlib）时，才可以包含代码。


* **ABI安全**

    请勿修改引擎位掩码布局、有效负载结构、序列化格式或任何 CPU/GPU 共享的二进制合约。


## 禁止

### 递归

被禁止。使用迭代模式。递归使堆栈使用不受限制且不可预测，这对硬件 Vite 目标很重要。

### 新的虚函数

除非严格要求，否则不要添加新的虚拟。在可能的情况下，缓存现有的引擎虚拟，而不是重复调用它们。

### Kismet /蓝图 API 添加

未经明确批准，不得使用新的“BlueprintCallable”或“BlueprintPure”函数。每个蓝图公开的函数都会添加反射元数据、二进制大小和比本机 C++ 慢的调用路径。在同一类型中，Blueprint Nativization 系统需要保持稳定。

### 大量模板模式

避免使用模板，除非它们提供明确的性能或架构优势。不要引入会增加编译时间、二进制大小或代码复杂性的模式。请参阅[Shader 编译和 PSO](../Performance/Shader-Compilation-And-PSO.md) 了解为什么这里要认真对待编译时间。

### 破坏 ABI 的修改

!!! 警告
    请勿修改以下任何内容：

    * 光线追踪有效负载位域

    * 着色器可见的枚举或标志

    * RHI 或 RenderCore 使用的打包位掩码

    * 反射系统位掩码定义

    * 任何 CPU/GPU 共享结构布局

    此类修改会破坏 PSO 缓存、光线追踪稳定性、序列化以及跨平台和跨供应商 GPU 行为。这是立即拒绝的最常见原因。


## 技术标准

1. 没有未定义的行为或非标准扩展。
2. 避免隐式类型转换。
3. 保持内存的正确性。避免大的堆栈分配；最小化动态内存。
4. 更喜欢 `constexpr`、`FORCEINLINE` 和零成本抽象。

## 性能规则

1. 尽量使用连续内存和有利于缓存的布局。
2. 避免热点路径中出现不可预测的分支。
3. 每帧循环内没有`FString`处理、反射调用、动态分配或虚拟调度。
4. 在适当的情况下使用 SIMD 友好的数学（SSE/AVX/NEON）。

规则 3 是最常被其他合理代码违反的规则。每个参与者节拍中的`FString`格式调用在测试场景中是不可见的，并且在规模上是灾难性的 -请参阅 [400 角色 CMC 基准测试](400-Characters-CMC-Bench.md)，了解相乘后的每帧成本。

## 正在进行中的工作

1. 将正在进行的工作提交给替代分支。
2. 对剩下要做的事情做一个简短的评论。
3. 不再需要临时分支后将其删除。
4. 让其他分叉者了解这项工作，这样就不会重复工作。

## 验证要求

### 编译

必须在 MSVC 下干净编译，并符合 Clang 标准。“在我的机器上编译时有警告”不属于干净编译。

### 稳定性

- 启动、关闭或运行技术展示项目时不得崩溃。
- 不得产生大量日志。
- 保护所有非发行必需的代码。

### ABI

- 所有着色器可见的结构体必须保持逐位一致。
- 不得更改有效载荷、位掩码布局、统一缓冲区或反射标志。
- 违反 ABI 将导致立即拒绝。

### 门控代码

任何增加系统开销或改变现有行为的代码都必须置于 [Engine\Source\Runtime\Core\Public\Misc\CoreDefines.h](https://github.com/OpenHUTB/engine/blob/hutb/Engine/Source/Runtime/Core/Public/Misc/CoreDefines.h) 中定义的保护机制之后：

```c++
#ifndef VITE_MY_FEATURE
	#define VITE_MY_FEATURE 0
#endif
```

由此可得出两条规则。默认值必须保持现有路径不变——默认值会改变行为的保护机制就不能称之为保护机制。此外，当开关关闭时，受保护的代码必须真正被排除，而不是被编译进去并在运行时跳过，否则你原本设置的限制开销仍然会产生。


这使得可选功能对不使用它们的项目来说是免费的，也正是 `VITE_PHYSX_FIXED_TIMESTEP` 和其他[编译时开关](Compile-Time-Switches.md)以这种形式存在的原因。请在同一页面上记录每个新增的开关。


## 审查清单

在打开拉取请求之前确认所有这些：

| 查看 | |
|---|---|
| 无递归 | ☐ |
| 不添加虚拟调用，尽可能缓存现有引擎虚拟 | ☐ |
| 没有新的 Kismet 曝光 | ☐ |
| 版权干净的代码 | ☐ |
| 保留 ABI 和位掩码完整性 | ☐ |
| 严格遵守 Clang | ☐ |
| 没有未定义的行为 | ☐ |
| 无性能回退 | ☐ |
| 不会增加不必要的二进制大小 | ☐ |
| 没有着色器或 RHI ABI 不匹配 | ☐ |
| 开销或行为更改在[CoreDefines.h](https://github.com/OpenHUTB/engine/blob/hutb/Engine/Source/Runtime/Core/Public/Misc/CoreDefines.h)中门控，默认关闭 | ☐ |
| 新的编译时开关记录在文档中 | ☐ |

## 标记您的更改

Vite 的引擎修改以内联方式标记，以便它们能够保留在上游合并之后，并便于后续查找。代码树中现有的约定是在修改的行尾添加注释：

```c++
case MP_Anisotropy:
    CustomPinNames.Add({ MSM_CallistoBRDF, "Diffuse Fresnel" }); // AKCHANGES
    CustomPinNames.Add({ MSM_Toon, "Softness" }); //Fletch
```

遵循周围代码使用的任何标记。对于新区域，`//AKCHANGES START`/ `//AKCHANGES END`对可以明确更改的范围。

## 另请参阅

- [贡献](Contributing.md)
- [提交约定](Commit-Conventions.md)
- [向后移植](Backporting.md)
- [编译时开关](../Performance/Compile-Time-Switches.md)
- [性能目标](../EngineOverview/Performance-Targets.md)
