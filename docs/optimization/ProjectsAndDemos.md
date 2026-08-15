# Projects and Demos

<tldr>
<p>
Downloadable projects demonstrating Vite's features and establishing its performance numbers. Several are
the benchmarks the <a href="./EngineOverview/Performance-Targets.md">performance targets</a> are measured against.
</p>
</tldr>

These fall into two groups: **showcases** that demonstrate what the engine can do visually, and
**benchmarks** that establish what it costs. The benchmarks are the more useful of the two if you are
deciding whether Vite fits your project.

## Showcases

| Project | Demonstrates |
|---|---|
| [Tech Demo Project](Tech-Demo-Project.md) | DDGI, SSGI, Apex Destruction and Apex Cloth in one package |
| [Stylized Raytracing Demo](Stylized-Raytracing-Demo.md) | Ray tracing applied to non-photoreal art direction |
| [Callisto BRDF Demos](Callisto-BRDF-Demos.md) | The custom skin and character shading model |
| [Abandoned Apartment](Abandone-Apartment.md) | Interior lighting scene |
| [Attic Scene](Attic-Scene.md) | Interior lighting scene |
| [Crash Bandicoot: Timetwister](Crash-Bandicoot.md) | A complete gameplay level built on UE4 |

## Benchmarks

| Project | Measures |
|---|---|
| [400 Characters CMC Bench](400-Characters-CMC-Bench.md) | Character Movement Component cost at scale |
| [Physics Cube Bench](Physics-Cube-Bench.md) | PhysX rigid body throughput |
| [PhysX Test](PhysXTest.md) | General PhysX behaviour and stability |
| [PhysX Instanced Subsystem](PhysX-Instanced-Subsystem.md) | Instanced rigid bodies at counts individual actors cannot reach |

## Using the benchmarks

The benchmarks are more informative than a frame rate number suggests. Run them with `stat unit` visible
and note which thread is the limit &mdash; that tells you what a similar workload will cost you, and which
of Vite's optimisations apply. See [Profiling](Profiling.md).

<note>
Numbers from these projects reflect the hardware they were captured on. Vite's
<a href="Coding-Guidelines.md">performance baseline</a> is an ARM-class ~1 GHz CPU, which is far below a
development desktop. A benchmark that is comfortable on your machine may not be on your target.
</note>

## See also

- [Performance Targets](Performance-Targets.md)
- [Profiling](Profiling.md)
- [UE4 versus UE5 Cost Analysis](UE4-Versus-UE5-Cost-Analysis.md)
