# PS5 CPU FPU

!!! note "概要"
    PS5 的 Zen 2 删除了 `FP3`，把 `FP2` 降为只负责向量存储，并且只保留**一条 256 位 FMA 管道**。AVX2 与 FMA 延迟保持不变，而 Series X 的 Zen 2 没有做这刀削减。Vite 在 **AMD 4700S** 上验证受 FPU 限制的工作——其中主要是 **PhysX 3.4** 的现代化改造——这颗 CPU 搭载的正是进一步削减后的 Zen 2，既不是桌面的 Matisse，也不是 Series X。

4700S 进实验室是为了它的 CPU，不是为了 GPU：相对 Series X 的 Zen 2，它搭载的是进一步削弱 FPU 的那一版。它的实际用途，是观察 PhysX 3.4 库中 FPU 一侧的改动在 PS5 级核心上的表现。island 求解、接触生成、约束准备与积分都属于受 FMA 吞吐限制的工作，而这正是 Vite 现代化改造所针对的负载。

桌面 Ryzen 3000 的 FPU 会高估这部分工作，Series X 同样会高估，4700S 不会。

## 为什么 Series X 不是合适的 CPU 参照

Scarlett（Series X）的晶体管数量为 **153 亿**，Oberon（PS5）为 **106 亿**，前者约为后者的 **1.4 倍**，常被粗略说成「多 50%」。多出来的硅片并不只是更大的 GPU（52 CU 对 36 CU）：Series X 还把更多晶体管用在了 CPU 上，其中就包括一套更完整的 Zen 2 FPU。

PS5 的 Zen 2 在整数、加载/存储与乱序执行方面基本维持 Matisse 的水平，只在 FPU 上缩减了面积：

| 端口 | 桌面 Zen 2 / Series X 级别 | PS5 / 4700S 级别 |
|---|---|---|
| `FP0` | 256 位 FMA / FMUL | **256 位 FMA / FMUL（仅剩的 FMA 管道）** |
| `FP1` | 256 位 FMA / FMUL | 非 FMA 的浮点运算（FADD 等） |
| `FP2` | FADD + 向量存储 | **仅向量存储** |
| `FP3` | FADD | **已删除** |
| AVX2 | 256 位 | 256 位（不是用 128 位 FPU 分两拍执行 AVX） |
| FMA 延迟 | 约 5 周期 | 约 5 周期 |
| FP 物理寄存器堆 | 160 × 256 位 | 160 × 256 位 |

与同频 Matisse 核心相比，稳态发射情况如下：

| 操作 | 桌面 Zen 2 | PS5 / 4700S |
|---|---|---|
| 打包 FMA（`vfmadd` / `a*b+c`） | **每周期 2 × 256 位** | **每周期 1 × 256 位** |
| 作为独立操作的打包 FADD / FMUL | 各每周期 2 × 256 位 | 各每周期 1 × 256 位 |
| 向量存储 | 与 FP 集群共用端口 | 仍在 `FP2`，不占用 FMA 管道 |
| 打包整数 MUL | 1 条管道 | 不变 |
| 打包整数 ADD | 3 条管道 | 约 2 条管道 |

互相独立的 FMA 在 Series X 与 3700X 上可以双发射；在 PS5 与 4700S 上，它们**只能在一条管道上排队**。FMA 与 FADD 也不再能躲到 `FP2`/`FP3` 上。两个累加器仍足以覆盖 5 周期的 FMA 延迟，但找不回第二条 FMA 单元。

!!! warning
    给 PhysX 以及其他受 FPU 限制的内核计时，请在 4700S 上进行。3700X、5800X3D、13700K 或 Series X 芯片都会双发射 PS5 核心做不到的 FMA；这些芯片只用来回答一个问题：编译器是不是变差了。

## 测试机及其用途

| 机器 | 用途 | 不用于 |
|---|---|---|
| **AMD 4700S** | Windows + PS5 级 CPU FPU。用于 PhysX 3.4、动画 SIMD、音频。 | GPU。核显已被屏蔽（fused off）。 |
| **AMD 4800S** | 基于 Xbox Series X APU 构建，GPU 已屏蔽。既是 Series X 的 CPU 参照机，也是主机频率下全 FPU 的对照。 | PS5 的 FPU 数据，以及 GPU。把它归入某一级 FPU 之前，先跑下方的 FMA 探针。 |
| **RX 6700 RDNA2** | 桌面 RDNA2 的编译器与吞吐参照。性能与架构足够接近，可以捕捉着色器编译器的行为。 | 周期精确的 PS5 GPU。PS5 的 RDNA2 存在差异（缓存、RB+、导出、图元着色器、RT ISA），这限制了 6700 抓取结果作为主机 GPU 参照的可信范围。 |
| **AMD BC-250** | 由 Oberon 级晶圆筛选而来的 APU：同一 FPU 家族，GPU **可用**。 | Vite 的 Windows / D3D12 路径。没有可用的 Windows GPU 驱动，因此不能作为实用的编辑器或 Shipping 测试机。 |

公开微基准之所以能摸清 PS5 的 FPU，靠的就是 BC-250（尤见 [Chips and Cheese](https://chipsandcheese.com/p/the-nerfed-fpu-in-ps5s-zen-2-cores)）。它跑不了 Vite 编辑器；4700S 可以，在削减后的 FPU 上运行，由独立显卡负责 RHI。

!!! note
    主机平台代码保持不公开。本页讨论的是可移植 SIMD、PhysX 3.4 与一台 Windows CPU 参照机，不是 PS5 SDK 文档。参见[平台](../Platforms/Platforms.md)。

## PhysX 3.4 才是 FPU 负载

Vite 正在对 **PhysX 3.4** 库做现代化改造（更新的 Clang/MSVC、FMA 收缩、批量浮点内核），优化目标是 PS5 的 CPU。这套 SDK 通过 SSE2/SSE4 处理 `float`，封装在 `physx::shdfnd::aos`（`Vec3V`、`FloatV`、`V3ScaleAdd`、`V3MulAdd`）中。窄相、约束准备、PGS 求解器与积分都是打包 FMA，恰恰就是丢掉第二条 FMA 管道的那些内核。

Chaos 的 ISPC AVX2 路径假设这颗核心拥有更高的 FPU 发射带宽，因此不适合作为这项工作的参照。整个模拟步的占用率仍然胜过一条从不执行的英雄式内核——参见[物理](../Physics/Physics.md)——但 SDK 重写必须把预算压在剩下的这一条 FMA 管道之内。

重写遵循以下规则：

1. **被削减的是受发射限制的打包 FMA。** 3700X 能双发射的独立 `V3ScaleAdd` / `fmadd`，现在只能排队等一条管道。保留互不依赖的 island 与接触批次；不要指望单条依赖链里出现每周期 2 次 FMA。
2. **把乘加收缩为 FMA。** `V3ScaleAdd(a, s, b)`（即 `a * s + b`）是一条微操作；先 `V3Scale` 再 `V3Add` 是两条微操作，而且要争抢剩下的数学端口。桌面 Zen 2 能把那条加法藏到 `FP2`/`FP3` 上，PS5 不能。
3. **使用 SoA / `Vec3V` 批处理与对齐加载。** 内层循环里的标量 `PxVec3` 会把发射带宽消耗在插入与标量存储上。`FP2` 仍能承担向量存储，要用起来。
4. **AVX2 可以使用，但它找不回第二条 FMA。** 相比 SSE，256 位 FMA 让每条指令处理的浮点数翻倍，每周期却仍只能完成 1 条。不要因为「128 位 FPU」的旧传言而弃用 AVX2——相对 256 位 FPU、管道更少的 Matisse，256 位与 128 位向量浮点的下降幅度大体相当。
5. **不要生成 AVX-512。** 这颗核心不支持。热点 TU 使用 `/arch:AVX2` 或 `-mavx2 -mfma`；除非内核明确按 AVX2 编写，否则沿用虚幻的 `VectorRegister` SSE 路径。
6. **坚持 FP32。** 求解器、接触与积分都是 `float`。FP64 会让 SIMD 占用率再减半；LWC 的 double 不属于这条路径。
7. **在开启 SMT 的 4700S 上验证**（PS5 出货时 SMT 是开启的）。指标是相同频率下的 `time_4700S / time_3700X`：趋近 1.0，说明 FMA 流量确实减少了；停在 2 倍附近，说明你只是增加了 ILP，而这部分正好被桌面多出来的那条 FMA 管道吸收。

### 探针：确认只有一条 FMA 管道

在相同频率下，互相独立的 `_mm256_fmadd_ps` 在 4700S 上应约为 **1 周期/微操作**，在桌面 Zen 2 上约为 **0.5 周期/微操作**；依赖链（同一累加器）在两者上都应约为 **5 周期/微操作**。如果「收益」只出现在依赖链测试中，说明你只是隐藏了延迟，并没有恢复 PS5 的 FPU 吞吐。

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

该 TU 用 `/arch:AVX2` 或 `-mavx2 -mfma` 编译。

### 求解器内的 FMA 写法：用 `V3ScaleAdd`，不要先乘后加

PhysX 3.4 已经提供融合形式，直接使用即可。对批量法线冲量做 PGS 增量时，每条 lane 只需一次 FMA；把它拆开，就会白白浪费 4700S 上本就不富余的那条管道。

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

积分同理：`x += v * dt` 应写成 `V3ScaleAdd`，而不是标量 `PxVec3` 循环；向量存储仍由 `FP2` 承担。

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

上面的 SoA 示例每次调用只加载一个分量；真实内核会使用三条 `Vec3V` lane，或四刚体的 AoSoA 块。要点是 FMA 与对齐向量存储，而不是这个示例布局。

### 不要放进 SDK 热路径的写法

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

1. 先确认会影响全局代码生成的改动。参见[编译时开关](Compile-Time-Switches.md)。
2. 在 4700S 与一台桌面 Zen 2 机器上，用完全相同的 ISA 标志构建现代化后的 PhysX 3.4 库与引擎的 Test/Shipping 目标（内核 TU 使用 `/arch:AVX2` 或 `-mavx2 -mfma`）。
3. 运行 FMA 探针：在信任这台机器之前，先确认 4700S 上每个独立 FMA 约为 1 周期。
4. 给真实的 SDK 路径计时：`stat physics`、island/接触/求解器拆分、ETW。不要把 13700K 上的 Insights 抓取当作 PS5 的数据。
5. 报告相同频率、开启 SMT 下的 `time_4700S / time_3700X`。趋近 1.0：FMA 流量减少；停在 2 倍附近：多出的 ILP 只被桌面第二条 FMA 管道吸收。
6. 让场景在 RX 6700 上以 GPU 受限的方式重跑。如果帧时间没有变化，说明场景并不受 CPU-FPU 限制；只有当 4700S 的物理抓取仍然需要这次改动时，才保留 SDK 改动。

若测试机有睿频，请锁定频率。不要把 8C/16T 的 4700S 数据与 16 核桌面 CPU 相比，再把差异归因于 FPU。

## GPU 测试仍在 6700 上进行

4700S 与 4800S 回答不了 GPU 编译器的问题：它们的 GPU 已被完全屏蔽。

RX 6700 在性能与架构上接近 PS5 的 RDNA2，足以观察 **GPU 编译器行为**。PS5 的 RDNA2 与 PC RDNA2 仍有差异（缓存层级、RB+、导出路径、图元着色器、RT 求交 ISA、末级缓存），这些差异决定了 6700 的 trace 能在多大程度上替代主机 GPU。

BC-250 的 GPU 可用，且是 Oberon 级硅片，但无法用于 Windows D3D12，因此无法进行实际的 Vite 编辑器与 Shipping 测试。不要围绕它规划 GPU 验证。

## 另请参阅

- [PhysX](../Physics/PhysX.md)
- [物理](../Physics/Physics.md)
- [系统要求](../GettingStarted/System-Requirements.md)
- [性能目标](../EngineOverview/Performance-Targets.md)
- [性能分析](Profiling.md)
- [编码指南](../Contributing/Coding-Guidelines.md)
- [平台](../Platforms/Platforms.md)
