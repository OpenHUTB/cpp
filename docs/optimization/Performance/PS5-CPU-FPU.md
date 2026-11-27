# PS5 CPU FPU

!!! note "概要"
    PS5 的 Zen 2 删掉了 `FP3`，把 `FP2` 变成只做向量存储的端口，并且只保留**一条 256 位 FMA 管道**。AVX2 与 FMA 延迟不变。Series X 的 Zen 2 没有做这刀削减。Vite 在 **AMD 4700S** 上验证受 FPU 限制的工作——首当其冲是 **PhysX 3.4** 的现代化改造——这颗 CPU 正是那款进一步削减的 Zen 2，既不是桌面 Matisse 的 FPU，也不是 Series X。

4700S 进实验室是为了它的 CPU，而不是 GPU。相对 Series X 的 Zen 2，它的 Zen 2 是进一步削减的 FPU。实际用途是观察 PhysX 3.4 库里 FPU 一侧的改动在 PS5 级核心上的表现：island 求解、接触生成、约束准备与积分都是 FMA 吞吐型工作，而这正是 Vite 正在现代化改造的负载。

桌面 Ryzen 3000 的 FPU 会高估这部分工作，Series X 同样会高估，4700S 不会。

## 为什么 Series X 不是合适的 CPU 参照

Scarlett（Series X）有 **153 亿**个晶体管，Oberon（PS5）是 **106 亿**个，前者约为后者的 **1.4 倍**，常被四舍五入为「多 50%」。多出来的硅片不只是更大的 GPU（52 CU 对 36 CU）。Series X 还把更多晶体管花在 CPU 上，包括一套更完整的 Zen 2 FPU。

PS5 的 Zen 2 在整数、加载/存储与乱序执行方面基本保持 Matisse 的水平，只在 FPU 上省面积：

| 端口 | 桌面 Zen 2 / Series X 级别 | PS5 / 4700S 级别 |
|---|---|---|
| `FP0` | 256 位 FMA / FMUL | **256 位 FMA / FMUL（仅剩的 FMA 管道）** |
| `FP1` | 256 位 FMA / FMUL | 非 FMA 的浮点运算（FADD 等） |
| `FP2` | FADD + 向量存储 | **仅向量存储** |
| `FP3` | FADD | **已删除** |
| AVX2 | 256 位 | 256 位（不是用 128 位 FPU 分两拍执行 AVX） |
| FMA 延迟 | 约 5 周期 | 约 5 周期 |
| FP 物理寄存器堆 | 160 × 256 位 | 160 × 256 位 |

与同频率的 Matisse 核心相比，稳态发射情况如下：

| 操作 | 桌面 Zen 2 | PS5 / 4700S |
|---|---|---|
| 打包 FMA（`vfmadd` / `a*b+c`） | **每周期 2 × 256 位** | **每周期 1 × 256 位** |
| 作为独立操作的打包 FADD / FMUL | 各每周期 2 × 256 位 | 各每周期 1 × 256 位 |
| 向量存储 | 与 FP 集群共用端口 | 仍在 `FP2`，不占用 FMA 管道 |
| 打包整数 MUL | 1 条管道 | 不变 |
| 打包整数 ADD | 3 条管道 | 约 2 条管道 |

在 Series X 和 3700X 上，互相独立的 FMA 可以双发射；在 PS5 和 4700S 上，它们**在一条管道上排队**。FMA + FADD 不再能躲到 `FP2`/`FP3` 上。两个累加器仍能覆盖 5 周期的 FMA 延迟，但找不回第二条 FMA 单元。

!!! warning
    在 4700S 上给 PhysX 和其他受 FPU 限制的内核计时。3700X、5800X3D、13700K 或 Series X 芯片都会双发射 PS5 核心做不到的 FMA。这些芯片只用来回答「编译器是不是变差了」。

## 测试机器及各自用途

| 机器 | 用途 | 不用于 |
|---|---|---|
| **AMD 4700S** | Windows + PS5 级 CPU FPU。PhysX 3.4、动画 SIMD、音频。 | GPU。核显已熔断。 |
| **AMD 4800S** | 基于 Xbox Series X APU 构建，GPU 熔断。Series X 的 CPU 参照机，也是主机频率下全 FPU 的对照。 | PS5 的 FPU 数据，以及 GPU。把它当作任一 FPU 级别之前，先跑下面的 FMA 探针。 |
| **RX 6700 RDNA2** | 桌面 RDNA2 的编译器和吞吐参照。性能与架构足够接近，可以观察着色器编译器的行为。 | 周期精确的 PS5 GPU。PS5 的 RDNA2 存在差异（缓存、RB+、导出、图元着色器、RT ISA），这限制了 6700 抓取结果的可信范围。 |
| **AMD BC-250** | 筛选出来的 Oberon 级 APU：同一 FPU 家族，GPU **可用**。 | Vite 的 Windows / D3D12 路径。没有可用的 Windows GPU 驱动，因此不是实用的编辑器或 Shipping 测试机。 |

公开的微基准（尤其是 [Chips and Cheese](https://chipsandcheese.com/p/the-nerfed-fpu-in-ps5s-zen-2-cores)）正是用 BC-250 摸清 PS5 的 FPU。它跑不了 Vite 编辑器；4700S 可以，在削减版 FPU 上跑，由独立显卡负责 RHI。

!!! note
    主机平台代码保持私有。本页讲的是可移植 SIMD、PhysX 3.4 和一台 Windows CPU 参照机，不是 PS5 SDK 文档。参见[平台](../Platforms/Platforms.md)。

## PhysX 3.4 才是 FPU 负载

Vite 正在对 **PhysX 3.4** 库做现代化改造（更新的 Clang/MSVC、FMA 收缩、批量浮点内核），以 PS5 的 CPU 为优化目标。这套 SDK 在 `physx::shdfnd::aos`（`Vec3V`、`FloatV`、`V3ScaleAdd`、`V3MulAdd`）里对 `float` 使用 SSE2/SSE4。窄相、约束准备、PGS 求解器与积分都是打包 FMA，恰恰就是失去第二条 FMA 管道的那些内核。

Chaos 的 ISPC AVX2 路径假设的 FPU 发射带宽高于这颗核心，不适合作为这项工作的参照。整个步进的占用率仍然胜过一条从不运行的英雄式内核——参见[物理](../Physics/Physics.md)——但剩下的那条 FMA 管道，才是 SDK 重写必须落在其内的预算。

这次重写的规则：

1. **缩小的是受发射限制的打包 FMA。** 3700X 能双发射的独立 `V3ScaleAdd` / `fmadd`，现在要在一条管道上等待。保留独立的 island 与接触批次；不要在单条依赖链里指望每周期 2 次 FMA。
2. **把乘加收缩成 FMA。** `V3ScaleAdd(a, s, b)`（`a * s + b`）是一条微操作；先 `V3Scale` 再 `V3Add` 是两条微操作，还要争抢剩下的数学端口。桌面 Zen 2 能把那条加法藏到 `FP2`/`FP3` 上，PS5 不能。
3. **SoA / `Vec3V` 批处理、对齐加载。** 内层循环里的标量 `PxVec3` 会把发射带宽花在插入和标量存储上。`FP2` 仍能承担向量存储，要用起来。
4. **AVX2 合法，但找不回第二条 FMA。** 相比 SSE，256 位 FMA 让每条指令处理的浮点数翻倍，但每周期仍只能完成 1 条。不要因为「128 位 FPU」的旧传言而放弃 AVX2——相对 Matisse（256 位 FPU、管道更少），256 位与 128 位向量浮点下降的倍数大体相同。
5. **不要生成 AVX-512。** 不存在。热点 TU 用 `/arch:AVX2` 或 `-mavx2 -mfma`。除非内核明确是 AVX2，否则保持虚幻的 `VectorRegister` SSE 路径。
6. **留在 FP32。** 求解器、接触与积分都是 `float`。FP64 会让 SIMD 占用率再减半；LWC 的 double 不属于这条路径。
7. **在开启 SMT 的 4700S 上验证**（PS5 出货时 SMT 是开的）。指标是相同频率下的 `time_4700S / time_3700X`：趋近 1.0 说明你去掉了 FMA 流量；卡在 2 倍附近说明你只是加了 ILP，而多出来的那条桌面 FMA 管道原本就吃得下。

### 探针：确认只有一条 FMA 管道

在相同频率下，独立的 `_mm256_fmadd_ps` 在 4700S 上应约为 **1 周期/微操作**，在桌面 Zen 2 上约为 **0.5 周期/微操作**。依赖链（同一累加器）在两者上都应约为 **5 周期/微操作**。如果「提升」只出现在依赖链测试里，你只是藏住了延迟，并没有找回 PS5 的 FPU 吞吐。

```cpp
#include <immintrin.h>
#include <stdint.h>

#if defined(_MSC_VER)
#include <intrin.h>
static uint64_t Ticks() { return __rdtsc(); }
#else
static uint64_t Ticks()
{
	uint32_t Lo, Hi;
	__asm__ volatile("rdtsc" : "=a"(Lo), "=d"(Hi));
	return (uint64_t(Hi) << 32) | Lo;
}
#endif

float ProbeFmaThroughput(uint32_t Iters)
{
	__m256 A = _mm256_set1_ps(1.0001f);
	__m256 B = _mm256_set1_ps(1.0002f);
	__m256 Acc0 = _mm256_set1_ps(1.0f);
	__m256 Acc1 = _mm256_set1_ps(1.0f);
	__m256 Acc2 = _mm256_set1_ps(1.0f);
	__m256 Acc3 = _mm256_set1_ps(1.0f);

	const uint64_t T0 = Ticks();
	for (uint32_t I = 0; I < Iters; ++I)
	{
		Acc0 = _mm256_fmadd_ps(Acc0, A, B);
		Acc1 = _mm256_fmadd_ps(Acc1, A, B);
		Acc2 = _mm256_fmadd_ps(Acc2, A, B);
		Acc3 = _mm256_fmadd_ps(Acc3, A, B);
	}
	const uint64_t Dt = Ticks() - T0;

	Acc0 = _mm256_add_ps(Acc0, Acc1);
	Acc0 = _mm256_add_ps(Acc0, Acc2);
	Acc0 = _mm256_add_ps(Acc0, Acc3);
	alignas(32) float Out[8];
	_mm256_store_ps(Out, Acc0);

	(void)Out;
	return float(Dt) / float(Iters * 4u); // ~1.0 on 4700S, ~0.5 on Matisse
}
```

这个 TU 用 `/arch:AVX2` 或 `-mavx2 -mfma` 编译。

### 求解器形状的 FMA：用 `V3ScaleAdd`，而不是先乘后加

PhysX 3.4 已经有融合形式，直接用。对一个批量法线冲量做 PGS 增量，每条 lane 就是一次 FMA；拆开会浪费 4700S 没有多余可用的那条管道。

```cpp
#include "PsVecMath.h"
using namespace physx::shdfnd::aos;

// deltaV = deltaV + n * (impulse * invMass)   — one FMA, not V3Scale + V3Add
inline void ApplyNormalImpulse(
	Vec3V& DeltaLin,
	const Vec3V N,
	const FloatV Impulse,
	const FloatV InvMass)
{
	const FloatV S = FMul(Impulse, InvMass);
	DeltaLin = V3ScaleAdd(N, S, DeltaLin);
}

// BAD on this FPU: two math uops, FADD can no longer hide on FP2/FP3
inline void ApplyNormalImpulseSplit(
	Vec3V& DeltaLin,
	const Vec3V N,
	const FloatV Impulse,
	const FloatV InvMass)
{
	const Vec3V Tmp = V3Scale(N, FMul(Impulse, InvMass));
	DeltaLin = V3Add(DeltaLin, Tmp);
}
```

积分同理：`x += v * dt` 是 `V3ScaleAdd`，而不是标量 `PxVec3` 循环。`FP2` 仍然接住向量存储。

```cpp
void IntegrateLinearSoA(
	PxF32* RESTRICT Px, PxF32* RESTRICT Py, PxF32* RESTRICT Pz,
	const PxF32* RESTRICT Vx, const PxF32* RESTRICT Vy, const PxF32* RESTRICT Vz,
	PxF32 Dt,
	PxU32 Count)
{
	const FloatV VDt = FLoad(Dt);
	const PxU32 Padded = Count & ~3u;
	PxU32 I = 0;

	for (; I < Padded; I += 4)
	{
		Vec3V P = V3LoadA(Px + I); // SoA: load X of four bodies; same for Y/Z in real code
		Vec3V V = V3LoadA(Vx + I);
		P = V3ScaleAdd(V, VDt, P);
		V3StoreA(P, Px + I);
		(void)Py; (void)Pz; (void)Vy; (void)Vz;
	}

	for (; I < Count; ++I)
	{
		Px[I] += Vx[I] * Dt;
		Py[I] += Vy[I] * Dt;
		Pz[I] += Vz[I] * Dt;
	}
}
```

上面的 SoA 示例每次调用只加载一个分量；真正的内核会用三条 `Vec3V` lane，或四刚体的 AoSoA 块。要点是 FMA + 对齐向量存储，而不是这个玩具布局。

### 不要放进 SDK 热路径的东西

```cpp
// Scalar PxVec3 in the solver inner loop — insert/extract plus scalar stores
for (PxU32 I = 0; I < Count; ++I)
{
	PxVec3 V = Bodies[I].linearVelocity;
	V.z += gravityZ * dt;
	Bodies[I].linearVelocity = V;
}

// Separate mul and add when V3ScaleAdd exists
Vec3V Tmp = V3Scale(N, Impulse);
Delta = V3Add(Delta, Tmp);

// FP64 in a per-body kernel
PxF64 X = PxF64(p.x) + PxF64(v.x) * dt;

// Assuming AVX2 restores Series X / Matisse dual FMA
// Acc0 and Acc1 independent fmadd — desktop/SX: 2/cycle; PS5/4700S: 1/cycle
```

## 如何在 4700S 上测试 PhysX FPU 工作

### 针对 PS5 CPU 验证 PhysX 3.4 的 FPU 改动

1. 守住影响全局代码生成的改动。参见[编译时开关](Compile-Time-Switches.md)。

2. 在 4700S 和一台桌面 Zen 2 机器上，用相同的 ISA 标志构建现代化后的 PhysX 3.4 库以及引擎的 Test/Shipping 目标（内核 TU 用 `/arch:AVX2` 或 `-mavx2 -mfma`）。

3. 跑 FMA 探针。在信任这台机器之前，先确认 4700S 上每个独立 FMA 约为 1 周期。

4. 给真实的 SDK 路径计时：`stat physics`、island/接触/求解器拆分、ETW。不要把 13700K 上的 Insights 抓取当作 PS5 的数据。

5. 报告相同频率、开启 SMT 下的 `time_4700S / time_3700X`。趋近 1.0：FMA 流量减少；卡在 2 倍附近：多出来的 ILP 只有桌面第二条 FMA 管道吃得下。

6. 在 RX 6700 上以 GPU 受限的方式重跑场景。如果帧时间没有变化，说明场景并不受 CPU-FPU 限制——只有当 4700S 的物理抓取仍然需要这次 SDK 改动时才保留它。

如果测试机有睿频，请锁频。不要把 8C/16T 的 4700S 数据拿去和 16 核桌面 CPU 比较，然后称之为 FPU 差异。

## GPU 测试仍在 6700 上进行

4700S 和 4800S 回答不了 GPU 编译器的问题，它们的 GPU 被完全削减。

RX 6700 在性能与架构上接近 PS5 的 RDNA2，足以观察 **GPU 编译器行为**。PS5 的 RDNA2 与 PC RDNA2 仍有差异（缓存层级、RB+、导出路径、图元着色器、RT 求交 ISA、末级缓存），这些差异决定了 6700 的 trace 能在多大程度上代表主机 GPU。

BC-250 的 GPU 可用，且是 Oberon 级硅片，但无法用于 Windows D3D12，这让实际的 Vite 编辑器与 Shipping 测试无法进行。不要围绕它规划 GPU 验证。

## 另请参阅

- [PhysX](../Physics/PhysX.md)
- [物理](../Physics/Physics.md)
- [系统要求](../GettingStarted/System-Requirements.md)
- [性能目标](../EngineOverview/Performance-Targets.md)
- [性能分析](Profiling.md)
- [编码指南](../Contributing/Coding-Guidelines.md)
- [平台](../Platforms/Platforms.md)
